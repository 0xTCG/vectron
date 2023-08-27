#include "loop_anlz.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include "codon/cir/cir.h"
#include "codon/cir/flow.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/analyze/dataflow/capture.h"
#include "codon/cir/analyze/dataflow/reaching.h"
#include "codon/cir/util/side_effect.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <utility>


namespace vectron {

using namespace codon::ir;

void LoopAnalyzer::transform(ImperativeForFlow *v) {
    int arg1_flag = 0;
    int arg2_flag = 0;
    int arg3_flag = 0;
    int flag = 0;
    auto *pf = getParentFunc();;
    auto pf_name = pf->getUnmangledName();    
    if (pf_name == "prep"){   
        auto pf_arg1 = pf->arg_front()->getName();
        auto pf_arg2 = pf->arg_back()->getName();
        auto *check = v->getBody();
        auto *temp = cast<SeriesFlow>(check)->back();
        auto *inner_loop = cast<ImperativeForFlow>(temp); // second for loop
        if (inner_loop == NULL)
            return;
        auto *w_outer = v->getActual();
        std::vector<codon::ir::Var *> vars_outer = w_outer->getUsedVariables();
        auto var_str_outer = vars_outer[0]->getName(); // outer loop's iterator's name

        auto *w_inner = inner_loop->getActual();
        std::vector<codon::ir::Var *> vars_inner = w_inner->getUsedVariables();
        auto var_str_inner = vars_inner[0]->getName(); // inner loop's iterator's name                
        auto *ass = cast<CallInstr>(cast<SeriesFlow>(inner_loop->getBody())->back()); // assignment op inside the inner loop    
        auto *right_side = cast<CallInstr>(ass->back()); // the right side of the assignment operation
        auto *oper = util::getFunc(right_side->getCallee());
        auto op = oper->getUnmangledName(); // the name of the assignment function (add, sub, min or max)

        auto *s_outer = v->getStart();
        auto *e_outer = v->getEnd();
        auto *call_end_outer = cast<CallInstr>(e_outer);
        auto *call_start_outer = cast<CallInstr>(s_outer);

        auto *s_inner = inner_loop->getStart();
        auto *e_inner = inner_loop->getEnd();
        auto *call_end_inner = cast<CallInstr>(e_inner);
        auto *call_start_inner = cast<CallInstr>(s_inner);
        std::ofstream MyFile("Prep_info.txt");
        if (call_start_outer){ // writing the starting condition of the loop
            auto *start_func = util::getFunc(call_start_outer->getCallee());
            auto final_start = start_func->getUnmangledName();
            MyFile << final_start << "\n"; // work in this to recognize functions
        }
        else{   
            MyFile << *s_outer << "\n";     
        }
        MyFile << v->getStep() << "\n";     //writing the step condition of the loop
        if (call_end_outer){ // writing the end condition of the loop
            auto *end_func = util::getFunc(call_end_outer->getCallee());
            auto final_end = end_func->getUnmangledName();            
            if (final_end == "len"){
                std::__1::vector<codon::ir::Var *> len_arg = call_end_outer->back()->getUsedVariables();
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                    MyFile << "0\n";
                }
                else{
                    MyFile << "-2\n";
                    MyFile << "0\n";                        
                }
            }
            else{
                auto *right_end = call_end_outer->back();                
                std::__1::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_outer->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                }
                else{
                    MyFile << "-2\n";                      
                }
                if (final_end == "__sub__"){
                    MyFile << "-";
                }
                MyFile << *right_end << "\n";
            }
        }
        else{
            MyFile << *e_outer << "\n";
            MyFile << "0\n";   
        }
        if (call_start_inner){ // writing the starting condition of the loop
            auto *start_func_inner = util::getFunc(call_start_inner->getCallee());
            auto final_start_inner = start_func_inner->getUnmangledName();
            MyFile << final_start_inner << "\n"; // work in this to recognize functions
        }
        else{       
            MyFile << *s_inner << "\n";     
        }
        MyFile << inner_loop->getStep() << "\n";     //writing the step condition of the loop
        if (call_end_inner){ // writing the end condition of the loop
            auto *end_func_inner = util::getFunc(call_end_inner->getCallee());
            auto final_end_inner = end_func_inner->getUnmangledName();
            if (final_end_inner == "len"){                
                //auto *len_arg_fr = cast<CallInstr>(cast<CallInstr>(call_end_inner)->front())->front();
                //std::vector<codon::ir::Var *> end_inner_var = len_arg_fr->getUsedVariables();
                //auto len_arg = end_inner_var[0]->getName();
                //auto len_arg = end_func_inner->arg_front()->getName();
                std::__1::vector<codon::ir::Var *> len_arg_inner = call_end_inner->back()->getUsedVariables();
                auto len_arg_name_inner = len_arg_inner[0]->getName();        
                if (len_arg_name_inner == pf_arg1){
                    MyFile << "-1\n";
                    MyFile << "0\n";
                }
                else{
                    MyFile << "-2\n";
                    MyFile << "0\n";                        
                }
            }
            else{
                auto *right_end_inner = call_end_inner->back();  
                std::__1::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_inner->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                }
                else{
                    MyFile << "-2\n";                      
                }                              
                if (final_end_inner == "__sub__"){
                    MyFile << "-";
                }
                MyFile << *right_end_inner << "\n";
            }
        }
        else{
            MyFile << *e_inner << "\n";
            MyFile << "0\n";   
        }        
        MyFile.close();
    }    
    else if (pf_name == "orig"){         
        auto pf_arg1 = pf->arg_front()->getName();
        auto pf_arg2 = pf->arg_back()->getName();
        auto *check = v->getBody();
        auto *temp = cast<SeriesFlow>(check)->back();
        auto *inner_loop = cast<ImperativeForFlow>(temp); // second for loop
        if (inner_loop == NULL)
            return;
        auto *w_outer = v->getActual();
        std::vector<codon::ir::Var *> vars_outer = w_outer->getUsedVariables();
        auto var_str_outer = vars_outer[0]->getName(); // outer loop's iterator's name
        auto *w_inner = inner_loop->getActual();
        std::vector<codon::ir::Var *> vars_inner = w_inner->getUsedVariables();
        auto var_str_inner = vars_inner[0]->getName(); // inner loop's iterator's name  
        auto *chk = cast<IfFlow>(cast<SeriesFlow>(inner_loop->getBody())->back());
        if(chk == NULL){
            std::ofstream bw_manager("bw.txt");            
            bw_manager << "0\n0\n0\n0\n0\n0\n0\n0\n0\n";         
            flag = 1;
            bw_manager.close();

        }     
        else{
            std::ofstream bw_manager("bw.txt");                     
            auto *main_true_branch = cast<IfFlow>(cast<SeriesFlow>(inner_loop->getBody())->back())->getCond();
            std::__1::vector<codon::ir::Value *> main_if_ops = main_true_branch->getUsedValues();
            std::ofstream bw_temp_write("temp_bw.txt");
            bw_temp_write << *main_if_ops[1];
            bw_temp_write.close();
            std::ifstream bw_temp_read("temp_bw.txt");
            std::basic_string or_check = "";
            bw_temp_read >> or_check;
            bw_temp_read.close();
            bw_temp_write.open("temp_bw.txt");
            bw_temp_write << *main_if_ops[2];
            bw_temp_write.close();
            bw_temp_read.open("temp_bw.txt");
            std::basic_string and_check = "";
            bw_temp_read >> and_check;
            bw_temp_read.close();  
            std::remove("temp_bw.txt");
            if(or_check == "true"){
                bw_manager << "2\n";
            }
            else{
                if(and_check == "false"){
                    bw_manager << "1\n";
                }
                else{
                    bw_manager << "0\n";
                }
            }
            if (and_check == "false" || or_check == "true"){
                auto *left_cond = cast<CallInstr>(main_if_ops[0]);
                std::__1::vector<codon::ir::Value *> left_cond_vars = left_cond->getUsedValues();

                auto *left_operator = util::getFunc(left_cond_vars[2]);
                auto left_op_name = left_operator->getUnmangledName();
                if (left_op_name == "__eq__"){
                    bw_manager << "0\n";
                }
                else{
                    if (left_op_name == "__gt__"){
                        bw_manager << "1\n";
                    }
                    else{
                        if(left_op_name == "__ge__"){
                            bw_manager << "2\n";
                        }
                        else{
                            if(left_op_name == "__lt__"){
                                bw_manager << "-1\n";
                            }
                            else{
                                bw_manager << "-2\n";
                            }
                        }
                    }
                }            
                auto *left_lhs = cast<CallInstr>(left_cond_vars[0]); // the last op of the lhs of first condition
                if(left_lhs == NULL){
                    std::__1::vector<codon::ir::Var *> var_lhs = left_cond_vars[0]->getUsedVariables();
                    auto lhs_name = var_lhs[0]->getName();
                    if (lhs_name == var_str_outer){
                        bw_manager << "1\n";
                    }
                    else{
                        if(lhs_name == var_str_inner){
                            bw_manager << "2\n";
                        }
                        else{
                            bw_manager << "0\n";
                        }
                    }
                    bw_manager << "0\n";

                }
                else{
                    auto last_lhs_op = util::getFunc(left_lhs->getCallee())->getUnmangledName(); 
                    auto *left_lhs_lhs = left_lhs->front();
                    auto *left_lhs_lhs_func = cast<CallInstr>(left_lhs_lhs); // the second to last op of the lhs of first condition
                    if(left_lhs_lhs_func == NULL){
                        std::__1::vector<codon::ir::Var *> var_lhs_lhs = left_lhs_lhs->getUsedVariables();
                        auto lhs_lhs_name = var_lhs_lhs[0]->getName();
                        if (lhs_lhs_name == var_str_outer){
                            bw_manager << "1\n";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }
                        auto *left_lhs_rhs = left_lhs->back();
                        std::__1::vector<codon::ir::Var *> var_lhs_rhs = left_lhs_rhs->getUsedVariables();
                        auto lhs_rhs_name = var_lhs_rhs[0]->getName();
                        if (lhs_rhs_name == var_str_outer){
                            if (last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }
                            bw_manager << "1\n";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                if (last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(left_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *first_lhs_operand = left_lhs_lhs_func->back();
                        std::__1::vector<codon::ir::Var *> first_var = first_lhs_operand->getUsedVariables();
                        auto first_var_name = first_var[0]->getName();
                        if (first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_manager << "-";
                            }                            
                            bw_manager << "1\n";
                        }
                        else{
                            if(first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_manager << "-";
                                }                                             
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }
                                    
                                    
                        auto *left_lhs_rhs = left_lhs->back();                  
                        std::__1::vector<codon::ir::Var *> second_var = left_lhs_rhs->getUsedVariables();                
                        auto second_var_name = second_var[0]->getName();    
                        if (second_var_name == var_str_outer){
                            if(last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }                        
                            bw_manager << "1\n";
                        }
                        else{
                            if(second_var_name == var_str_inner){    
                                if(last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }  
                    }                                       

                }
                bw_manager << *left_cond_vars[1] << "\n";
                
                auto *right_cond = cast<CallInstr>(main_if_ops[2]);
                if (and_check == "false"){
                    right_cond = cast<CallInstr>(main_if_ops[1]); 
                }

                std::__1::vector<codon::ir::Value *> right_cond_vars = right_cond->getUsedValues();    
                int build_flag = 0;   
                auto *right_operator = util::getFunc(cast<CallInstr>(right_cond_vars[0])->getCallee());
                auto right_op_name = right_operator->getUnmangledName();
                if (right_op_name == "__eq__"){
                    bw_manager << "0\n";
                }
                else{
                    if (right_op_name == "__gt__"){
                        bw_manager << "1\n";
                    }
                    else{
                        if(right_op_name == "__ge__"){
                            bw_manager << "2\n";
                        }
                        else{
                            if(right_op_name == "__lt__"){
                                bw_manager << "-1\n";
                            }
                            else{
                                if(right_op_name == "__le__"){
                                    bw_manager << "-2\n";
                                }  
                                else{
                                    build_flag = 1;
                                    right_operator =  util::getFunc(cast<CallInstr>(right_cond)->getCallee());
                                    right_op_name = right_operator->getUnmangledName();
                                    if (right_op_name == "__eq__"){
                                            bw_manager << "0\n";
                                        }
                                    else{
                                        if (right_op_name == "__gt__"){
                                            bw_manager << "1\n";
                                        }
                                        else{
                                            if(right_op_name == "__ge__"){
                                                bw_manager << "2\n";
                                            }
                                            else{
                                                if(right_op_name == "__lt__"){
                                                    bw_manager << "-1\n";
                                                }
                                                else{
                                                    if(right_op_name == "__le__"){
                                                        bw_manager << "-2\n";
                                                    }      
                                                }
                                            }
                                        }
                                    }                               

                                }                                

                            }
                        }
                    }
                }    




                auto *right = cast<CallInstr>(right_cond_vars[0])->front(); // the left hand side of the right condition
                auto *right_lhs = cast<CallInstr>(right);
                if(build_flag == 1){
                    right_lhs = cast<CallInstr>(right_cond->front());
                }
                if(right_lhs == NULL){
                    std::__1::vector<codon::ir::Var *> r_var_lhs = right->getUsedVariables();
                    auto r_lhs_name = r_var_lhs[0]->getName();
                    if (r_lhs_name == var_str_outer){
                        bw_manager << "1\n";
                    }
                    else{
                        if(r_lhs_name == var_str_inner){
                            bw_manager << "2\n";
                        }
                        else{
                            bw_manager << "0\n";
                        }
                    }
                    bw_manager << "0\n";

                }
                else{
                    auto r_last_lhs_op = util::getFunc(right_lhs->getCallee())->getUnmangledName(); 
                    auto *right_lhs_lhs = right_lhs->front();
                    auto *right_lhs_lhs_func = cast<CallInstr>(right_lhs_lhs); // the second to last op of the lhs of first condition
                    if(right_lhs_lhs_func == NULL){
                        std::__1::vector<codon::ir::Var *> r_var_lhs_lhs = right_lhs_lhs->getUsedVariables();
                        auto r_lhs_lhs_name = r_var_lhs_lhs[0]->getName();
                        if (r_lhs_lhs_name == var_str_outer){
                            bw_manager << "1\n";
                        }
                        else{
                            if(r_lhs_lhs_name == var_str_inner){
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }
                        auto *right_lhs_rhs = right_lhs->back();
                        std::__1::vector<codon::ir::Var *> r_var_lhs_rhs = right_lhs_rhs->getUsedVariables();
                        auto r_lhs_rhs_name = r_var_lhs_rhs[0]->getName();
                        if (r_lhs_rhs_name == var_str_outer){
                            if (r_last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }
                            bw_manager << "1\n";
                        }
                        else{
                            if(r_lhs_lhs_name == var_str_inner){
                                if (r_last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(right_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *r_first_lhs_operand = right_lhs_lhs_func->back();
                        std::__1::vector<codon::ir::Var *> r_first_var = r_first_lhs_operand->getUsedVariables();
                        auto r_first_var_name = r_first_var[0]->getName();
                        if (r_first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_manager << "-";
                            }                            
                            bw_manager << "1\n";
                        }
                        else{
                            if(r_first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_manager << "-";
                                }                                             
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }      
                                    
                        auto *right_lhs_rhs = right_lhs->back();                  
                        std::__1::vector<codon::ir::Var *> r_second_var = right_lhs_rhs->getUsedVariables();                
                        auto r_second_var_name = r_second_var[0]->getName();    
                        if (r_second_var_name == var_str_outer){
                            if(r_last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }                        
                            bw_manager << "1\n";
                        }
                        else{
                            if(r_second_var_name == var_str_inner){    
                                if(r_last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }  
                    }                                       

                }
                auto *right_rhs = cast<CallInstr>(right_cond_vars[0])->back();
                if(build_flag == 1){
                    right_rhs = right_cond->back();
                }
                bw_manager << *right_rhs << "\n";      
            }              
            else{             
                auto *single_left_cond = cast<CallInstr>(main_true_branch);
                //std::__1::vector<codon::ir::Value *> left_cond_vars = left_cond->getUsedValues();
                auto *left_operator = util::getFunc(single_left_cond->getCallee());
                auto left_op_name = left_operator->getUnmangledName();
                if (left_op_name == "__eq__"){
                    bw_manager << "0\n";
                }
                else{
                    if (left_op_name == "__gt__"){
                        bw_manager << "1\n";
                    }
                    else{
                        if(left_op_name == "__ge__"){
                            bw_manager << "2\n";
                        }
                        else{
                            if(left_op_name == "__lt__"){
                                bw_manager << "-1\n";
                            }
                            else{
                                bw_manager << "-2\n";
                            }
                        }
                    }
                }            

                auto *left_lhs = cast<CallInstr>(main_true_branch); // the last op of the lhs of first condition
                auto *op_check = cast<CallInstr>(left_lhs->front());
                std::__1::vector<codon::ir::Value *> left_cond_vars = left_lhs->getUsedValues();             
                if(op_check == NULL){
                    std::__1::vector<codon::ir::Var *> var_lhs = left_cond_vars[0]->getUsedVariables();
                    auto lhs_name = var_lhs[0]->getName();
                    if (lhs_name == var_str_outer){
                        bw_manager << "1\n";
                    }
                    else{
                        if(lhs_name == var_str_inner){
                            bw_manager << "2\n";
                        }
                        else{
                            bw_manager << "0\n";
                        }
                    }
                    bw_manager << "0\n";

                }
                else{
                    auto last_lhs_op = util::getFunc(op_check->getCallee())->getUnmangledName(); 
                    auto *left_lhs_lhs = op_check->front();
                    auto *left_lhs_lhs_func = cast<CallInstr>(left_lhs_lhs); // the second to last op of the lhs of first condition
                    if(left_lhs_lhs_func == NULL){
                        std::__1::vector<codon::ir::Var *> var_lhs_lhs = left_lhs_lhs->getUsedVariables();
                        auto lhs_lhs_name = var_lhs_lhs[0]->getName();
                        if (lhs_lhs_name == var_str_outer){
                            bw_manager << "1\n";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }
                        auto *left_lhs_rhs = op_check->back();
                        std::__1::vector<codon::ir::Var *> var_lhs_rhs = left_lhs_rhs->getUsedVariables();
                        auto lhs_rhs_name = var_lhs_rhs[0]->getName();
                        if (lhs_rhs_name == var_str_outer){
                            if (last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }
                            bw_manager << "1\n";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                if (last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(left_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *first_lhs_operand = left_lhs_lhs_func->back();
                        std::__1::vector<codon::ir::Var *> first_var = first_lhs_operand->getUsedVariables();
                        auto first_var_name = first_var[0]->getName();
                        if (first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_manager << "-";
                            }                            
                            bw_manager << "1\n";
                        }
                        else{
                            if(first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_manager << "-";
                                }                                             
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }
                                    
                                    
                        auto *left_lhs_rhs = left_lhs->back();                  
                        std::__1::vector<codon::ir::Var *> second_var = left_lhs_rhs->getUsedVariables();                
                        auto second_var_name = second_var[0]->getName();    
                        if (second_var_name == var_str_outer){
                            if(last_lhs_op == "__sub__"){
                                bw_manager << "-";
                            }                        
                            bw_manager << "1\n";
                        }
                        else{
                            if(second_var_name == var_str_inner){    
                                if(last_lhs_op == "__sub__"){
                                    bw_manager << "-";
                                }                                            
                                bw_manager << "2\n";
                            }
                            else{
                                bw_manager << "0\n";
                            }                        
                        }  
                    }                                       

                }
                bw_manager << *main_if_ops[1] << "\n";                
                bw_manager << "0\n0\n0\n0\n";
            }
            
            bw_manager.close();
        }    
        auto *ass = cast<CallInstr>(cast<SeriesFlow>(inner_loop->getBody())->back()); // assignment op inside the inner loop   
        if (flag == 0){
            auto *main_else_branch = cast<IfFlow>(cast<SeriesFlow>(inner_loop->getBody())->back())->getFalseBranch();                        
            ass = cast<CallInstr>(cast<SeriesFlow>(main_else_branch)->back()); // assignment op inside the inner loop                
        }
        auto *right_side = cast<CallInstr>(ass->back()); // the right side of the assignment operation
        std::__1::vector<codon::ir::Var *> left_side = cast<CallInstr>(ass->front())->front()->getUsedVariables();
        auto left_side_name = left_side[0]->getName();

        auto *oper = util::getFunc(right_side->getCallee());
        auto op = oper->getUnmangledName(); // the name of the assignment function (add, sub, min or max)

        auto *s_outer = v->getStart();
        auto *e_outer = v->getEnd();
        auto *call_end_outer = cast<CallInstr>(e_outer);
        auto *call_start_outer = cast<CallInstr>(s_outer);

        auto *s_inner = inner_loop->getStart();
        auto *e_inner = inner_loop->getEnd();
        auto *call_end_inner = cast<CallInstr>(e_inner);
        auto *call_start_inner = cast<CallInstr>(s_inner);
        std::ofstream MyFile("LoopInfo.txt");
        if (call_start_outer){ // writing the starting condition of the loop
            auto *start_func = util::getFunc(call_start_outer->getCallee());
            auto final_start = start_func->getUnmangledName();
            MyFile << final_start << "\n"; // work in this to recognize functions
        }
        else{   
            MyFile << *s_outer << "\n";     
        }
        MyFile << v->getStep() << "\n";     //writing the step condition of the loop
        if (call_end_outer){ // writing the end condition of the loop
            auto *end_func = util::getFunc(call_end_outer->getCallee());
            auto final_end = end_func->getUnmangledName();            
            if (final_end == "len"){
                std::__1::vector<codon::ir::Var *> len_arg = call_end_outer->back()->getUsedVariables();
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                    MyFile << "0\n";
                }
                else{
                    MyFile << "-2\n";
                    MyFile << "0\n";                        
                }
            }
            else{
                auto *right_end = call_end_outer->back(); 
                std::__1::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_outer->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                }
                else{
                    MyFile << "-2\n";                      
                }
                if (final_end == "__sub__"){
                    MyFile << "-";
                }
                MyFile << *right_end << "\n";
            }
        }
        else{
            MyFile << *e_outer << "\n";
            MyFile << "0\n";   
        }
        if (call_start_inner){ // writing the starting condition of the loop
            auto *start_func_inner = util::getFunc(call_start_inner->getCallee());
            auto final_start_inner = start_func_inner->getUnmangledName();
            MyFile << final_start_inner << "\n"; // work in this to recognize functions
        }
        else{       
            MyFile << *s_inner << "\n";     
        }
        MyFile << inner_loop->getStep() << "\n";     //writing the step condition of the loop
        if (call_end_inner){ // writing the end condition of the loop
            auto *end_func_inner = util::getFunc(call_end_inner->getCallee());
            auto final_end_inner = end_func_inner->getUnmangledName();
            if (final_end_inner == "len"){                
                //auto *len_arg_fr = cast<CallInstr>(cast<CallInstr>(call_end_inner)->front())->front();
                //std::vector<codon::ir::Var *> end_inner_var = len_arg_fr->getUsedVariables();
                //auto len_arg = end_inner_var[0]->getName();
                //auto len_arg = end_func_inner->arg_front()->getName();
                std::__1::vector<codon::ir::Var *> len_arg_inner = call_end_inner->back()->getUsedVariables();
                auto len_arg_name_inner = len_arg_inner[0]->getName();        
                if (len_arg_name_inner == pf_arg1){
                    MyFile << "-1\n";
                    MyFile << "0\n";
                }
                else{
                    MyFile << "-2\n";
                    MyFile << "0\n";                        
                }
            }
            else{
                auto *right_end_inner = call_end_inner->back();                
                std::__1::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_inner->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    MyFile << "-1\n";
                }
                else{
                    MyFile << "-2\n";                      
                }                          
                if (final_end_inner == "__sub__"){
                    MyFile << "-";
                }
                MyFile << *right_end_inner << "\n";
            }
        }
        else{
            MyFile << *e_inner << "\n";
            MyFile << "0\n";   
        }        
        if (op == "min"){
            MyFile << "0\n";
        }
        else{ 
            MyFile << "1\n";
        }
        if (op == "max" || op == "min"){
            auto *arg1 = cast<CallInstr>(right_side->front()); // the left side operand
            auto *arg1_inst = cast<CallInstr>(arg1->front()); 
                            
            std::__wrap_iter<codon::ir::Value **> r_mid = cast<CallInstr>(right_side->front())->begin();
            auto *arg2_inst = cast<CallInstr>(r_mid[1]);        
                
            auto *r_end = cast<CallInstr>(right_side->front())->back(); // the last argument of the main function (min or max)
            int main_args = 0;
            if (r_end != r_mid[1]){
                main_args = 1;
            }                                         

            if (arg1_inst == NULL){
                MyFile << "0\n";
            }
            else{
                auto arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();
                if (arg1_func_name == "__add__" || arg1_func_name == "__sub__" || arg1_func_name == "__mul__" || arg1_func_name == "__div__" ){
                    MyFile << "1\n";
                }
                else{
                    if(arg1_func_name == "max_store"){
                        MyFile << "0\n";    
                    }
                    else{
                        MyFile << "1\n";
                    }
                }
            }
            
            if (arg2_inst == NULL){
                MyFile << "0\n";
            }
            else{
                auto arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();
                if (arg2_func_name == "__add__" || arg2_func_name == "__sub__" || arg2_func_name == "__mul__" || arg2_func_name == "__div__"){
                    MyFile << "1\n";
                }
                else{
                    if(arg2_func_name == "max_store"){
                        MyFile << "0\n";    
                    }
                    else{
                        MyFile << "1\n";
                    }
                }                    
            }
            if (main_args == 0)
                MyFile << "-1\n";  // reserved for min max with 3 arguments
            else{
                auto *arg3 = cast<CallInstr>(right_side->back()); // the right side operand                         
                auto *arg3_inst = cast<CallInstr>(arg3->back());        
                if (arg3_inst == NULL){
                    MyFile << "0\n";
                }
                else{
                    auto arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();   
                    if (arg3_func_name == "__add__" || arg3_func_name == "__sub__" || arg3_func_name == "__mul__" || arg3_func_name == "__div__"){
                        MyFile << "1\n";
                    }
                    else{
                        if(arg3_func_name == "max_store"){
                            MyFile << "0\n";    
                        }
                        else{
                            MyFile << "1\n";
                        }
                    }                               
                }
            }
            if (arg1_inst == NULL){
                MyFile << "0\n0\n0\n0\n0\n" << *arg1->front() << "\n"; // writing down the constant value + reserved elements for further instructions                
                arg1_flag = 1;
            }
            else{
                auto arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();
                if(arg1_func_name == "max_store"){
                    MyFile << "0\n0\n0\n0\n0\n0\n";
                    std::ofstream mx_file("mx_arg1.txt");
                    std::vector<codon::ir::Value *> mx_arg_1 = arg1_inst->getUsedValues();
                    std::vector<codon::ir::Var *> mx_arg_1_1 = mx_arg_1[0]->getUsedVariables();
                    auto mx_arg_1_1_name = mx_arg_1_1[0]->getName();
                    mx_file << mx_arg_1_1_name << "\n";
                    auto *mx_arg_1_2 = cast<CallInstr>(mx_arg_1[1]);
                    if(mx_arg_1_2 == NULL){
                        mx_file << "0\n";
                    }
                    else{
                        auto mx_arg_1_2_func = util::getFunc(mx_arg_1_2->getCallee())->getUnmangledName();
                        if(mx_arg_1_2_func == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_1_2_add = mx_arg_1_2->back();
                        mx_file << *mx_arg_1_2_add << "\n";
                    }
                    auto *mx_arg_1_3 = cast<CallInstr>(mx_arg_1[2]);
                    if(mx_arg_1_3 == NULL){
                        mx_file << "0\n";
                    }
                    else{
                        auto mx_arg_1_3_func = util::getFunc(mx_arg_1_3->getCallee())->getUnmangledName();
                        if(mx_arg_1_3_func == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_1_3_add = mx_arg_1_3->back();
                        mx_file << *mx_arg_1_3_add << "\n";
                    }                    
                    auto *mx_arg_1_4 = cast<CallInstr>(mx_arg_1[3]);       
                    auto mx_arg_1_4_name = util::getFunc(cast<CallInstr>(mx_arg_1_4)->getCallee())->getUnmangledName();
                    if(mx_arg_1_4_name == "__add__" || mx_arg_1_4_name == "__sub__"){
                        auto *mx_arg_1_4_left = mx_arg_1_4->front();
                        auto *mx_arg_1_4_left_part_1 = cast<CallInstr>(mx_arg_1_4_left)->front();
                        auto *mx_arg_1_4_left_part_1_1 = cast<CallInstr>(mx_arg_1_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_1_4_left_part_1_2 = cast<CallInstr>(mx_arg_1_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_1_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_4_left_part_1_2)->getCallee());
                            auto mx_arg_1_4_inst = mx_arg_1_4_left_inst->getUnmangledName();
                            if(mx_arg_1_4_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_4_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_1_4_part_2 = cast<CallInstr>(mx_arg_1_4_left)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_1_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_4_part_2)->getCallee());
                            auto mx_arg_1_4_part_2_inst = mx_arg_1_4_left_part_2->getUnmangledName();;
                            if(mx_arg_1_4_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_1_4_part_2)->back();
                            mx_file << *part_2_add << "\n";
                        }
                        if(mx_arg_1_4_name == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_1_4_right = mx_arg_1_4->back(); // final value               
                        mx_file << *mx_arg_1_4_right << "\n";
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_1_4_left_part_1 = cast<CallInstr>(mx_arg_1_4)->front();
                        auto *mx_arg_1_4_left_part_1_1 = cast<CallInstr>(mx_arg_1_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_1_4_left_part_1_2 = cast<CallInstr>(mx_arg_1_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_1_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_4_left_part_1_2)->getCallee());
                            auto mx_arg_1_4_inst = mx_arg_1_4_left_inst->getUnmangledName();
                            if(mx_arg_1_4_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_4_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_1_4_part_2 = cast<CallInstr>(mx_arg_1_4)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_1_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_4_part_2)->getCallee());
                            auto mx_arg_1_4_part_2_inst = mx_arg_1_4_left_part_2->getUnmangledName();;
                            if(mx_arg_1_4_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = mx_arg_1_4->back();
                            mx_file << *part_2_add << "\n";
                        }  
                        mx_file << "0\n";                 
                    }

                    auto *mx_arg_1_5 = cast<CallInstr>(mx_arg_1[4]);       
                    auto mx_arg_1_5_name = util::getFunc(cast<CallInstr>(mx_arg_1_5)->getCallee())->getUnmangledName();
                    if(mx_arg_1_5_name == "__add__" || mx_arg_1_5_name == "__sub__"){
                        auto *mx_arg_1_5_left = mx_arg_1_5->front();
                        auto *mx_arg_1_5_left_part_1 = cast<CallInstr>(mx_arg_1_5_left)->front();
                        auto *mx_arg_1_5_left_part_1_1 = cast<CallInstr>(mx_arg_1_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_1_5_left_part_1_2 = cast<CallInstr>(mx_arg_1_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_1_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_5_left_part_1_2)->getCallee());
                            auto mx_arg_1_5_inst = mx_arg_1_5_left_inst->getUnmangledName();
                            if(mx_arg_1_5_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_5_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_1_5_part_2 = cast<CallInstr>(mx_arg_1_5_left)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_1_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_5_part_2)->getCallee());
                            auto mx_arg_1_5_part_2_inst = mx_arg_1_5_left_part_2->getUnmangledName();;
                            if(mx_arg_1_5_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_1_5_part_2)->back();
                            mx_file << *part_2_add << "\n";
                        }
                        if(mx_arg_1_5_name == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_1_5_right = mx_arg_1_5->back(); // final value               
                        mx_file << *mx_arg_1_5_right << "\n";
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_1_5_left_part_1 = cast<CallInstr>(mx_arg_1_5)->front();
                        auto *mx_arg_1_5_left_part_1_1 = cast<CallInstr>(mx_arg_1_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_1_5_left_part_1_2 = cast<CallInstr>(mx_arg_1_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_1_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_5_left_part_1_2)->getCallee());
                            auto mx_arg_1_5_inst = mx_arg_1_5_left_inst->getUnmangledName();
                            if(mx_arg_1_5_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_5_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_1_5_part_2 = cast<CallInstr>(mx_arg_1_5)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_1_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_5_part_2)->getCallee());
                            auto mx_arg_1_5_part_2_inst = mx_arg_1_5_left_part_2->getUnmangledName();;
                            if(mx_arg_1_5_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = mx_arg_1_5->back();
                            mx_file << *part_2_add << "\n";
                        }  
                        mx_file << "0\n";                 
                    }                                        
                    mx_file.close();                    
                }
                else{
                    auto arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();
                    if (arg1_func_name == "__add__" || arg1_func_name == "__sub__" || arg1_func_name == "__mul__" || arg1_func_name == "__div__"){
                        auto *arg1_op = cast<CallInstr>(arg1_inst->front()); // the left side operand
                        auto *arg1_fr = cast<CallInstr>(arg1_op->front()); // the left side variable + its row index                 
                        auto *arg1_var = arg1_fr->front(); // the left side variable  
                        auto *arg1_row = arg1_fr->back(); // the left side row index 
                        auto *arg1_col = arg1_op->back(); // the left side column index          
                        auto *arg1_const = arg1_inst->back(); // the constant  
                        std::vector<codon::ir::Var *> arg1_var_name = arg1_var->getUsedVariables();                        
                        auto *arg1_row_instr = cast<CallInstr>(arg1_row); // is arg1 row index a math op?
                        auto *arg1_col_instr = cast<CallInstr>(arg1_col); // is arg1 row index a math op?             
                        auto arg1_fr_name = arg1_var_name[0]->getName(); // left side variable's name   
                        if (arg1_func_name == "__mul__"){
                            MyFile << *arg1_const << "\n";
                        }
                        else if (arg1_func_name == "__div__"){
                            MyFile << "1/" << arg1_const << "\n";
                        }
                        else{
                            MyFile << "1\n";
                        }
                        std::string arg1_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg1_row_instr != NULL){
                            auto *arg1_row_func = util::getFunc(arg1_row_instr->getCallee());
                            auto arg1_row_func_name = arg1_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row_instr->front()->getUsedVariables();
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg1_row_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg1_num_row = arg1_row_instr->back();
                            MyFile << *arg1_num_row << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row->getUsedVariables();            
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        std::string arg1_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg1_col_instr != NULL){
                            auto *arg1_col_func = util::getFunc(arg1_col_instr->getCallee());
                            auto arg1_col_func_name = arg1_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col_instr->front()->getUsedVariables();
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg1_col_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg1_num_col = arg1_col_instr->back();
                            MyFile << *arg1_num_col << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col->getUsedVariables();            
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        if (arg1_func_name == "__sub__"){
                            MyFile << "-";
                            MyFile << *arg1_const << "\n";
                        }
                        else if (arg1_func_name == "__add__"){
                            if (cast<CallInstr>(arg1_const) == NULL){                            
                                MyFile << *arg1_const << "\n"; // writing down the constant value + reserved elements for further instructions                            
                            }
                            else{
                                MyFile << "0\n";
                                auto *m_call_1 = cast<CallInstr>(arg1_const);
                                auto *m_call_1_func = util::getFunc(m_call_1->getCallee());
                                auto m_call_1_name = m_call_1_func->getUnmangledName();                        
                                if (m_call_1_name == "match_func"){
                                    std::__1::vector<codon::ir::Value *> body_1 = m_call_1_func->getUsedValues();
                                    auto match_arg_1 = m_call_1_func->arg_front()->getName();
                                    auto match_arg_2 = m_call_1_func->arg_back()->getName();                                

                                    auto *true_return_1 = cast<IfFlow>(cast<SeriesFlow>(body_1[0])->back())->getTrueBranch();

                                    auto *elif_branch_1_series = cast<IfFlow>(cast<SeriesFlow>(body_1[0])->back())->getFalseBranch();
                                    auto *elif_branch_1 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_1_series)->back())->getCond();
                                    auto *elif_return_1 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_1_series)->back())->getTrueBranch();

                                    auto *else_branch_1 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_1_series)->back())->getFalseBranch();

                                    auto *elif_cond_1_call = cast<CallInstr>(elif_branch_1);
                                    auto *elif_cond_1 = util::getFunc(elif_cond_1_call->getCallee());
                                    auto elif_cond_func_name = elif_cond_1->getUnmangledName();
                                    std::__1::vector<codon::ir::Var *> elif_arg_1 = elif_cond_1_call->front()->getUsedVariables();
                                    std::__1::vector<codon::ir::Var *> elif_arg_2 = elif_cond_1_call->back()->getUsedVariables();
                                    auto elif_arg_1_name = elif_arg_1[0]->getName();
                                    auto elif_arg_2_name = elif_arg_2[0]->getName();
                                    std::ofstream arg_1_file("arg_1.txt");
                                    arg_1_file << "0\n";                                    
                                    std::__1::vector<codon::ir::Value *> all_args_1 = m_call_1->getUsedValues();
                                    auto *a_arg = all_args_1[1];                                
                                    auto *b_arg = all_args_1[2];
                                    auto *am_arg = all_args_1[3];        
                                    if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                        auto *return_val_1_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_1)->back())->getValue();
                                        auto *return_val_1_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_1)->back())->getValue();
                                        auto *return_val_1_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_1)->back())->getValue(); 
                                        std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_1_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_1_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_1_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }                                    
                                    
                                    }                                
                                    else{
                                        auto *return_val_1_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_1)->back())->getValue();
                                        auto *return_val_1_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_1)->back())->getValue();
                                        auto *return_val_1_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_1)->back())->getValue();  
                                        std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_1_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_1_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_1_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            arg_1_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                arg_1_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    arg_1_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }                                     
                                    }  
                                    auto *arg_1_1_index = cast<CallInstr>(m_call_1->front())->back();
                                    if(cast<CallInstr>(arg_1_1_index) != NULL){
                                        auto *arg_1_1_f = cast<CallInstr>(arg_1_1_index);
                                        auto *arg_1_1_index_func = util::getFunc(arg_1_1_f->getCallee());
                                        auto arg_1_1_index_func_name = arg_1_1_index_func->getUnmangledName();                                    
                                        if(arg_1_1_index_func_name == "__sub__"){
                                            arg_1_file << "-";
                                        }
                                        auto *tst_1 = arg_1_1_f->back(); 
                                        arg_1_file << *tst_1 << "\n";
                                    }
                                    else{
                                        arg_1_file << "0\n";
                                    }
                                    auto *arg_1_2_index = cast<CallInstr>(m_call_1->back())->back();
                                    if(cast<CallInstr>(arg_1_2_index) != NULL){
                                        auto *arg_1_2_f = cast<CallInstr>(arg_1_2_index);
                                        auto *arg_1_2_index_func = util::getFunc(arg_1_2_f->getCallee());
                                        auto arg_1_2_index_func_name = arg_1_2_index_func->getUnmangledName();                                    
                                        if(arg_1_2_index_func_name == "__sub__"){
                                            arg_1_file << "-";
                                        }
                                        auto *tst_1 = arg_1_2_f->back(); 
                                        arg_1_file << *tst_1 << "\n";
                                    }
                                    else{
                                        arg_1_file << "0\n";
                                    }                                
                                    arg_1_file.close();                                
                                }
                                
                            }
                        }
                        else{
                            MyFile << "0\n";
                        }                    
                    }
                    else{
                        auto *arg1_fr = cast<CallInstr>(arg1_inst->front()); // the left side operand + its row index
                        auto *arg1_var = arg1_fr->front(); // the left side variable
                        auto *arg1_row = arg1_fr->back(); // the left side row index 
                        auto *arg1_col = arg1_inst->back(); // the left side column index
                        std::vector<codon::ir::Var *> arg1_var_name = arg1_var->getUsedVariables();
                        auto *arg1_row_instr = cast<CallInstr>(arg1_row); // is arg1 row index a math op?
                        auto *arg1_col_instr = cast<CallInstr>(arg1_col); // is arg1 row index a math op? 
                        auto arg1_fr_name = arg1_var_name[0]->getName(); // left side variable's name                           
                        //if (arg1_fr_name == pf_arg1){
                        MyFile << "1\n";
                        //}
                        //if (arg1_fr_name == pf_arg2){
                        //    MyFile << "1\n";
                        //}                
                        std::string arg1_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg1_row_instr != NULL){
                            auto *arg1_row_func = util::getFunc(arg1_row_instr->getCallee());
                            auto arg1_row_func_name = arg1_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row_instr->front()->getUsedVariables();
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg1_row_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg1_num_row = arg1_row_instr->back();
                            MyFile << *arg1_num_row << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row->getUsedVariables();            
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        std::string arg1_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg1_col_instr != NULL){
                            auto *arg1_col_func = util::getFunc(arg1_col_instr->getCallee());
                            auto arg1_col_func_name = arg1_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col_instr->front()->getUsedVariables();
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg1_col_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg1_num_col = arg1_col_instr->back();
                            MyFile << *arg1_num_col << "\n0\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col->getUsedVariables();            
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n0\n";
                        }
                    }
                }
            }
            if (arg2_inst == NULL){
                MyFile << "0\n0\n0\n0\n0\n"<< *r_mid[1] << "\n"; // writing down the constant value + reserved elements for further instructions
                arg2_flag = 1;
            }
            else{
                auto arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();
                if(arg2_func_name == "max_store"){
                    MyFile << "0\n0\n0\n0\n0\n0\n";
                    std::ofstream mx_file("mx_arg2.txt");
                    std::vector<codon::ir::Value *> mx_arg_2 = arg2_inst->getUsedValues();
                    std::vector<codon::ir::Var *> mx_arg_2_1 = mx_arg_2[0]->getUsedVariables();
                    auto mx_arg_2_1_name = mx_arg_2_1[0]->getName();
                    mx_file << mx_arg_2_1_name << "\n";
                    auto *mx_arg_2_2 = cast<CallInstr>(mx_arg_2[1]);
                    if(mx_arg_2_2 == NULL){
                        mx_file << "0\n";
                    }
                    else{
                        auto mx_arg_2_2_func = util::getFunc(mx_arg_2_2->getCallee())->getUnmangledName();
                        if(mx_arg_2_2_func == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_2_2_add = mx_arg_2_2->back();
                        mx_file << *mx_arg_2_2_add << "\n";
                    }
                    auto *mx_arg_2_3 = cast<CallInstr>(mx_arg_2[2]);
                    if(mx_arg_2_3 == NULL){
                        mx_file << "0\n";
                    }
                    else{
                        auto mx_arg_2_3_func = util::getFunc(mx_arg_2_3->getCallee())->getUnmangledName();
                        if(mx_arg_2_3_func == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_2_3_add = mx_arg_2_3->back();
                        mx_file << *mx_arg_2_3_add << "\n";
                    }                    
                    auto *mx_arg_2_4 = cast<CallInstr>(mx_arg_2[3]);       
                    auto mx_arg_2_4_name = util::getFunc(cast<CallInstr>(mx_arg_2_4)->getCallee())->getUnmangledName();
                    if(mx_arg_2_4_name == "__add__" || mx_arg_2_4_name == "__sub__"){
                        auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_4_left_part_1 = cast<CallInstr>(mx_arg_2_4_left)->front();
                        auto *mx_arg_2_4_left_part_1_1 = cast<CallInstr>(mx_arg_2_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_2_4_left_part_1_2 = cast<CallInstr>(mx_arg_2_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_2_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_4_left_part_1_2)->getCallee());
                            auto mx_arg_2_4_inst = mx_arg_2_4_left_inst->getUnmangledName();
                            if(mx_arg_2_4_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_4_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_2_4_part_2 = cast<CallInstr>(mx_arg_2_4_left)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_2_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_4_part_2)->getCallee());
                            auto mx_arg_2_4_part_2_inst = mx_arg_2_4_left_part_2->getUnmangledName();;
                            if(mx_arg_2_4_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_2_4_part_2)->back();
                            mx_file << *part_2_add << "\n";
                        }
                        if(mx_arg_2_4_name == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_2_4_right = mx_arg_2_4->back(); // final value               
                        mx_file << *mx_arg_2_4_right << "\n";
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_4_left_part_1 = cast<CallInstr>(mx_arg_2_4)->front();
                        auto *mx_arg_2_4_left_part_1_1 = cast<CallInstr>(mx_arg_2_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_2_4_left_part_1_2 = cast<CallInstr>(mx_arg_2_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_2_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_4_left_part_1_2)->getCallee());
                            auto mx_arg_2_4_inst = mx_arg_2_4_left_inst->getUnmangledName();
                            if(mx_arg_2_4_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_4_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_2_4_part_2 = cast<CallInstr>(mx_arg_2_4)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_2_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_4_part_2)->getCallee());
                            auto mx_arg_2_4_part_2_inst = mx_arg_2_4_left_part_2->getUnmangledName();;
                            if(mx_arg_2_4_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = mx_arg_2_4->back();
                            mx_file << *part_2_add << "\n";
                        }  
                        mx_file << "0\n";                 
                    }

                    auto *mx_arg_2_5 = cast<CallInstr>(mx_arg_2[4]);       
                    auto mx_arg_2_5_name = util::getFunc(cast<CallInstr>(mx_arg_2_5)->getCallee())->getUnmangledName();
                    if(mx_arg_2_5_name == "__add__" || mx_arg_2_5_name == "__sub__"){
                        auto *mx_arg_2_5_left = mx_arg_2_5->front();
                        auto *mx_arg_2_5_left_part_1 = cast<CallInstr>(mx_arg_2_5_left)->front();
                        auto *mx_arg_2_5_left_part_1_1 = cast<CallInstr>(mx_arg_2_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_2_5_left_part_1_2 = cast<CallInstr>(mx_arg_2_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_2_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_5_left_part_1_2)->getCallee());
                            auto mx_arg_2_5_inst = mx_arg_2_5_left_inst->getUnmangledName();
                            if(mx_arg_2_5_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_5_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_2_5_part_2 = cast<CallInstr>(mx_arg_2_5_left)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_2_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_5_part_2)->getCallee());
                            auto mx_arg_2_5_part_2_inst = mx_arg_2_5_left_part_2->getUnmangledName();;
                            if(mx_arg_2_5_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_2_5_part_2)->back();
                            mx_file << *part_2_add << "\n";
                        }
                        if(mx_arg_2_5_name == "__sub__"){
                            mx_file << "-";
                        }
                        auto *mx_arg_2_5_right = mx_arg_2_5->back(); // final value               
                        mx_file << *mx_arg_2_5_right << "\n";
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_5_left_part_1 = cast<CallInstr>(mx_arg_2_5)->front();
                        auto *mx_arg_2_5_left_part_1_1 = cast<CallInstr>(mx_arg_2_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx_file << part_1_lst_name << "\n";
                        auto *mx_arg_2_5_left_part_1_2 = cast<CallInstr>(mx_arg_2_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_left_part_1_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_2_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_5_left_part_1_2)->getCallee());
                            auto mx_arg_2_5_inst = mx_arg_2_5_left_inst->getUnmangledName();
                            if(mx_arg_2_5_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_5_left_part_1_2)->back();
                            mx_file << *part_1_add << "\n";
                        }
                        auto *mx_arg_2_5_part_2 = cast<CallInstr>(mx_arg_2_5)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_part_2) == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto *mx_arg_2_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_5_part_2)->getCallee());
                            auto mx_arg_2_5_part_2_inst = mx_arg_2_5_left_part_2->getUnmangledName();;
                            if(mx_arg_2_5_part_2_inst == "__sub__"){
                                mx_file << "-";
                            }
                            auto *part_2_add = mx_arg_2_5->back();
                            mx_file << *part_2_add << "\n";
                        }  
                        mx_file << "0\n";                 
                    }                    
                        

                    mx_file.close();
                }
                else{                
                    auto arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();
                    if (arg2_func_name == "__add__" || arg2_func_name == "__sub__" || arg2_func_name == "__mul__" || arg2_func_name == "__div__"){
                        auto *arg2_op = cast<CallInstr>(arg2_inst->front()); // the left side operand
                        auto *arg2_fr = cast<CallInstr>(arg2_op->front()); // the left side variable + its row index                 
                        auto *arg2_var = arg2_fr->front(); // the left side variable  
                        auto *arg2_row = arg2_fr->back(); // the left side row index 
                        auto *arg2_col = arg2_op->back(); // the left side column index          
                        auto *arg2_const = arg2_inst->back(); // the constant                              
                        std::vector<codon::ir::Var *> arg2_var_name = arg2_var->getUsedVariables();
                        auto *arg2_row_instr = cast<CallInstr>(arg2_row); // is arg2 row index a math op?
                        auto *arg2_col_instr = cast<CallInstr>(arg2_col); // is arg2 row index a math op?                 
                        auto arg2_fr_name = arg2_var_name[0]->getName(); // left side variable's name                             
                        if (arg2_func_name == "__mul__"){
                            MyFile << *arg2_const << "\n";
                        }
                        else if (arg2_func_name == "__div__"){
                            MyFile << "1/" << *arg2_const << "\n";
                        }
                        else{
                            MyFile << "1\n";
                        }               
                        std::string arg2_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg2_row_instr != NULL){
                            auto *arg2_row_func = util::getFunc(arg2_row_instr->getCallee());
                            auto arg2_row_func_name = arg2_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row_instr->front()->getUsedVariables();
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg2_row_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg2_num_row = arg2_row_instr->back();
                            MyFile << *arg2_num_row << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row->getUsedVariables();            
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        std::string arg2_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg2_col_instr != NULL){
                            auto *arg2_col_func = util::getFunc(arg2_col_instr->getCallee());
                            auto arg2_col_func_name = arg2_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col_instr->front()->getUsedVariables();
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg2_col_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg2_num_col = arg2_col_instr->back();
                            MyFile << *arg2_num_col << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col->getUsedVariables();            
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        if (arg2_func_name == "__sub__"){
                            MyFile << "-";
                            MyFile << *arg2_const << "\n";
                        }
                        else if (arg2_func_name == "__add__"){
                            if (cast<CallInstr>(arg2_const) == NULL){
                                MyFile << *arg2_const << "\n"; // writing down the constant value + reserved elements for further instructions                            
                            }
                            else{
                                MyFile << "0\n";
                                auto *m_call_2 = cast<CallInstr>(arg2_const);
                                auto *m_call_2_func = util::getFunc(m_call_2->getCallee());
                                auto m_call_2_name = m_call_2_func->getUnmangledName();                            
                                if (m_call_2_name == "match_func"){
                                    std::__1::vector<codon::ir::Value *> body_2 = m_call_2_func->getUsedValues();
                                    auto match_arg_1 = m_call_2_func->arg_front()->getName();
                                    auto match_arg_2 = m_call_2_func->arg_back()->getName();
                                    
                                    //auto *true_branch_2 = cast<IfFlow>(cast<SeriesFlow>(body_2[0])->back())->getCond();
                                    auto *true_return_2 = cast<IfFlow>(cast<SeriesFlow>(body_2[0])->back())->getTrueBranch();

                                    auto *elif_branch_2_series = cast<IfFlow>(cast<SeriesFlow>(body_2[0])->back())->getFalseBranch();
                                    auto *elif_branch_2 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_2_series)->back())->getCond();
                                    auto *elif_return_2 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_2_series)->back())->getTrueBranch();

                                    auto *else_branch_2 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_2_series)->back())->getFalseBranch();

                                    auto *elif_cond_2_call = cast<CallInstr>(elif_branch_2);
                                    auto *elif_cond_2 = util::getFunc(elif_cond_2_call->getCallee());
                                    auto elif_cond_func_name = elif_cond_2->getUnmangledName();
                                    std::__1::vector<codon::ir::Var *> elif_arg_1 = elif_cond_2_call->front()->getUsedVariables();
                                    std::__1::vector<codon::ir::Var *> elif_arg_2 = elif_cond_2_call->back()->getUsedVariables();
                                    auto elif_arg_1_name = elif_arg_1[0]->getName();
                                    auto elif_arg_2_name = elif_arg_2[0]->getName();
                                    std::ofstream arg_2_file("arg_2.txt");
                                    arg_2_file << "0\n";   
                                    std::__1::vector<codon::ir::Value *> all_args_2 = m_call_2->getUsedValues();
                                    auto *a_arg = all_args_2[1];                                
                                    auto *b_arg = all_args_2[2];
                                    auto *am_arg = all_args_2[3];                                                                    
                                    if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                        auto *return_val_2_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_2)->back())->getValue();
                                        auto *return_val_2_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_2)->back())->getValue();
                                        auto *return_val_2_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_2)->back())->getValue();
                                        std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_2_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_2_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_2_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }                                     
                                    }                                
                                    else{
                                        auto *return_val_2_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_2)->back())->getValue();
                                        auto *return_val_2_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_2)->back())->getValue();
                                        auto *return_val_2_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_2)->back())->getValue();                                    
                                        std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_2_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_2_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_2_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            arg_2_file << *a_arg << "\n";                                        
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                arg_2_file << *b_arg << "\n";                                            
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    arg_2_file << *am_arg << "\n";                                                
                                                }
                                            }
                                        }                                  
                                    }       
                                    auto *arg_2_1_index = cast<CallInstr>(m_call_2->front())->back();
                                    if(cast<CallInstr>(arg_2_1_index) != NULL){
                                        auto *arg_2_1_f = cast<CallInstr>(arg_2_1_index);
                                        auto *arg_2_1_index_func = util::getFunc(arg_2_1_f->getCallee());
                                        auto arg_2_1_index_func_name = arg_2_1_index_func->getUnmangledName();                                    
                                        if(arg_2_1_index_func_name == "__sub__"){
                                            arg_2_file << "-";
                                        }
                                        auto *tst_1 = arg_2_1_f->back(); 
                                        arg_2_file << *tst_1 << "\n";
                                    }
                                    else{
                                        arg_2_file << "0\n";
                                    }
                                    auto *arg_2_2_index = cast<CallInstr>(m_call_2->back())->back();
                                    if(cast<CallInstr>(arg_2_2_index) != NULL){
                                        auto *arg_2_2_f = cast<CallInstr>(arg_2_2_index);
                                        auto *arg_2_2_index_func = util::getFunc(arg_2_2_f->getCallee());
                                        auto arg_2_2_index_func_name = arg_2_2_index_func->getUnmangledName();                                    
                                        if(arg_2_2_index_func_name == "__sub__"){
                                            arg_2_file << "-";
                                        }
                                        auto *tst_1 = arg_2_2_f->back(); 
                                        arg_2_file << *tst_1 << "\n";
                                    }
                                    else{
                                        arg_2_file << "0\n";
                                    }                                                 
                                    arg_2_file.close();

                                }
                                
                            }
                        }
                        else{
                            MyFile << "0\n";
                        }
                    }
                    else{
                        auto *arg2_fr = cast<CallInstr>(arg2_inst->front()); // the left side operand + its row index
                        auto *arg2_var = arg2_fr->front(); // the left side variable
                        auto *arg2_row = arg2_fr->back(); // the left side row index 
                        auto *arg2_col = arg2_inst->back(); // the left side column index
                        std::vector<codon::ir::Var *> arg2_var_name = arg2_var->getUsedVariables();
                        auto *arg2_row_instr = cast<CallInstr>(arg2_row); // is arg2 row index a math op?
                        auto *arg2_col_instr = cast<CallInstr>(arg2_col); // is arg2 row index a math op? 
                        auto arg2_fr_name = arg2_var_name[0]->getName(); // left side variable's name                           
                        //if (arg2_fr_name == pf_arg1){
                        MyFile << "1\n";
                        //}
                        //if (arg2_fr_name == pf_arg2){
                        //    MyFile << "1\n";
                        //}                
                        std::string arg2_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg2_row_instr != NULL){
                            auto *arg2_row_func = util::getFunc(arg2_row_instr->getCallee());
                            auto arg2_row_func_name = arg2_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row_instr->front()->getUsedVariables();
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg2_row_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg2_num_row = arg2_row_instr->back();
                            MyFile << *arg2_num_row << "\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row->getUsedVariables();            
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n";
                        }
                        std::string arg2_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg2_col_instr != NULL){
                            auto *arg2_col_func = util::getFunc(arg2_col_instr->getCallee());
                            auto arg2_col_func_name = arg2_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col_instr->front()->getUsedVariables();
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            if (arg2_col_func_name == "__sub__"){
                                MyFile << "-";
                            }            
                            auto *arg2_num_col = arg2_col_instr->back();
                            MyFile << *arg2_num_col << "\n0\n";            
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col->getUsedVariables();            
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                MyFile << "0\n";
                            }
                            else{
                                MyFile << "1\n";
                            }
                            MyFile << "0\n0\n";
                        }
                    }
                }
            }
            if (main_args != 0){
                auto *arg3 = cast<CallInstr>(right_side->back()); // the left side operand                
                auto *arg3_inst = cast<CallInstr>(arg3->back());
                if (arg3_inst == NULL){
                    MyFile << "0\n0\n0\n0\n0\n" << *r_end << "\n"; // writing down the constant value + reserved elements for further instructions
                    arg3_flag = 1;
                }
                else{
                    auto arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();
                    if(arg3_func_name == "max_store"){
                        MyFile << "0\n0\n0\n0\n0\n0\n";
                        std::ofstream mx_file("mx_arg3.txt");
                        std::vector<codon::ir::Value *> mx_arg_3 = arg3_inst->getUsedValues();
                        std::vector<codon::ir::Var *> mx_arg_3_1 = mx_arg_3[0]->getUsedVariables();
                        auto mx_arg_3_1_name = mx_arg_3_1[0]->getName();
                        mx_file << mx_arg_3_1_name << "\n";
                        auto *mx_arg_3_2 = cast<CallInstr>(mx_arg_3[1]);
                        if(mx_arg_3_2 == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_3_2_func = util::getFunc(mx_arg_3_2->getCallee())->getUnmangledName();
                            if(mx_arg_3_2_func == "__sub__"){
                                mx_file << "-";
                            }
                            auto *mx_arg_3_2_add = mx_arg_3_2->back();
                            mx_file << *mx_arg_3_2_add << "\n";
                        }
                        auto *mx_arg_3_3 = cast<CallInstr>(mx_arg_3[2]);
                        if(mx_arg_3_3 == NULL){
                            mx_file << "0\n";
                        }
                        else{
                            auto mx_arg_3_3_func = util::getFunc(mx_arg_3_3->getCallee())->getUnmangledName();
                            if(mx_arg_3_3_func == "__sub__"){
                                mx_file << "-";
                            }
                            auto *mx_arg_3_3_add = mx_arg_3_3->back();
                            mx_file << *mx_arg_3_3_add << "\n";
                        }                    
                        auto *mx_arg_3_4 = cast<CallInstr>(mx_arg_3[3]);       
                        auto mx_arg_3_4_name = util::getFunc(cast<CallInstr>(mx_arg_3_4)->getCallee())->getUnmangledName();
                        if(mx_arg_3_4_name == "__add__" || mx_arg_3_4_name == "__sub__"){
                            auto *mx_arg_3_4_left = mx_arg_3_4->front();
                            auto *mx_arg_3_4_left_part_1 = cast<CallInstr>(mx_arg_3_4_left)->front();
                            auto *mx_arg_3_4_left_part_1_1 = cast<CallInstr>(mx_arg_3_4_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_4_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx_file << part_1_lst_name << "\n";
                            auto *mx_arg_3_4_left_part_1_2 = cast<CallInstr>(mx_arg_3_4_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_left_part_1_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto mx_arg_3_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_4_left_part_1_2)->getCallee());
                                auto mx_arg_3_4_inst = mx_arg_3_4_left_inst->getUnmangledName();
                                if(mx_arg_3_4_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_4_left_part_1_2)->back();
                                mx_file << *part_1_add << "\n";
                            }
                            auto *mx_arg_3_4_part_2 = cast<CallInstr>(mx_arg_3_4_left)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_part_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto *mx_arg_3_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_4_part_2)->getCallee());
                                auto mx_arg_3_4_part_2_inst = mx_arg_3_4_left_part_2->getUnmangledName();;
                                if(mx_arg_3_4_part_2_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_2_add = cast<CallInstr>(mx_arg_3_4_part_2)->back();
                                mx_file << *part_2_add << "\n";
                            }
                            if(mx_arg_3_4_name == "__sub__"){
                                mx_file << "-";
                            }
                            auto *mx_arg_3_4_right = mx_arg_3_4->back(); // final value               
                            mx_file << *mx_arg_3_4_right << "\n";
                        }
                        else{
                            //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                            auto *mx_arg_3_4_left_part_1 = cast<CallInstr>(mx_arg_3_4)->front();
                            auto *mx_arg_3_4_left_part_1_1 = cast<CallInstr>(mx_arg_3_4_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_4_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx_file << part_1_lst_name << "\n";
                            auto *mx_arg_3_4_left_part_1_2 = cast<CallInstr>(mx_arg_3_4_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_left_part_1_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto mx_arg_3_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_4_left_part_1_2)->getCallee());
                                auto mx_arg_3_4_inst = mx_arg_3_4_left_inst->getUnmangledName();
                                if(mx_arg_3_4_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_4_left_part_1_2)->back();
                                mx_file << *part_1_add << "\n";
                            }
                            auto *mx_arg_3_4_part_2 = cast<CallInstr>(mx_arg_3_4)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_part_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto *mx_arg_3_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_4_part_2)->getCallee());
                                auto mx_arg_3_4_part_2_inst = mx_arg_3_4_left_part_2->getUnmangledName();;
                                if(mx_arg_3_4_part_2_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_2_add = mx_arg_3_4->back();
                                mx_file << *part_2_add << "\n";
                            }  
                            mx_file << "0\n";                 
                        }

                        auto *mx_arg_3_5 = cast<CallInstr>(mx_arg_3[4]);       
                        auto mx_arg_3_5_name = util::getFunc(cast<CallInstr>(mx_arg_3_5)->getCallee())->getUnmangledName();
                        if(mx_arg_3_5_name == "__add__" || mx_arg_3_5_name == "__sub__"){
                            auto *mx_arg_3_5_left = mx_arg_3_5->front();
                            auto *mx_arg_3_5_left_part_1 = cast<CallInstr>(mx_arg_3_5_left)->front();
                            auto *mx_arg_3_5_left_part_1_1 = cast<CallInstr>(mx_arg_3_5_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_5_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx_file << part_1_lst_name << "\n";
                            auto *mx_arg_3_5_left_part_1_2 = cast<CallInstr>(mx_arg_3_5_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_left_part_1_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto mx_arg_3_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_5_left_part_1_2)->getCallee());
                                auto mx_arg_3_5_inst = mx_arg_3_5_left_inst->getUnmangledName();
                                if(mx_arg_3_5_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_5_left_part_1_2)->back();
                                mx_file << *part_1_add << "\n";
                            }
                            auto *mx_arg_3_5_part_2 = cast<CallInstr>(mx_arg_3_5_left)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_part_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto *mx_arg_3_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_5_part_2)->getCallee());
                                auto mx_arg_3_5_part_2_inst = mx_arg_3_5_left_part_2->getUnmangledName();;
                                if(mx_arg_3_5_part_2_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_2_add = cast<CallInstr>(mx_arg_3_5_part_2)->back();
                                mx_file << *part_2_add << "\n";
                            }
                            if(mx_arg_3_5_name == "__sub__"){
                                mx_file << "-";
                            }
                            auto *mx_arg_3_5_right = mx_arg_3_5->back(); // final value               
                            mx_file << *mx_arg_3_5_right << "\n";
                        }
                        else{
                            //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                            auto *mx_arg_3_5_left_part_1 = cast<CallInstr>(mx_arg_3_5)->front();
                            auto *mx_arg_3_5_left_part_1_1 = cast<CallInstr>(mx_arg_3_5_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_5_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx_file << part_1_lst_name << "\n";
                            auto *mx_arg_3_5_left_part_1_2 = cast<CallInstr>(mx_arg_3_5_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_left_part_1_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto mx_arg_3_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_5_left_part_1_2)->getCallee());
                                auto mx_arg_3_5_inst = mx_arg_3_5_left_inst->getUnmangledName();
                                if(mx_arg_3_5_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_5_left_part_1_2)->back();
                                mx_file << *part_1_add << "\n";
                            }
                            auto *mx_arg_3_5_part_2 = cast<CallInstr>(mx_arg_3_5)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_part_2) == NULL){
                                mx_file << "0\n";
                            }
                            else{
                                auto *mx_arg_3_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_5_part_2)->getCallee());
                                auto mx_arg_3_5_part_2_inst = mx_arg_3_5_left_part_2->getUnmangledName();;
                                if(mx_arg_3_5_part_2_inst == "__sub__"){
                                    mx_file << "-";
                                }
                                auto *part_2_add = mx_arg_3_5->back();
                                mx_file << *part_2_add << "\n";
                            }  
                            mx_file << "0\n";                 
                        }                                        
                        mx_file.close();                         
                    }
                    else{                    
                        auto arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();
                        if (arg3_func_name == "__add__" || arg3_func_name == "__sub__" || arg3_func_name == "__mul__" || arg3_func_name == "__div__"){
                            auto *arg3_op = cast<CallInstr>(arg3_inst->front()); // the left side operand
                            auto *arg3_fr = cast<CallInstr>(arg3_op->front()); // the left side variable + its row index                
                            auto *arg3_var = arg3_fr->front(); // the left side variable
                            auto *arg3_row = arg3_fr->back(); // the left side row index    
                            auto *arg3_col = arg3_op->back(); // the left side column index  
                            auto *arg3_const = arg3_inst->back(); // the constant                    
                            std::vector<codon::ir::Var *> arg3_var_name = arg3_var->getUsedVariables();
                            auto *arg3_row_instr = cast<CallInstr>(arg3_row); // is arg3 row index a math op?
                            auto *arg3_col_instr = cast<CallInstr>(arg3_col); // is arg3 row index a math op? 
                            auto arg3_fr_name = arg3_var_name[0]->getName(); // left side variable's name                             
                            if (arg3_func_name == "__mul__"){
                                MyFile << *arg3_const << "\n";
                            }
                            else if (arg3_func_name == "__div__"){
                                MyFile << "1/" << *arg3_const << "\n";
                            }
                            else{
                                MyFile << "1\n";
                            }               
                            std::string arg3_row_var_name = ""; // taking care of the row index of the first (left side) operand
                            if (arg3_row_instr != NULL){
                                auto *arg3_row_func = util::getFunc(arg3_row_instr->getCallee());
                                auto arg3_row_func_name = arg3_row_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row_instr->front()->getUsedVariables();
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                if (arg3_row_func_name == "__sub__"){
                                    MyFile << "-";
                                }            
                                auto *arg3_num_row = arg3_row_instr->back();
                                MyFile << *arg3_num_row << "\n";            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row->getUsedVariables();            
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                MyFile << "0\n";
                            }
                            std::string arg3_col_var_name = ""; // taking care of the column index of the first (left side) operand
                            if (arg3_col_instr != NULL){
                                auto *arg3_col_func = util::getFunc(arg3_col_instr->getCallee());
                                auto arg3_col_func_name = arg3_col_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col_instr->front()->getUsedVariables();
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                if (arg3_col_func_name == "__sub__"){
                                    MyFile << "-";
                                }            
                                auto *arg3_num_col = arg3_col_instr->back();
                                MyFile << *arg3_num_col << "\n";            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col->getUsedVariables();            
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                MyFile << "0\n";
                            }
                            if (arg3_func_name == "__sub__"){
                                MyFile << "-";
                                MyFile << *arg3_const << "\n";
                            }
                            else if (arg3_func_name == "__add__"){
                                if (cast<CallInstr>(arg3_const) == NULL){
                                    MyFile << *arg3_const << "\n"; // writing down the constant value + reserved elements for further instructions                            
                                }
                                else{
                                    MyFile << "0\n";
                                    auto *m_call_3 = cast<CallInstr>(arg3_const);
                                    auto *m_call_3_func = util::getFunc(m_call_3->getCallee());
                                    auto m_call_3_name = m_call_3_func->getUnmangledName();                            
                                    if (m_call_3_name == "match_func"){
                                        std::__1::vector<codon::ir::Value *> body_3 = m_call_3_func->getUsedValues();
                                        auto match_arg_1 = m_call_3_func->arg_front()->getName();
                                        auto match_arg_2 = m_call_3_func->arg_back()->getName();
                                        
                                        //auto *true_branch_3 = cast<IfFlow>(cast<SeriesFlow>(body_3[0])->back())->getCond();
                                        auto *true_return_3 = cast<IfFlow>(cast<SeriesFlow>(body_3[0])->back())->getTrueBranch();

                                        auto *elif_branch_3_series = cast<IfFlow>(cast<SeriesFlow>(body_3[0])->back())->getFalseBranch();
                                        auto *elif_branch_3 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_3_series)->back())->getCond();
                                        auto *elif_return_3 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_3_series)->back())->getTrueBranch();

                                        auto *else_branch_3 = cast<IfFlow>(cast<SeriesFlow>(elif_branch_3_series)->back())->getFalseBranch();

                                        auto *elif_cond_3_call = cast<CallInstr>(elif_branch_3);
                                        auto *elif_cond_3 = util::getFunc(elif_cond_3_call->getCallee());
                                        auto elif_cond_func_name = elif_cond_3->getUnmangledName();
                                        std::__1::vector<codon::ir::Var *> elif_arg_1 = elif_cond_3_call->front()->getUsedVariables();
                                        std::__1::vector<codon::ir::Var *> elif_arg_2 = elif_cond_3_call->back()->getUsedVariables();
                                        auto elif_arg_1_name = elif_arg_1[0]->getName();
                                        auto elif_arg_2_name = elif_arg_2[0]->getName();
                                        std::ofstream arg_3_file("arg_3.txt");
                                        arg_3_file << "0\n";                                    
                                        std::__1::vector<codon::ir::Value *> all_args_3 = m_call_3->getUsedValues();
                                        auto *a_arg = all_args_3[1];                                
                                        auto *b_arg = all_args_3[2];
                                        auto *am_arg = all_args_3[3];     
                                        if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                            auto *return_val_3_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_3)->back())->getValue();
                                            auto *return_val_3_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_3)->back())->getValue();
                                            auto *return_val_3_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_3)->back())->getValue();
                                            std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_3_1->getUsedVariables();
                                            auto ret_val_1_name = ret_val_1[0]->getName();
                                            std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_3_2->getUsedVariables();
                                            auto ret_val_2_name = ret_val_2[0]->getName();
                                            std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_3_3->getUsedVariables();
                                            auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                            if(ret_val_1_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_1_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_1_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }
                                            if(ret_val_2_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_2_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_2_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }
                                            if(ret_val_3_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_3_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_3_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }                                   
                                        }                                
                                        else{
                                            auto *return_val_3_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_3)->back())->getValue();
                                            auto *return_val_3_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_3)->back())->getValue();
                                            auto *return_val_3_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_3)->back())->getValue();                                    
                                            std::__1::vector<codon::ir::Var *> ret_val_1 = return_val_3_1->getUsedVariables();
                                            auto ret_val_1_name = ret_val_1[0]->getName();
                                            std::__1::vector<codon::ir::Var *> ret_val_2 = return_val_3_2->getUsedVariables();
                                            auto ret_val_2_name = ret_val_2[0]->getName();
                                            std::__1::vector<codon::ir::Var *> ret_val_3 = return_val_3_3->getUsedVariables();
                                            auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                            if(ret_val_1_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_1_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_1_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }
                                            if(ret_val_2_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_2_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_2_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }
                                            if(ret_val_3_name == "a"){
                                                arg_3_file << *a_arg << "\n";                                        
                                            }
                                            else{
                                                if(ret_val_3_name == "b"){
                                                    arg_3_file << *b_arg << "\n";                                            
                                                }
                                                else{
                                                    if(ret_val_3_name == "am"){
                                                        arg_3_file << *am_arg << "\n";                                                
                                                    }
                                                }
                                            }                                    
                                        }      
                                        auto *arg_3_1_index = cast<CallInstr>(m_call_3->front())->back();
                                        if(cast<CallInstr>(arg_3_1_index) != NULL){
                                            auto *arg_3_1_f = cast<CallInstr>(arg_3_1_index);
                                            auto *arg_3_1_index_func = util::getFunc(arg_3_1_f->getCallee());
                                            auto arg_3_1_index_func_name = arg_3_1_index_func->getUnmangledName();                                    
                                            if(arg_3_1_index_func_name == "__sub__"){
                                                arg_3_file << "-";
                                            }
                                            auto *tst_1 = arg_3_1_f->back(); 
                                            arg_3_file << *tst_1 << "\n";
                                        }
                                        else{
                                            arg_3_file << "0\n";
                                        }
                                        auto *arg_3_2_index = cast<CallInstr>(m_call_3->back())->back();
                                        if(cast<CallInstr>(arg_3_2_index) != NULL){
                                            auto *arg_3_2_f = cast<CallInstr>(arg_3_2_index);
                                            auto *arg_3_2_index_func = util::getFunc(arg_3_2_f->getCallee());
                                            auto arg_3_2_index_func_name = arg_3_2_index_func->getUnmangledName();                                    
                                            if(arg_3_2_index_func_name == "__sub__"){
                                                arg_3_file << "-";
                                            }
                                            auto *tst_1 = arg_3_2_f->back(); 
                                            arg_3_file << *tst_1 << "\n";
                                        }
                                        else{
                                            arg_3_file << "0\n";
                                        }                                                  
                                        arg_3_file.close();

                                    }
                                    
                                }
                            }
                            else{
                                MyFile << "0\n";
                            }                        
                        }
                        else{
                            auto *arg3_fr = cast<CallInstr>(arg3_inst->front()); // the left side operand + its row index
                            auto *arg3_var = arg3_fr->front(); // the left side variable
                            auto *arg3_row = arg3_fr->back(); // the left side row index 
                            auto *arg3_col = arg3_inst->back(); // the left side column index
                            std::vector<codon::ir::Var *> arg3_var_name = arg3_var->getUsedVariables();
                            auto *arg3_row_instr = cast<CallInstr>(arg3_row); // is arg3 row index a math op?
                            auto *arg3_col_instr = cast<CallInstr>(arg3_col); // is arg3 row index a math op? 
                            auto arg3_fr_name = arg3_var_name[0]->getName(); // left side variable's name                           
                            //if (arg3_fr_name == pf_arg1){
                            MyFile << "1\n";
                            //}
                            //if (arg3_fr_name == pf_arg2){
                            //    MyFile << "1\n";
                            //}                
                            std::string arg3_row_var_name = ""; // taking care of the row index of the first (left side) operand
                            if (arg3_row_instr != NULL){
                                auto *arg3_row_func = util::getFunc(arg3_row_instr->getCallee());
                                auto arg3_row_func_name = arg3_row_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row_instr->front()->getUsedVariables();
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                if (arg3_row_func_name == "__sub__"){
                                    MyFile << "-";
                                }            
                                auto *arg3_num_row = arg3_row_instr->back();
                                MyFile << *arg3_num_row << "\n";            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row->getUsedVariables();            
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                MyFile << "0\n";
                            }
                            std::string arg3_col_var_name = ""; // taking care of the column index of the first (left side) operand
                            if (arg3_col_instr != NULL){
                                auto *arg3_col_func = util::getFunc(arg3_col_instr->getCallee());
                                auto arg3_col_func_name = arg3_col_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col_instr->front()->getUsedVariables();
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                if (arg3_col_func_name == "__sub__"){
                                    MyFile << "-";
                                }            
                                auto *arg3_num_col = arg3_col_instr->back();
                                MyFile << *arg3_num_col << "\n0\n";            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col->getUsedVariables();            
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    MyFile << "0\n";
                                }
                                else{
                                    MyFile << "1\n";
                                }
                                MyFile << "0\n0\n";
                            }
                        }
                    }
                }

            }

        }
        else
            return;
        auto *arg1 = cast<CallInstr>(right_side->front()); // the left side operand
        auto *arg1_inst = cast<CallInstr>(arg1->front());
        std::string arg1_func_name = "";
        if(arg1_inst != NULL){
            arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();   
        }
        if (arg1_inst != NULL && arg1_func_name != "max_store"){      
            auto *arg1_op = cast<CallInstr>(arg1_inst->front()); // the left side operand        
            auto *arg1_fr = cast<CallInstr>(arg1_op->front()); // the left side variable + its row index                         
            if(arg1_fr != NULL){
                auto *arg1_var = arg1_fr->front(); // the left side variable              
                std::vector<codon::ir::Var *> arg1_vars = arg1_var->getUsedVariables();
                auto arg1_var_name = arg1_vars[0]->getName(); 
                MyFile << arg1_var_name << "\n";  
            }
            else{
                std::vector<codon::ir::Var *> arg1_vars = arg1_op->front()->getUsedVariables();
                auto arg1_var_name = arg1_vars[0]->getName(); 
                MyFile << arg1_var_name << "\n";                  
            }
        }
        else{
            MyFile << "0" << "\n";        
        }
        auto *r_mid_0 = cast<CallInstr>(right_side->front());
        std::__wrap_iter<codon::ir::Value **> r_mid = r_mid_0->begin();        
        auto *arg2_inst = cast<CallInstr>(r_mid[1]);     
        std::string arg2_func_name = "";
        if (arg2_inst != NULL){
            arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();             
        }          
        if (arg2_inst != NULL && arg2_func_name != "max_store"){
            auto *arg2_op = cast<CallInstr>(arg2_inst->front()); // the left side operand
            auto *arg2_fr = cast<CallInstr>(arg2_op->front()); // the left side variable + its row index   
            if(arg2_fr != NULL){              
                auto *arg2_var = arg2_fr->front(); // the left side variable              
                std::vector<codon::ir::Var *> arg2_vars = arg2_var->getUsedVariables();            
                auto arg2_var_name = arg2_vars[0]->getName();
                MyFile << arg2_var_name << "\n";
            }
            else{
                std::vector<codon::ir::Var *> arg2_vars = arg2_op->front()->getUsedVariables();            
                auto arg2_var_name = arg2_vars[0]->getName();
                MyFile << arg2_var_name << "\n";                
            }
        }
        else{
            MyFile << "0" << "\n";
        }
        auto *r_end = cast<CallInstr>(right_side->front())->back(); // the last argument of the main function (min or max)
        if (r_end != r_mid[1]){        
            auto *arg3 = cast<CallInstr>(right_side->back()); // the right side operand                         
            auto *arg3_inst = cast<CallInstr>(arg3->back()); 
            std::string arg3_func_name = "";
            if(arg3_inst != NULL){
                arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();   
            }       
            if (arg3_inst != NULL && arg3_func_name != "max_store"){
                auto *arg3_op = cast<CallInstr>(arg3_inst->front()); // the left side operand
                auto *arg3_fr = cast<CallInstr>(arg3_op->front()); // the left side variable + its row index                       
                if(arg3_fr != NULL){
                    auto *arg3_var = arg3_fr->front(); // the left side variable                                                                                
                    std::vector<codon::ir::Var *> arg3_vars = arg3_var->getUsedVariables();            
                    auto arg3_var_name = arg3_vars[0]->getName();                  
                    MyFile << arg3_var_name << "\n";                
                }
                else{
                    std::vector<codon::ir::Var *> arg3_vars = arg3_op->front()->getUsedVariables();            
                    auto arg3_var_name = arg3_vars[0]->getName();                  
                    MyFile << arg3_var_name << "\n";                                
                }
            }
            else{
                MyFile << "0" << "\n";
            }
        }                           
        MyFile << left_side_name << "\n";            
         
        MyFile.close();         
    }
    else{
        return;
    }
}


void LoopAnalyzer::handle(ImperativeForFlow *v) { transform(v); }

} // namespace vectron


