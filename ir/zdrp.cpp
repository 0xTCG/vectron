#include "zdrp.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>

namespace vectron {

using namespace codon::ir;


void zdrp::transform(ReturnInstr *v) {
         
    auto *pf = getParentFunc();
    auto pf_name = pf->getUnmangledName();
    if(pf_name != "orig")
        return;
    std::__1::vector<codon::ir::Value *> func = v->getUsedValues();
    auto *func_log = cast<CallInstr>(func[0]);
    auto func_name = util::getFunc(func_log->getCallee())->getUnmangledName();    
    if(func_name != "zdrop")
        return;
    auto *z_value = func_log->back();
    std::ofstream MyFile("zdrop.txt");
    MyFile << *z_value << "\n";
    
    
}
void zdrp::handle(ReturnInstr *v) { transform(v); }

} // namespace vectron