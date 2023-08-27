#include "func_repl.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"

namespace vectron {

using namespace codon::ir;


void FuncReplacement::transform(CallInstr *v) {    
    auto *M = v->getModule(); 
    auto *orig = util::getFunc(v->getCallee());
    if (!orig || orig->getUnmangledName() != "prep")
      return;   
    else{
      auto *args_1 = v->front();
      auto *args_2 = v->back();    
      auto *sumOne = M->getOrRealizeFunc("sumOne", {args_1->getType(), args_2->getType()});    
      auto *sumOneCall = util::call(sumOne, {args_1, args_2});       
      v->replaceAll(sumOneCall);
    }
    
    
}


void FuncReplacement::handle(CallInstr *v) { transform(v); }

} // namespace vectron


