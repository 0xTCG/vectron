#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"
#include "codon/parser/visitors/typecheck/typecheck.h"


namespace vectron {

using namespace codon::ir;

std::vector<codon::ast::types::TypePtr>
translateGenerics(codon::ast::Cache *cache, std::vector<types::Generic> &generics) {
  std::vector<codon::ast::types::TypePtr> ret;
  for (auto &g : generics) {
    seqassertn(g.isStatic() || g.getTypeValue(), "generic must be static or a type");
    if (g.isStaticStr())
      ret.push_back(std::make_shared<codon::ast::types::LinkType>(
          std::make_shared<codon::ast::types::StrStaticType>(
              cache, g.getStaticStringValue())));
    else if (g.isStatic())
      ret.push_back(std::make_shared<codon::ast::types::LinkType>(
          std::make_shared<codon::ast::types::IntStaticType>(cache,
                                                             g.getStaticValue())));
    else
      ret.push_back(std::make_shared<codon::ast::types::LinkType>(
          g.getTypeValue()->getAstType()));
  }
  return ret;
}

codon::ir::types::Type *realizeTypePatch(
  codon::ast::Cache *cache,
  codon::ast::types::ClassType *type,
  const std::vector<codon::ast::types::TypePtr> &generics) {
    std::cout << "DEBUG realizeTypePatch 0" << std::endl;
    auto tv = codon::ast::TypecheckVisitor(cache->typeCtx);
    std::cout << "DEBUG realizeTypePatch 1" << std::endl;

    auto temp1 = cache->castVectorPtr(generics);
        std::cout << "DEBUG realizeTypePatch 2" << std::endl;
    auto temp2 = tv.instantiateType(type, temp1);
    std::cout << "DEBUG realizeTypePatch 3" << std::endl;
    auto rtv = tv.realize(temp2);

    if (rtv) {
      std::cout << "DEBUG realizeTypePatch 4" << std::endl;
      return cache->classes[rtv->getClass()->name]
        .realizations[rtv->getClass()->realizedName()]
        ->ir;
  }
  return nullptr;
}

types::Type *getOrRealizeTypePatch(
  Module *M,
  const std::string &typeName,
                                      std::vector<types::Generic> generics) {
  std::cout << "DEBUG getOrRealizeType 0" << std::endl;
  auto cache = M->getCache();
  auto type = cache->findClass(typeName);
  std::cout << "DEBUG getOrRealizeType 1" << std::endl;
  if (!type) {
    std::cout << "DEBUG getOrRealizeType 2" << std::endl;
    return nullptr;
  }

  try {
    std::cout << "DEBUG getOrRealizeType 3" << std::endl;
    auto gens = translateGenerics(cache, generics);
    std::cout << "DEBUG getOrRealizeType 3.5" << std::endl;
    // return cache->realizeType(type, gens);
    return realizeTypePatch(cache, type, gens);
  } catch (const codon::exc::ParserException &e) {
    std::cout << "DEBUG getOrRealizeType 4" << std::endl;
    for (auto &trace : e.getErrors())
      for (auto &msg : trace)
        LOG_IR("getOrRealizeType parser error at {}: {}", msg.getSrcInfo(),
               msg.getMessage());
    return nullptr;
  }
}

class TernaryVec : public transform::OperatorPass {
  const std::string KEY = "ternary-vec";
  std::string getKey() const override { return KEY; }

  Value *ternToCall(Value *v) {
    auto ternVal = cast<TernaryInstr>(v);
    assert(ternVal);

    std::cout << "DEBUG: Converting ternary to call 1\n";

    auto values = ternVal->getCond()->getUsedValues();
    if ( !values.size() )
      return v;

    std::cout << "DEBUG: Converting ternary to call 2\n";

    auto *M        = ternVal->getModule();
    auto *cond     = values[0];
    auto *trueVal  = ternVal->getTrueValue();
    auto *falseVal = ternVal->getFalseValue();

    std::cout << "DEBUG: Converting ternary to call 3\n";

    auto generics = cond->getType()->getGenerics();
    if ( generics.size() != 2 )
      return v;

    std::cout << "DEBUG: Converting ternary to call 4 " << generics[0].getTypeValue()->getName() << " " << generics[1].getStaticValue() << "\n";
    std::cout << "DEBUG: got Vec class " << codon::ast::getMangledClass("std.experimental.simd", "Vec") << "\n";

    auto *vecType = getOrRealizeTypePatch(
      M,
      codon::ast::getMangledClass("std.experimental.simd", "Vec"),
      generics);

    std::cout << "DEBUG: vecType: " << vecType << "\n";
    if ( !vecType || !cond->getType()->is(vecType) )
      return v;

    std::cout << "DEBUG: Converting ternary to call 5\n";

    auto *ternTrueVal  = cast<TernaryInstr>(trueVal);
    auto *ternFalseVal = cast<TernaryInstr>(falseVal);
    if ( ternTrueVal )
      trueVal = ternToCall(ternTrueVal);
    if ( ternFalseVal )
      falseVal = ternToCall(ternFalseVal);

    std::cout << "DEBUG: Converting ternary to call 6\n";

    auto *ternaryHelper = M->getOrRealizeFunc("ternary", {cond->getType(), trueVal->getType(), falseVal->getType()}, {}, "std.lib");
    assert(ternaryHelper);

    std::cout << "DEBUG: Converting ternary to call 7\n";

    auto *ternaryCall = util::call(ternaryHelper, {cond, trueVal, falseVal});
    assert(ternaryCall);

    std::cout << "DEBUG: Converting ternary to call 8\n";

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
