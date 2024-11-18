#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"


namespace vectron {

using namespace codon::ir;

class LoopVec : public transform::OperatorPass {
  const std::string KEY = "loop-vec";
  std::string getKey() const override { return KEY; }

  void handle(FlowInstr *v) override {
    // Skip if inner comprehension.
    if ( bool(findLast<ForFlow>()) )
      return;

    auto *forFl = cast<ForFlow>(cast<SeriesFlow>(v->getFlow())->back());
    // If not for flow then it is not comprehension.
    if ( !forFl )
      return;
    
    auto *innerFl = cast<FlowInstr>(cast<CallInstr>(cast<SeriesFlow>(forFl->getBody())->back())->back())->getFlow();
    if ( !innerFl )
      return;

    auto *innerForFl = cast<ForFlow>(cast<SeriesFlow>(innerFl)->back());
    // If not inner for flow then it is not a double comprehension.
    if ( !innerForFl )
      return;

    auto *innerCall = cast<CallInstr>(cast<CallInstr>(cast<SeriesFlow>(innerForFl->getBody())->back())->back());
    // If there is no inner call then it cannot be a typical Vectron use-case
    if ( !innerCall )
      return;
    
    // In case a [0] type bypass is used
    auto *innerFunc = util::getFunc(innerCall->getCallee());
    if ( !bool(innerFunc) || innerFunc->getUnmangledName() != Module::GETITEM_MAGIC_NAME )
      return;
    
    auto *vectronCall = cast<CallInstr>(innerCall->front());
    if ( !vectronCall )
      return;
    
    auto *vectronFunc = util::getFunc(vectronCall->getCallee());
    if ( !bool(vectronFunc) || !util::hasAttribute(vectronFunc, "std.vectron.attributes.vectron") )
      return;

    std::vector<Value*> args = {vectronCall->getCallee()};
    std::vector<types::Type*> tps = {vectronFunc->getType()};
    for ( auto it = vectronCall->begin(); it != vectronCall->end(); ++it ) {
      auto *getitemCall = cast<CallInstr>(*it);
      assert( getitemCall && "Arguments in Vectron method in comprehension detected but not accessed. Did you forgot to use __getitem__ (i.e. x[i])?");

      auto *getitem = util::getFunc(getitemCall->getCallee());
      assert(getitem && getitem->getUnmangledName() == Module::GETITEM_MAGIC_NAME && "Arguments in Vectron method in comprehension detected but not accessed. Did you forgot to use __getitem__ (i.e. x[i])?");

      auto *arg = getitemCall->front();
      args.push_back(arg);
      tps.push_back(arg->getType());
    }

    auto *M = v->getModule();
    auto *alpernFunc = M->getOrRealizeFunc("alpern", tps, {}, "std.lib");
    assert(alpernFunc);

    auto *alpernCall = util::call(alpernFunc, args);
    assert(alpernCall);

    v->replaceAll(alpernCall);
  }
};

} // namespace sequre
