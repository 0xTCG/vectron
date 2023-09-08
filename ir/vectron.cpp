#include "vectron.h"
#include "func_repl.h"
#include "loop_anlz.h"
#include "list_init.h"
#include "zdrp.h"


namespace vectron {

void Vectron::addIRPasses(codon::ir::transform::PassManager *pm, bool debug) {  
  //pm->registerPass(std::make_unique<zdrp>());
  //pm->registerPass(std::make_unique<ListInitializer>());
  //pm->registerPass(std::make_unique<LoopAnalyzer>());
  //pm->registerPass(std::make_unique<FuncReplacement>());


  //pm->registerPass(std::make_unique<ListInitializer>(), "core-parallel-openmp");
  //pm->registerPass(std::make_unique<FuncReplacement>(), "core-parallel-openmp");  
  //pm->registerPass(std::make_unique<LoopAnalyzer>(), "core-parallel-openmp");
  //pm->registerPass(std::make_unique<zdrp>(), "core-parallel-openmp");  
  //pm->registerPass(std::make_unique<LoopAnalyzer>(),"core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<FuncReplacement>(),"core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<FuncReplacement>(),"core-pythonic-str-addition-opt");  
  //pm->registerPass(std::make_unique<LoopAnalyzer>(),"core-pythonic-str-addition-opt");  
  //pm->registerPass(std::make_unique<ListInitializer>(), "core-folding-pass-group");
  //pm->registerPass(std::make_unique<FuncReplacement>(),"core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<LoopAnalyzer>(),"core-folding-pass-group"); 
  //pm->registerPass(std::make_unique<FuncReplacement>(), "core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<ListInitializer>(), "core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<LoopAnalyzer>(), "core-pythonic-dict-arithmetic-opt");  
  //pm->registerPass(std::make_unique<zdrp>(), "core-pythonic-dict-arithmetic-opt");      

  //pm->registerPass(std::make_unique<zdrp>(), "core-folding-pass-group", {} ,{"core-folding-pass-group"}); 
  //pm->registerPass(std::make_unique<LoopAnalyzer>(), "core-folding-pass-group", {} ,{"core-folding-pass-group"}); 
  //pm->registerPass(std::make_unique<ListInitializer>(), "core-folding-pass-group", {} ,{"core-folding-pass-group"}); 
  //pm->registerPass(std::make_unique<FuncReplacement>(), "core-folding-pass-group", {} ,{"core-folding-pass-group"}); 

  pm->registerPass(std::make_unique<zdrp>(), "core-folding-pass-group"); 
  pm->registerPass(std::make_unique<LoopAnalyzer>(), "core-folding-pass-group");
  pm->registerPass(std::make_unique<ListInitializer>(), "core-folding-pass-group"); 
  pm->registerPass(std::make_unique<FuncReplacement>(), "core-folding-pass-group");    
  
  

}

} // namespace vectron

extern "C" std::unique_ptr<codon::DSL> load() {return std::make_unique<vectron::Vectron>(); }