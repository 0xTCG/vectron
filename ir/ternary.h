#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"
#include "codon/parser/visitors/typecheck/typecheck.h"

namespace vectron {

using namespace codon;
using namespace codon::ir;

class TernaryVec : public transform::OperatorPass {
  const std::string KEY = "ternary-vec";
  std::string getKey() const override { return KEY; }

  Value *ternToCall(Value *v, const std::vector<types::Generic> &ternaryGenerics) {
    auto ternVal = cast<TernaryInstr>(v);
    assert(ternVal);

    auto values = ternVal->getCond()->getUsedValues();
    if (!values.size())
      return v;

    auto *M = ternVal->getModule();
    auto *cond = values[0];
    auto *trueVal = ternVal->getTrueValue();
    auto *falseVal = ternVal->getFalseValue();

    auto generics = cond->getType()->getGenerics();
    if (generics.size() != 2)
      return v;

    auto *vecType = M->getOrRealizeType(
        codon::ast::getMangledClass("std.experimental.simd", "Vec"), generics);

    if (!vecType || !cond->getType()->is(vecType))
      return v;

    auto *ternTrueVal = cast<TernaryInstr>(trueVal);
    auto *ternFalseVal = cast<TernaryInstr>(falseVal);
    if (ternTrueVal)
      trueVal = ternToCall(ternTrueVal, ternaryGenerics);
    if (ternFalseVal)
      falseVal = ternToCall(ternFalseVal, ternaryGenerics);

    auto *ternaryHelper = M->getOrRealizeFunc(
        "ternary", {cond->getType(), trueVal->getType(), falseVal->getType()},
        ternaryGenerics, "std.vectron.lib");
    assert(ternaryHelper);

    auto *ternaryCall = util::call(ternaryHelper, {cond, trueVal, falseVal});
    assert(ternaryCall);

    return ternaryCall;
  }

  void handle(CallInstr *v) override {
    auto *M = v->getModule();
    if (v->getType()->is(M->getNoneType()))
      return;

    auto *pf = getParentFunc();
    auto vectronAttr = codon::ast::getMangledFunc("std.vectron.__init__", "vectron");
    if (!bool(pf) || !util::hasAttribute(pf, vectronAttr))
      return;
    auto attr = pf->getAttribute<KeyValueAttribute>()->get(vectronAttr);
    auto cache = v->getModule()->getCache();
    auto dtype = cache->typeCtx->forceFind("int")->getType()->getClass();
    auto mode = 1;
    auto lane_size = 8;
    auto threads = 1;
    if (!attr.empty()) {
      auto f = cache->typeCtx->forceFind(attr)->getType();
      auto tv = ast::TypecheckVisitor(cache->typeCtx);
      lane_size = tv.extractFuncGeneric(f, 0)->getIntStatic()->value;
      dtype = tv.extractFuncGeneric(f, 1)->getClass();
      threads = tv.extractFuncGeneric(f, 2)->getIntStatic()->value;
      mode = tv.extractFuncGeneric(f, 3)->getIntStatic()->value;
    }
    seqassertn(dtype && in(cache->getClass(dtype)->realizations,
                           dtype->getClass()->realizedName()),
               "{} not realized", dtype ? dtype->debugString(0) : "<null>");
    auto vectronParams = std::vector<types::Generic>{
        types::Generic{lane_size},
        types::Generic{cache->getClass(dtype)
                           ->realizations[dtype->getClass()->realizedName()]
                           ->ir}
    };

    std::vector<Value *> args;
    std::vector<types::Type *> argsTypes;
    bool ternFound = false;
    for (auto it = v->begin(); it != v->end(); ++it) {
      bool check = (*it)->is<TernaryInstr>();
      ternFound |= check;
      auto arg = check ? ternToCall(*it, vectronParams) : *it;
      args.push_back(arg);
      argsTypes.push_back(arg->getType());
    }

    if (!ternFound)
      return;

    auto *newCallerHelper = M->getOrRealizeMethod(
        v->getType(), util::getFunc(v->getCallee())->getUnmangledName(), argsTypes);
    assert(newCallerHelper);

    auto *newCaller = util::call(newCallerHelper, args);
    assert(newCaller);

    v->replaceAll(newCaller);
  }
};

} // namespace vectron
