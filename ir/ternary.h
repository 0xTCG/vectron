#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"
#include "codon/parser/visitors/typecheck/typecheck.h"


namespace vectron {

using namespace codon::ir;

class TernaryVec : public transform::OperatorPass {
  const std::string KEY = "ternary-vec";
  std::string getKey() const override { return KEY; }

  Value *ternToCall(Value *v) {
    auto ternVal = cast<TernaryInstr>(v);
    assert(ternVal);

    auto values = ternVal->getCond()->getUsedValues();
    if ( !values.size() )
      return v;

    auto *M        = ternVal->getModule();
    auto *cond     = values[0];
    auto *trueVal  = ternVal->getTrueValue();
    auto *falseVal = ternVal->getFalseValue();

    auto generics = cond->getType()->getGenerics();
    if ( generics.size() != 2 )
      return v;

    auto *vecType = M->getOrRealizeType(
      codon::ast::getMangledClass("std.experimental.simd", "Vec"),
      generics);

    if ( !vecType || !cond->getType()->is(vecType) )
      return v;

    auto *ternTrueVal  = cast<TernaryInstr>(trueVal);
    auto *ternFalseVal = cast<TernaryInstr>(falseVal);
    if ( ternTrueVal )
      trueVal = ternToCall(ternTrueVal);
    if ( ternFalseVal )
      falseVal = ternToCall(ternFalseVal);

    auto *ternaryHelper = M->getOrRealizeFunc("ternary", {cond->getType(), trueVal->getType(), falseVal->getType()}, {}, "std.lib");
    assert(ternaryHelper);

    auto *ternaryCall = util::call(ternaryHelper, {cond, trueVal, falseVal});
    assert(ternaryCall);

    return ternaryCall;
  }

  void handle(CallInstr *v) override {
    auto *pf = getParentFunc();
    if ( !bool(pf) || !util::hasAttribute(pf, "std.vectron.attributes.vectron.0:0") )
      return;

    auto *M = v->getModule();
    std::vector<Value*> args;
    std::vector<types::Type*> argsTypes;
    bool ternFound = false;
    for ( auto it = v->begin(); it != v->end(); ++it ) {
      bool check = (*it)->is<TernaryInstr>();
      ternFound |= check;
      auto arg = check ? ternToCall(*it) : *it;
      args.push_back(arg);
      argsTypes.push_back(arg->getType());
    }

    if ( !ternFound )
      return;

    auto *newCallerHelper = M->getOrRealizeMethod(
      v->getType(),
      util::getFunc(v->getCallee())->getUnmangledName(),
      argsTypes
    );
    assert(newCallerHelper);

    auto *newCaller = util::call(newCallerHelper, args);
    assert(newCaller);

    v->replaceAll(newCaller);
  }
};

} // namespace sequre
