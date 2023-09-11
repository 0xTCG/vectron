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
    if(std::filesystem::exists("LoopInfo.txt")){
        std::remove("LoopInfo.txt");
    }
    if(std::filesystem::exists("Prep_info.txt")){
        std::remove("Prep_info.txt");
    }
    if(std::filesystem::exists("arg_1.txt")){
        std::remove("arg_1.txt");
    }
    if(std::filesystem::exists("arg_2.txt")){
        std::remove("arg_2.txt");
    }
    if(std::filesystem::exists("arg_3.txt")){
        std::remove("arg_3.txt");
    }
    if(std::filesystem::exists("mx_arg1.txt")){
        std::remove("mx_arg1.txt");
    }
    if(std::filesystem::exists("mx_arg2.txt")){
        std::remove("mx_arg2.txt");
    }
    if(std::filesystem::exists("mx_arg3.txt")){
        std::remove("mx_arg3.txt");
    }
    if(std::filesystem::exists("lst_1.txt")){
        std::remove("lst_1.txt");
    }
    if(std::filesystem::exists("lst_2.txt")){
        std::remove("lst_2.txt");
    }
    if(std::filesystem::exists("lst_3.txt")){
        std::remove("lst_3.txt");
    }            
    auto *pf = getParentFunc();
    auto pf_name = pf->getUnmangledName();
    if(pf_name != "orig")
        return;
    std::vector<codon::ir::Value *> func = v->getUsedValues();
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