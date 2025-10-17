#include "vectron.h"
#include "func_repl.h"
#include "loop_anlz.h"
#include "list_init.h"
#include "bypass.h"
#include "env_select.h"
#include "ternary.h"
#include "loop.h"


namespace vectron {

void Vectron::addIRPasses(codon::ir::transform::PassManager *pm, bool debug) {  
  pm->registerPass(std::make_unique<byPass>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt"); 
  pm->registerPass(std::make_unique<LoopAnalyzer>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt");
  pm->registerPass(std::make_unique<ListInitializer>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt"); 
  pm->registerPass(std::make_unique<EvnSelector>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt");    
  pm->registerPass(std::make_unique<FuncReplacement>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt");

  pm->registerPass(std::make_unique<LoopVec>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt");
  pm->registerPass(std::make_unique<TernaryVec>(), debug ? "core-pipeline-lowering" : "core-pythonic-dict-arithmetic-opt");
}

} // namespace vectron

extern "C" std::unique_ptr<codon::DSL> load() {return std::make_unique<vectron::Vectron>(); }