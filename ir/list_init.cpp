#include "list_init.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>

namespace vectron {

using namespace codon::ir;


void ListInitializer::transform(AssignInstr *v) {          
    auto *pf = getParentFunc();
    auto pf_name = pf->getUnmangledName();
    if(pf_name != "orig")
        return;  
    auto pf_arg1 = pf->arg_front()->getName();
    auto pf_arg2 = pf->arg_back()->getName();
    auto *c = cast<CallInstr>(v->getRhs());
    if (c == NULL)
        return;    
    auto *lst = util::getFunc(c->getCallee());
    if (!lst || lst->getUnmangledName() != "listCR")
      return;   
    else{       
        std::ofstream MyFile;
        if(std::filesystem::exists("lst_1.txt")){
            if(std::filesystem::exists("lst_2.txt")){
                MyFile.open("lst_3.txt");
            }
            else{
                MyFile.open("lst_2.txt");
            }
        }
        else{
            MyFile.open("lst_1.txt");
        }        
        auto v_name = v->getLhs()->getName();  
        MyFile << v_name << "\n"; 
        std::vector<codon::ir::Value *> vls = c->getUsedValues();  // the real values passed by the function call (numbers)    
        auto *first_arg = cast<CallInstr>(vls[0]);
        if(first_arg != NULL){
            auto *first_arg_func = util::getFunc(first_arg->getCallee());
            auto first_arg_func_name = first_arg_func->getUnmangledName();
            if(first_arg_func_name == "len"){
                std::vector<codon::ir::Var *> len_arg_1 = first_arg->front()->getUsedVariables();
                auto len_arg_1_name = len_arg_1[0]->getName();
                if (len_arg_1_name == pf_arg1){
                    MyFile << "1\n";
                }
                else{
                    MyFile << "2\n";                       
                }                
                MyFile << "0\n";
            }
            else{
                if(first_arg_func_name == "__add__" || first_arg_func_name == "__sub__"){
                    auto *first_arg_add = first_arg->back(); // add/sub value of first_arg
                    auto *first_arg_var = cast<CallInstr>(first_arg->front())->front();
                    std::vector<codon::ir::Var *> len_arg_1 = first_arg_var->getUsedVariables();                    
                    auto len_arg_1_name = len_arg_1[0]->getName();
                    
                    if (len_arg_1_name == pf_arg1){
                        MyFile << "1\n";
                    }
                    else{
                        MyFile << "2\n";                       
                    }                
                    if(first_arg_func_name == "__sub__")
                        MyFile << "-";
                    MyFile << *first_arg_add << "\n";                              
                }
            }

        }
        else{
            MyFile << "0\n" << *vls[0] << "\n";
        }

        auto *second_arg = cast<CallInstr>(vls[1]);
        if(second_arg != NULL){
            auto *second_arg_func = util::getFunc(second_arg->getCallee());
            auto second_arg_func_name = second_arg_func->getUnmangledName();
            if(second_arg_func_name == "len"){
                std::vector<codon::ir::Var *> len_arg_2 = second_arg->front()->getUsedVariables();
                auto len_arg_2_name = len_arg_2[0]->getName();
                if (len_arg_2_name == pf_arg1){
                    MyFile << "1\n";
                }
                else{
                    MyFile << "2\n";                       
                }                
                MyFile << "0\n";
            }
            else{
                if(second_arg_func_name == "__add__" || second_arg_func_name == "__sub__"){
                    auto *second_arg_add = second_arg->back(); // add/sub value of first_arg
                    auto *second_arg_var = cast<CallInstr>(second_arg->front())->front();
                    std::vector<codon::ir::Var *> len_arg_2 = second_arg_var->getUsedVariables();                    
                    auto len_arg_2_name = len_arg_2[0]->getName();                    
                    if (len_arg_2_name == pf_arg1){
                        MyFile << "1\n";
                    }
                    else{
                        MyFile << "2\n";                       
                    }                
                    if(second_arg_func_name == "__sub__")
                        MyFile << "-";
                    MyFile << *second_arg_add << "\n";                              
                }
            }

        }
        else{
            MyFile << "0\n" << *vls[1] << "\n";
        }
        MyFile << *vls[2] << "\n";
        MyFile << *vls[3] << "\n";
        MyFile << *vls[4] << "\n";
        MyFile << *vls[5] << "\n";
        MyFile << *vls[6] << "\n";
        MyFile << *vls[7] << "\n";

        MyFile.close();
    }
    
}


void ListInitializer::handle(AssignInstr *v) { transform(v); }

} // namespace vectron


