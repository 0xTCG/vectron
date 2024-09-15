#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"


namespace vectron {

using namespace codon::ir;

class TernaryVec : public transform::OperatorPass {
  const std::string KEY = "ternary-vec";
  std::string getKey() const override { return KEY; }

  void handle(TernaryInstr *v) override {
    auto values = v->getCond()->getUsedValues();
    
    if ( !values.size() )
      return;
    
    auto *M        = v->getModule();
    auto *cond     = values[0];
    auto *trueVal  = v->getTrueValue();
    auto *falseVal = v->getFalseValue();

    auto generics = cond->getType()->getGenerics();
    if ( !generics.size() )
      return;
    
    auto *vecType = M->getOrRealizeType("Vec", generics, "std.experimental.simd");
    if ( !cond->getType()->is(vecType) )
      return;

    auto *ternaryHelper = M->getOrRealizeFunc("ternary", {cond->getType(), trueVal->getType(), falseVal->getType()}, {}, "std.lib");
    assert(ternaryHelper);

    auto *ternaryCall = util::call(ternaryHelper, {cond, trueVal, falseVal});
    assert(ternaryCall);

    auto *caller = findLast<CallInstr>();
    if ( !caller ) 
      v->replaceAll(ternaryCall);
      return;
    
    // Figure out a better way to do this:
    // v->replaceAll(ternaryCall) does not invoke typecheck of the caller
    // forcing the call of newly realized function below
    // (won't work for non-method caller)
    std::vector<Value*> args;
    std::vector<types::Type*> argsTypes;
    for ( auto it = caller->begin(); it != caller->end(); ++it ) {
      auto arg = (*it)->is<TernaryInstr>() ? ternaryCall : *it;
      args.push_back(arg);
      argsTypes.push_back(arg->getType());
    }
    
    auto *newCallerHelper = M->getOrRealizeMethod(
      caller->getType(),
      util::getFunc(caller->getCallee())->getUnmangledName(),
      argsTypes
    );
    assert(newCallerHelper);

    auto *newCaller = util::call(newCallerHelper, args);
    assert(newCaller);

    caller->replaceAll(newCaller);
  }
};

} // namespace sequre
