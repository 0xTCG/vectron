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
#include <unordered_map> 

namespace vectron {
extern std::unordered_map<std::string, std::map<std::string, std::string>> globalAttributes; 
using namespace codon::ir;

void LoopAnalyzer::transform(ImperativeForFlow *v) {
    int arg1_flag = 0;
    int arg2_flag = 0;
    int arg3_flag = 0;
    int flag = 0;
    auto *pf = getParentFunc();;
    auto att_att = util::hasAttribute(pf, "std.vectron.dispatcher.vectron_scheduler");
    auto att_calc = util::hasAttribute(pf, "std.vectron.dispatcher.vectron_kernel");
    if (att_att){   
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
        std::string prep_00 = "";
        if (call_start_outer){ // writing the starting condition of the loop
            auto *start_func = util::getFunc(call_start_outer->getCallee());
            auto final_start = start_func->getUnmangledName();
            prep_00 = final_start;
        }
        else{   
            std::ostringstream oss;                                                                             
            oss << *s_outer;
            std::string s_outer_str = oss.str(); 
            prep_00 = s_outer_str;    
        }
        std::string prep_01 = std::to_string(v->getStep());
        std::string prep_02 = "";
        std::string prep_03 = "";
        if (call_end_outer){ // writing the end condition of the loop
            auto *end_func = util::getFunc(call_end_outer->getCallee());
            auto final_end = end_func->getUnmangledName();            
            if (final_end == "len"){
                std::vector<codon::ir::Var *> len_arg = call_end_outer->back()->getUsedVariables();
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    prep_02 = "-1";
                    prep_03 = "0";
                }
                else{
                    prep_02 = "-2";                    
                    prep_03 = "0";
                }
            }
            else{
                auto *right_end = call_end_outer->back();                
                std::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_outer->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    prep_02 = "-1";
                }
                else{                  
                    prep_02 = "-1";
                }
                if (final_end == "__sub__"){
                    prep_03 = "-";
                }
                std::ostringstream oss_right_end;                                                                             
                oss_right_end << *right_end;
                std::string s_right_end_str = oss_right_end.str();                 
                prep_03 += s_right_end_str;             
            }
        }
        else{
            std::ostringstream oss_e_outer;                                                                             
            oss_e_outer << *e_outer;
            std::string oss_e_outer_str = oss_e_outer.str();                             
            prep_02 = oss_e_outer_str;
            prep_03 = "0";
        }
        std::string prep_04 = "";
        if (call_start_inner){ // writing the starting condition of the loop
            auto *start_func_inner = util::getFunc(call_start_inner->getCallee());
            auto final_start_inner = start_func_inner->getUnmangledName();
            prep_04 = final_start_inner;
        }
        else{     
            std::ostringstream oss_s_inner;                                                                             
            oss_s_inner << *s_inner;
            std::string oss_s_inner_str = oss_s_inner.str();                             
            prep_04 = oss_s_inner_str;              
        }
        std::string prep_05 = std::to_string(inner_loop->getStep());
        std::string prep_06 = "";
        std::string prep_07 = "";
        if (call_end_inner){ // writing the end condition of the loop
            auto *end_func_inner = util::getFunc(call_end_inner->getCallee());
            auto final_end_inner = end_func_inner->getUnmangledName();
            if (final_end_inner == "len"){                
                std::vector<codon::ir::Var *> len_arg_inner = call_end_inner->back()->getUsedVariables();
                auto len_arg_name_inner = len_arg_inner[0]->getName();        
                if (len_arg_name_inner == pf_arg1){
                    prep_06 = "-1";
                    prep_07 = "0";
                }
                else{
                    prep_06 = "-2";
                    prep_07 = "0";
                }
            }
            else{
                auto *right_end_inner = call_end_inner->back();  
                std::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_inner->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    prep_06 = "-1";
                }
                else{
                    prep_06 = "-2";             
                }                              
                if (final_end_inner == "__sub__"){
                    prep_07 = "-";
                }
                std::ostringstream oss_right_end_inner;  
                oss_right_end_inner << *right_end_inner;
                std::string oss_right_end_inner_str = oss_right_end_inner.str();                             
                prep_07 += oss_right_end_inner_str;                     
            }
        }
        else{
            std::ostringstream oss_e_inner;  
            oss_e_inner << *e_inner;
            std::string oss_e_inner_str = oss_e_inner.str();                             
            prep_06 += oss_e_inner_str;                
            prep_07 = "0";
        } 
        std::map<std::string, std::string> prep_attributes{{"prep_00", prep_00}, {"prep_01", prep_01}, {"prep_02", prep_02}, {"prep_03", prep_03}, {"prep_04", prep_04}, {"prep_05", prep_05}, {"prep_06", prep_06}, {"prep_07", prep_07}}; 
        globalAttributes["prep"] = prep_attributes;                       
    }    
    else if (att_calc){    
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
        std::string bw_00 = "";
        std::string bw_01 = "";
        std::string bw_02 = "";
        std::string bw_03 = "";
        std::string bw_04 = "";
        std::string bw_05 = "";
        std::string bw_06 = "";
        std::string bw_07 = "";
        std::string bw_08 = "";
        if(chk == NULL){
            bw_00 = "0";
            bw_01 = "0";
            bw_02 = "0";
            bw_03 = "0";
            bw_04 = "0";
            bw_05 = "0";
            bw_06 = "0";
            bw_07 = "0";
            bw_08 = "0";
            flag = 1;
        }     
        else{             
            auto *main_true_branch = cast<IfFlow>(cast<SeriesFlow>(inner_loop->getBody())->back())->getCond();
            std::vector<codon::ir::Value *> main_if_ops = main_true_branch->getUsedValues();
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
                bw_00 = "2";
            }
            else{
                if(and_check == "false"){
                    bw_00 = "1";
                }
                else{
                    bw_00 = "0";
                }
            }
            if (and_check == "false" || or_check == "true"){
                auto *left_cond = cast<CallInstr>(main_if_ops[0]);
                std::vector<codon::ir::Value *> left_cond_vars = left_cond->getUsedValues();

                auto *left_operator = util::getFunc(left_cond_vars[2]);
                auto left_op_name = left_operator->getUnmangledName();
                if (left_op_name == "__eq__"){
                    bw_01 = "0";
                }
                else{
                    if (left_op_name == "__gt__"){
                        bw_01 = "1";
                    }
                    else{
                        if(left_op_name == "__ge__"){
                            bw_01 = "2";
                        }
                        else{
                            if(left_op_name == "__lt__"){
                                bw_01 = "-1";
                            }
                            else{
                                bw_01 = "-2";
                            }
                        }
                    }
                }            
                auto *left_lhs = cast<CallInstr>(left_cond_vars[0]); // the last op of the lhs of first condition
                if(left_lhs == NULL){
                    std::vector<codon::ir::Var *> var_lhs = left_cond_vars[0]->getUsedVariables();
                    auto lhs_name = var_lhs[0]->getName();
                    if (lhs_name == var_str_outer){
                        bw_02 = "1";
                    }
                    else{
                        if(lhs_name == var_str_inner){
                            bw_02 = "2";
                        }
                        else{
                            bw_02 = "0";
                        }
                    }
                    bw_03 = "0";

                }
                else{
                    auto last_lhs_op = util::getFunc(left_lhs->getCallee())->getUnmangledName(); 
                    auto *left_lhs_lhs = left_lhs->front();
                    auto *left_lhs_lhs_func = cast<CallInstr>(left_lhs_lhs); // the second to last op of the lhs of first condition
                    if(left_lhs_lhs_func == NULL){
                        std::vector<codon::ir::Var *> var_lhs_lhs = left_lhs_lhs->getUsedVariables();
                        auto lhs_lhs_name = var_lhs_lhs[0]->getName();
                        if (lhs_lhs_name == var_str_outer){
                            bw_02 = "1";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                bw_02 = "2";
                            }
                            else{
                                bw_02 = "0";
                            }
                        }
                        auto *left_lhs_rhs = left_lhs->back();
                        std::vector<codon::ir::Var *> var_lhs_rhs = left_lhs_rhs->getUsedVariables();
                        auto lhs_rhs_name = var_lhs_rhs[0]->getName();
                        if (lhs_rhs_name == var_str_outer){
                            if (last_lhs_op == "__sub__"){
                                bw_03 += "-";
                            }
                            bw_03 += "1";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                if (last_lhs_op == "__sub__"){
                                    bw_03 += "-";
                                }                
                                bw_03 += "2";            
                            }
                            else{
                                bw_03 = "0";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(left_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *first_lhs_operand = left_lhs_lhs_func->back();
                        std::vector<codon::ir::Var *> first_var = first_lhs_operand->getUsedVariables();
                        auto first_var_name = first_var[0]->getName();
                        if (first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_02 += "-";
                            }                
                            bw_02 += "1";            
                        }
                        else{
                            if(first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_02 += "-";
                                }                
                                bw_02 += "2";                             
                            }
                            else{
                                bw_02 = "0";
                            }                        
                        }
                                    
                                    
                        auto *left_lhs_rhs = left_lhs->back();                  
                        std::vector<codon::ir::Var *> second_var = left_lhs_rhs->getUsedVariables();                
                        auto second_var_name = second_var[0]->getName();    
                        if (second_var_name == var_str_outer){
                            if(last_lhs_op == "__sub__"){
                                bw_03 += "-";
                            }                
                            bw_03 += "1";        
                        }
                        else{
                            if(second_var_name == var_str_inner){    
                                if(last_lhs_op == "__sub__"){
                                    bw_03 += "-";
                                }                
                                bw_03 += "2";                            
                            }
                            else{
                                bw_03 = "0";
                            }                        
                        }  
                    }                                       

                }
                std::ostringstream oss_left_cond_vars;  
                oss_left_cond_vars << *left_cond_vars[1];
                std::string oss_left_cond_vars_str = oss_left_cond_vars.str();                             
                bw_04 = oss_left_cond_vars_str;                       
                
                auto *right_cond = cast<CallInstr>(main_if_ops[2]);
                if (and_check == "false"){
                    right_cond = cast<CallInstr>(main_if_ops[1]); 
                }                
                std::vector<codon::ir::Value *> right_cond_vars = right_cond->getUsedValues();    
                int build_flag = 0;   
                auto *right_operator = util::getFunc(cast<CallInstr>(right_cond_vars[0])->getCallee());
                auto right_op_name = right_operator->getUnmangledName();
                if (right_op_name == "__eq__"){
                    bw_05 = "0";
                }
                else{
                    if (right_op_name == "__gt__"){
                        bw_05 = "1";
                    }
                    else{
                        if(right_op_name == "__ge__"){
                            bw_05 = "2";
                        }
                        else{
                            if(right_op_name == "__lt__"){
                                bw_05 = "-1";
                            }
                            else{
                                if(right_op_name == "__le__"){
                                    bw_05 = "-2";
                                }  
                                else{
                                    build_flag = 1;
                                    right_operator =  util::getFunc(cast<CallInstr>(right_cond)->getCallee());
                                    right_op_name = right_operator->getUnmangledName();
                                    if (right_op_name == "__eq__"){
                                            bw_05 = "0";
                                        }
                                    else{
                                        if (right_op_name == "__gt__"){
                                            bw_05 = "1";
                                        }
                                        else{
                                            if(right_op_name == "__ge__"){
                                                bw_05 = "2";
                                            }
                                            else{
                                                if(right_op_name == "__lt__"){
                                                    bw_05 = "-1";
                                                }
                                                else{
                                                    if(right_op_name == "__le__"){
                                                        bw_05 = "-2";
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
                    std::vector<codon::ir::Var *> r_var_lhs = right->getUsedVariables();
                    auto r_lhs_name = r_var_lhs[0]->getName();
                    if (r_lhs_name == var_str_outer){
                        bw_06 = "1";
                    }
                    else{
                        if(r_lhs_name == var_str_inner){
                            bw_06 = "2";
                        }
                        else{
                            bw_06 = "0";
                        }
                    }
                    bw_07 = "0";

                }
                else{
                    auto r_last_lhs_op = util::getFunc(right_lhs->getCallee())->getUnmangledName(); 
                    auto *right_lhs_lhs = right_lhs->front();
                    auto *right_lhs_lhs_func = cast<CallInstr>(right_lhs_lhs); // the second to last op of the lhs of first condition
                    if(right_lhs_lhs_func == NULL){
                        std::vector<codon::ir::Var *> r_var_lhs_lhs = right_lhs_lhs->getUsedVariables();
                        auto r_lhs_lhs_name = r_var_lhs_lhs[0]->getName();
                        if (r_lhs_lhs_name == var_str_outer){
                            bw_06 = "1";
                        }
                        else{
                            if(r_lhs_lhs_name == var_str_inner){
                                bw_06 = "2";
                            }
                            else{
                                bw_06 = "0";
                            }
                        }
                        auto *right_lhs_rhs = right_lhs->back();
                        std::vector<codon::ir::Var *> r_var_lhs_rhs = right_lhs_rhs->getUsedVariables();
                        auto r_lhs_rhs_name = r_var_lhs_rhs[0]->getName();
                        if (r_lhs_rhs_name == var_str_outer){
                            if (r_last_lhs_op == "__sub__"){
                                bw_07 += "-";
                            }
                            bw_07 += "1";
                        }
                        else{
                            if(r_lhs_lhs_name == var_str_inner){
                                if (r_last_lhs_op == "__sub__"){
                                    bw_07 += "-";
                                }                
                                bw_07 += "2";            
                            }
                            else{
                                bw_07 = "0";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(right_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *r_first_lhs_operand = right_lhs_lhs_func->back();
                        std::vector<codon::ir::Var *> r_first_var = r_first_lhs_operand->getUsedVariables();
                        auto r_first_var_name = r_first_var[0]->getName();
                        if (r_first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_06 += "-";
                            }                
                            bw_06 += "1";            
                        }
                        else{
                            if(r_first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_06 += "-";
                                }              
                                bw_06 += "2";                               
                            }
                            else{
                                bw_06 = "0";
                            }                        
                        }      
                                    
                        auto *right_lhs_rhs = right_lhs->back();                  
                        std::vector<codon::ir::Var *> r_second_var = right_lhs_rhs->getUsedVariables();                
                        auto r_second_var_name = r_second_var[0]->getName();    
                        if (r_second_var_name == var_str_outer){
                            if(r_last_lhs_op == "__sub__"){
                                bw_07 += "-";
                            }                
                            bw_07 += "1";        
                        }
                        else{
                            if(r_second_var_name == var_str_inner){    
                                if(r_last_lhs_op == "__sub__"){
                                    bw_07 += "-";
                                }         
                                bw_07 += "2";                                   
                            }
                            else{
                                bw_07 = "0";
                            }                        
                        }  
                    }                                       

                }
                auto *right_rhs = cast<CallInstr>(right_cond_vars[0])->back();
                if(build_flag == 1){
                    right_rhs = right_cond->back();
                }
                std::ostringstream oss_right_rhs;  
                oss_right_rhs << *right_rhs;
                std::string oss_right_rhs_str = oss_right_rhs.str();                             
                bw_08 = oss_right_rhs_str;                       
            }              
            else{     
                auto *single_left_cond = cast<CallInstr>(main_true_branch);
                //std::vector<codon::ir::Value *> left_cond_vars = left_cond->getUsedValues();
                auto *left_operator = util::getFunc(single_left_cond->getCallee());
                auto left_op_name = left_operator->getUnmangledName();
                if (left_op_name == "__eq__"){
                    bw_01 = "0";
                }
                else{
                    if (left_op_name == "__gt__"){
                        bw_01 = "1";
                    }
                    else{
                        if(left_op_name == "__ge__"){
                            bw_01 = "2";
                        }
                        else{
                            if(left_op_name == "__lt__"){
                                bw_01 = "-1";
                            }
                            else{
                                bw_01 = "-2";
                            }
                        }
                    }
                }            

                auto *left_lhs = cast<CallInstr>(main_true_branch); // the last op of the lhs of first condition
                auto *op_check = cast<CallInstr>(left_lhs->front());
                std::vector<codon::ir::Value *> left_cond_vars = left_lhs->getUsedValues();             
                if(op_check == NULL){
                    std::vector<codon::ir::Var *> var_lhs = left_cond_vars[0]->getUsedVariables();
                    auto lhs_name = var_lhs[0]->getName();
                    if (lhs_name == var_str_outer){
                        bw_02 = "1";
                    }
                    else{
                        if(lhs_name == var_str_inner){
                            bw_02 = "2";
                        }
                        else{
                            bw_02 = "0";
                        }
                    }
                    bw_03 = "0";

                }
                else{
                    auto last_lhs_op = util::getFunc(op_check->getCallee())->getUnmangledName(); 
                    auto *left_lhs_lhs = op_check->front();
                    auto *left_lhs_lhs_func = cast<CallInstr>(left_lhs_lhs); // the second to last op of the lhs of first condition
                    if(left_lhs_lhs_func == NULL){
                        std::vector<codon::ir::Var *> var_lhs_lhs = left_lhs_lhs->getUsedVariables();
                        auto lhs_lhs_name = var_lhs_lhs[0]->getName();
                        if (lhs_lhs_name == var_str_outer){
                            bw_02 = "1";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                bw_02 = "2";
                            }
                            else{
                                bw_02 = "0";
                            }
                        }
                        auto *left_lhs_rhs = op_check->back();
                        std::vector<codon::ir::Var *> var_lhs_rhs = left_lhs_rhs->getUsedVariables();
                        auto lhs_rhs_name = var_lhs_rhs[0]->getName();
                        if (lhs_rhs_name == var_str_outer){
                            if (last_lhs_op == "__sub__"){
                                bw_03 += "-";
                            }
                            bw_03 += "1";
                        }
                        else{
                            if(lhs_lhs_name == var_str_inner){
                                if (last_lhs_op == "__sub__"){
                                    bw_03 += "-";
                                }              
                                bw_03 += "2";              
                            }
                            else{
                                bw_03 = "0";
                            }
                        }                           

                    }
                    else{         
                        auto neg_test_name = util::getFunc(left_lhs_lhs_func->getCallee())->getUnmangledName();
                        auto *first_lhs_operand = left_lhs_lhs_func->back();
                        std::vector<codon::ir::Var *> first_var = first_lhs_operand->getUsedVariables();
                        auto first_var_name = first_var[0]->getName();
                        if (first_var_name == var_str_outer){
                            if(neg_test_name == "__neg__"){
                                bw_02 += "-";
                            }                
                            bw_02 += "1";            
                        }
                        else{
                            if(first_var_name == var_str_inner){       
                                if(neg_test_name == "__neg__"){
                                    bw_02 += "-";
                                }         
                                bw_02 += "2";                                    
                            }
                            else{
                                bw_02 = "0";
                            }                        
                        }
                                    
                                    
                        auto *left_lhs_rhs = left_lhs->back();                  
                        std::vector<codon::ir::Var *> second_var = left_lhs_rhs->getUsedVariables();                
                        auto second_var_name = second_var[0]->getName();    
                        if (second_var_name == var_str_outer){
                            if(last_lhs_op == "__sub__"){
                                bw_03 += "-";
                            }                
                            bw_03 += "1";        
                        }
                        else{
                            if(second_var_name == var_str_inner){    
                                if(last_lhs_op == "__sub__"){
                                    bw_03 += "-";
                                }   
                                bw_03 += "2";                                         
                            }
                            else{
                                bw_03 += "0";
                            }                        
                        }  
                    }                                       

                }
                std::ostringstream oss_main_if_ops;  
                oss_main_if_ops << *main_if_ops[1];
                std::string oss_main_if_ops_str = oss_main_if_ops.str();                             
                bw_04 = oss_main_if_ops_str;                  
                bw_05 = "0";            
                bw_06 = "0";
                bw_07 = "0";
                bw_08 = "0";
            }
            std::map<std::string, std::string> bw_attributes{{"bw_00", bw_00}, {"bw_01", bw_01}, {"bw_02", bw_02}, {"bw_03", bw_03}, {"bw_04", bw_04}, {"bw_05", bw_05}, {"bw_06", bw_06}, {"bw_07", bw_07}, {"bw_08", bw_08}}; 
            globalAttributes["bw"] = bw_attributes;                   
        } 
        std::string params_00 = "";
        std::string params_01 = "";
        std::string params_02 = "";
        std::string params_03 = "";
        std::string params_04 = "";
        std::string params_05 = "";
        std::string params_06 = "";
        std::string params_07 = "";
        std::string params_08 = "";
        std::string params_09 = "";
        std::string params_10 = "";
        std::string params_11 = "";
        std::string params_12 = "";
        std::string params_13 = "";
        std::string params_14 = "";
        std::string params_15 = "";
        std::string params_16 = "";
        std::string params_17 = ""; 
        std::string params_18 = "";
        std::string params_19 = "";
        std::string params_20 = "";
        std::string params_21 = "";
        std::string params_22 = "";
        std::string params_23 = "";
        std::string params_24 = "";
        std::string params_25 = "";
        std::string params_26 = "";
        std::string params_27 = "";
        std::string params_28 = "";
        std::string params_29 = "";
        std::string params_30 = "";
        std::string params_31 = "";
        std::string params_32 = "";
        std::string params_33 = "";        
        auto *ass = cast<CallInstr>(cast<SeriesFlow>(inner_loop->getBody())->back()); // assignment op inside the inner loop    
        if (flag == 0){
            auto *main_else_branch = cast<IfFlow>(cast<SeriesFlow>(inner_loop->getBody())->back())->getFalseBranch();                        
            ass = cast<CallInstr>(cast<SeriesFlow>(main_else_branch)->back()); // assignment op inside the inner loop                
        }
        auto *right_side = cast<CallInstr>(ass->back()); // the right side of the assignment operation
        std::vector<codon::ir::Var *> left_side = cast<CallInstr>(ass->front())->front()->getUsedVariables();
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

        if (call_start_outer){ // writing the starting condition of the loop
            auto *start_func = util::getFunc(call_start_outer->getCallee());
            auto final_start = start_func->getUnmangledName();
            params_00 = final_start;
        }
        else{   
            std::ostringstream oss_s_outer_2;  
            oss_s_outer_2 << *s_outer;
            std::string oss_s_outer_2_str = oss_s_outer_2.str();                             
            params_00 = oss_s_outer_2_str;                
        }
        params_01 = std::to_string(v->getStep());
        
        if (call_end_outer){ // writing the end condition of the loop
            auto *end_func = util::getFunc(call_end_outer->getCallee());
            auto final_end = end_func->getUnmangledName();            
            if (final_end == "len"){
                std::vector<codon::ir::Var *> len_arg = call_end_outer->back()->getUsedVariables();
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    params_02 = "-1";
                    params_03 = "0";
                }
                else{
                    params_02 = "-2";
                    params_03 = "0";                      
                }
            }
            else{
                auto *right_end = call_end_outer->back(); 
                std::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_outer->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    params_02 = "-1";
                }
                else{
                    params_02 = "-2";                 
                }
                if (final_end == "__sub__"){
                    params_03 += "-";
                }
                std::ostringstream oss_right_end;  
                oss_right_end << *right_end;
                std::string oss_right_end_str = oss_right_end.str();                             
                params_03 += oss_right_end_str;  
            }
        }
        else{
            std::ostringstream oss_e_outer;  
            oss_e_outer << *e_outer;
            std::string oss_e_outer_str = oss_e_outer.str();                             
            params_02 = oss_e_outer_str;
            params_03 = "0";              
        }        
        if (call_start_inner){ // writing the starting condition of the loop
            auto *start_func_inner = util::getFunc(call_start_inner->getCallee());
            auto final_start_inner = start_func_inner->getUnmangledName();
            params_04 = final_start_inner;
        }
        else{  
            std::ostringstream oss_s_inner;  
            oss_s_inner << *s_inner;
            std::string oss_s_inner_str = oss_s_inner.str();                             
            params_04 = oss_s_inner_str;                   
        }
        params_05 = std::to_string(inner_loop->getStep());
        if (call_end_inner){ // writing the end condition of the loop
            auto *end_func_inner = util::getFunc(call_end_inner->getCallee());
            auto final_end_inner = end_func_inner->getUnmangledName();
            if (final_end_inner == "len"){                
                //auto *len_arg_fr = cast<CallInstr>(cast<CallInstr>(call_end_inner)->front())->front();
                //std::vector<codon::ir::Var *> end_inner_var = len_arg_fr->getUsedVariables();
                //auto len_arg = end_inner_var[0]->getName();
                //auto len_arg = end_func_inner->arg_front()->getName();
                std::vector<codon::ir::Var *> len_arg_inner = call_end_inner->back()->getUsedVariables();
                auto len_arg_name_inner = len_arg_inner[0]->getName();        
                if (len_arg_name_inner == pf_arg1){
                    params_06 = "-1";
                    params_07 = "0";
                }
                else{
                    params_06 = "-2";
                    params_07 = "0";                       
                }
            }
            else{
                auto *right_end_inner = call_end_inner->back();                
                std::vector<codon::ir::Var *> len_arg = cast<CallInstr>(call_end_inner->front())->back()->getUsedVariables();                               
                auto len_arg_name = len_arg[0]->getName();
                if (len_arg_name == pf_arg1){
                    params_06 = "-1";
                }
                else{
                    params_06 = "-2";                   
                }                          
                if (final_end_inner == "__sub__"){
                    params_07 += "-";
                }
                std::ostringstream oss_right_end_inner;  
                oss_right_end_inner << *right_end_inner;
                std::string oss_right_end_inner_str = oss_right_end_inner.str();                             
                params_07 += oss_right_end_inner_str;                   
            }
        }
        else{
            std::ostringstream oss_e_inner;  
            oss_e_inner << *e_inner;
            std::string oss_e_inner_str = oss_e_inner.str();                             
            params_06 = oss_e_inner_str;             
            params_07 = "0";  
        }                
        if (op == "min"){
            params_08 = "0";
        }
        else{ 
            params_08 = "1";
        }
              
        if (op == "max" || op == "min"){
            auto *arg1 = cast<CallInstr>(right_side->front()); // the left side operand
            auto *arg1_inst = cast<CallInstr>(arg1->front()); 
                            
            auto r_mid = cast<CallInstr>(right_side->front())->begin();
            auto *arg2_inst = cast<CallInstr>(r_mid[1]);        
                
            auto *r_end = cast<CallInstr>(right_side->front())->back(); // the last argument of the main function (min or max)
            int main_args = 0;
            if (r_end != r_mid[1]){
                main_args = 1;
            }                                         

            if (arg1_inst == NULL){
                params_09 = "0";
            }
            else{
                auto arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();
                auto *max_st_1 = util::getFunc(arg1_inst->getCallee());
                auto max_att_1 = util::hasAttribute(max_st_1, "std.vectron.dispatcher.vectron_max");                                
                if (arg1_func_name == "__add__" || arg1_func_name == "__sub__" || arg1_func_name == "__mul__" || arg1_func_name == "__div__" ){
                    params_09 = "1";
                }
                else{
                    if(max_att_1){
                        params_09 = "0"; 
                    }
                    else{
                        params_09 = "1";
                    }
                }
            }
            if (arg2_inst == NULL){
                params_10 = "0";
            }
            else{
                auto arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();
                auto *max_st_2 = util::getFunc(arg2_inst->getCallee());
                auto max_att_2 = util::hasAttribute(max_st_2, "std.vectron.dispatcher.vectron_max");                                
                if (arg2_func_name == "__add__" || arg2_func_name == "__sub__" || arg2_func_name == "__mul__" || arg2_func_name == "__div__"){
                    params_10 = "1";
                }
                else{
                    if(max_att_2){
                        params_10 = "0";  
                    }
                    else{
                        params_10 = "1";
                    }
                }                    
            }
            if (main_args == 0){  
                params_11 = "-1";          
            }
            else{                                                                                    
                auto *arg3 = cast<CallInstr>(right_side->front()); // the right side operand                                         
                auto *arg3_inst = cast<CallInstr>(arg3->back());                                                              
                if (arg3_inst == NULL){
                    params_11 = "0";
                }
                else{
                    auto arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();   
                    auto *max_st_3 = util::getFunc(arg3_inst->getCallee());
                    auto max_att_3 = util::hasAttribute(max_st_3, "std.vectron.dispatcher.vectron_max");                                    
                    if (arg3_func_name == "__add__" || arg3_func_name == "__sub__" || arg3_func_name == "__mul__" || arg3_func_name == "__div__"){
                        params_11 = "1";
                    }
                    else{
                        if(max_att_3){
                            params_11 = "0"; 
                        }
                        else{
                            params_11 = "1";
                        }
                    }                               
                }                
            }
            if (arg1_inst == NULL){
                params_12 = "0";
                params_13 = "0";
                params_14 = "0";
                params_15 = "0";
                params_16 = "0";
                std::ostringstream oss_arg_1;  
                oss_arg_1 << *arg1->front();
                std::string oss_arg_1_str = oss_arg_1.str();                             
                params_17 = oss_arg_1_str;                  
                arg1_flag = 1;
            }
            else{
                auto arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();
                auto *max_st_4 = util::getFunc(arg1_inst->getCallee());
                auto max_att_4 = util::hasAttribute(max_st_4, "std.vectron.dispatcher.vectron_max");                
                if(max_att_4){
                    params_12 = "0";
                    params_13 = "0";
                    params_14 = "0";
                    params_15 = "0";
                    params_16 = "0";
                    params_17 = "0";                    

                    std::string mx1_00 = "";
                    std::string mx1_01 = "";
                    std::string mx1_02 = "";
                    std::string mx1_03 = "";
                    std::string mx1_04 = "";
                    std::string mx1_05 = "";
                    std::string mx1_06 = "";
                    std::string mx1_07 = ""; 
                    std::string mx1_08 = ""; 
                    std::string mx1_09 = ""; 
                    std::string mx1_10 = ""; 


                    std::vector<codon::ir::Value *> mx_arg_1 = arg1_inst->getUsedValues();
                    std::vector<codon::ir::Var *> mx_arg_1_1 = mx_arg_1[0]->getUsedVariables();
                    auto mx_arg_1_1_name = mx_arg_1_1[0]->getName();
                    mx1_00 = mx_arg_1_1_name;
                    auto *mx_arg_1_2 = cast<CallInstr>(mx_arg_1[1]);
                    if(mx_arg_1_2 == NULL){
                        mx1_01 = "0";
                    }
                    else{
                        auto mx_arg_1_2_func = util::getFunc(mx_arg_1_2->getCallee())->getUnmangledName();
                        if(mx_arg_1_2_func == "__sub__"){
                            mx1_01 += "-";
                        }                        
                        auto *mx_arg_1_2_add = mx_arg_1_2->back();
                        std::ostringstream oss_mx_arg_1_2_add;  
                        oss_mx_arg_1_2_add << *mx_arg_1_2_add;
                        std::string oss_mx_arg_1_2_add_str = oss_mx_arg_1_2_add.str();                             
                        mx1_01 += oss_mx_arg_1_2_add_str;                          
                    }
                    auto *mx_arg_1_3 = cast<CallInstr>(mx_arg_1[2]);
                    if(mx_arg_1_3 == NULL){
                        mx1_02 = "0";
                    }
                    else{
                        auto mx_arg_1_3_func = util::getFunc(mx_arg_1_3->getCallee())->getUnmangledName();
                        if(mx_arg_1_3_func == "__sub__"){
                            mx1_02 += "-";
                        }
                        auto *mx_arg_1_3_add = mx_arg_1_3->back();
                        std::ostringstream oss_mx_arg_1_3_add;  
                        oss_mx_arg_1_3_add << *mx_arg_1_3_add;
                        std::string oss_mx_arg_1_3_add_str = oss_mx_arg_1_3_add.str();                             
                        mx1_02 += oss_mx_arg_1_3_add_str;                          
                    }   
                    auto *mx_arg_1_4 = cast<CallInstr>(mx_arg_1[3]);       
                    auto mx_arg_1_4_name = util::getFunc(cast<CallInstr>(mx_arg_1_4)->getCallee())->getUnmangledName();
                    if(mx_arg_1_4_name == "__add__" || mx_arg_1_4_name == "__sub__"){
                        auto *mx_arg_1_4_left = mx_arg_1_4->front();
                        auto *mx_arg_1_4_left_part_1 = cast<CallInstr>(mx_arg_1_4_left)->front();
                        auto *mx_arg_1_4_left_part_1_1 = cast<CallInstr>(mx_arg_1_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx1_03 = part_1_lst_name;
                        auto *mx_arg_1_4_left_part_1_2 = cast<CallInstr>(mx_arg_1_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_left_part_1_2) == NULL){
                            mx1_04 = "0";
                        }
                        else{
                            auto mx_arg_1_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_4_left_part_1_2)->getCallee());
                            auto mx_arg_1_4_inst = mx_arg_1_4_left_inst->getUnmangledName();
                            if(mx_arg_1_4_inst == "__sub__"){
                                mx1_04 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_4_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx1_04 += oss_part_1_add_str;                              
                        }
                        auto *mx_arg_1_4_part_2 = cast<CallInstr>(mx_arg_1_4_left)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_part_2) == NULL){
                            mx1_05 = "0";
                        }
                        else{
                            auto *mx_arg_1_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_4_part_2)->getCallee());
                            auto mx_arg_1_4_part_2_inst = mx_arg_1_4_left_part_2->getUnmangledName();;
                            if(mx_arg_1_4_part_2_inst == "__sub__"){
                                mx1_05 += "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_1_4_part_2)->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx1_05 += oss_part_2_add_str;                              
                        }
                        if(mx_arg_1_4_name == "__sub__"){
                            mx1_06 += "-";
                        }
                        auto *mx_arg_1_4_right = mx_arg_1_4->back(); // final value               
                        std::ostringstream oss_mx_arg_1_4_right;  
                        oss_mx_arg_1_4_right << *mx_arg_1_4_right;
                        std::string oss_mx_arg_1_4_right_str = oss_mx_arg_1_4_right.str();                             
                        mx1_06 += oss_mx_arg_1_4_right_str;                          
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_1_4_left_part_1 = cast<CallInstr>(mx_arg_1_4)->front();
                        auto *mx_arg_1_4_left_part_1_1 = cast<CallInstr>(mx_arg_1_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx1_03 = part_1_lst_name;
                        auto *mx_arg_1_4_left_part_1_2 = cast<CallInstr>(mx_arg_1_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_left_part_1_2) == NULL){
                            mx1_04 = "0";
                        }
                        else{
                            auto mx_arg_1_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_4_left_part_1_2)->getCallee());
                            auto mx_arg_1_4_inst = mx_arg_1_4_left_inst->getUnmangledName();
                            if(mx_arg_1_4_inst == "__sub__"){
                                mx1_04 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_4_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx1_04 += oss_part_1_add_str;                          
                        }
                        auto *mx_arg_1_4_part_2 = cast<CallInstr>(mx_arg_1_4)->back();                        
                        if(cast<CallInstr>(mx_arg_1_4_part_2) == NULL){
                            mx1_05 = "0";
                        }
                        else{
                            auto *mx_arg_1_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_4_part_2)->getCallee());
                            auto mx_arg_1_4_part_2_inst = mx_arg_1_4_left_part_2->getUnmangledName();;
                            if(mx_arg_1_4_part_2_inst == "__sub__"){
                                mx1_05 += "-";
                            }
                            auto *part_2_add = mx_arg_1_4->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx1_05 += oss_part_2_add_str;  
                        }
                        mx1_06 = "0";          
                    }
                    auto *mx_arg_1_5 = cast<CallInstr>(mx_arg_1[4]);       
                    auto mx_arg_1_5_name = util::getFunc(cast<CallInstr>(mx_arg_1_5)->getCallee())->getUnmangledName();
                    if(mx_arg_1_5_name == "__add__" || mx_arg_1_5_name == "__sub__"){
                        auto *mx_arg_1_5_left = mx_arg_1_5->front();
                        auto *mx_arg_1_5_left_part_1 = cast<CallInstr>(mx_arg_1_5_left)->front();
                        auto *mx_arg_1_5_left_part_1_1 = cast<CallInstr>(mx_arg_1_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx1_07 = part_1_lst_name;
                        auto *mx_arg_1_5_left_part_1_2 = cast<CallInstr>(mx_arg_1_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_left_part_1_2) == NULL){
                            mx1_08 = "0";
                        }
                        else{
                            auto mx_arg_1_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_5_left_part_1_2)->getCallee());
                            auto mx_arg_1_5_inst = mx_arg_1_5_left_inst->getUnmangledName();
                            if(mx_arg_1_5_inst == "__sub__"){
                                mx1_08 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_5_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx1_08 += oss_part_1_add_str;                              
                        }
                        auto *mx_arg_1_5_part_2 = cast<CallInstr>(mx_arg_1_5_left)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_part_2) == NULL){
                            mx1_09 = "0";
                        }
                        else{
                            auto *mx_arg_1_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_5_part_2)->getCallee());
                            auto mx_arg_1_5_part_2_inst = mx_arg_1_5_left_part_2->getUnmangledName();;
                            if(mx_arg_1_5_part_2_inst == "__sub__"){
                                mx1_09 += "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_1_5_part_2)->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx1_09 += oss_part_2_add_str;                               
                        }
                        if(mx_arg_1_5_name == "__sub__"){
                            mx1_10 += "-";
                        }
                        auto *mx_arg_1_5_right = mx_arg_1_5->back(); // final value       
                        std::ostringstream oss_mx_arg_1_5_right;  
                        oss_mx_arg_1_5_right << *mx_arg_1_5_right;
                        std::string oss_mx_arg_1_5_right_str = oss_mx_arg_1_5_right.str();                             
                        mx1_10 += oss_mx_arg_1_5_right_str;                                   
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_1_5_left_part_1 = cast<CallInstr>(mx_arg_1_5)->front();
                        auto *mx_arg_1_5_left_part_1_1 = cast<CallInstr>(mx_arg_1_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_1_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx1_07 = part_1_lst_name;
                        auto *mx_arg_1_5_left_part_1_2 = cast<CallInstr>(mx_arg_1_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_left_part_1_2) == NULL){
                            mx1_08 = "0";
                        }
                        else{
                            auto mx_arg_1_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_1_5_left_part_1_2)->getCallee());
                            auto mx_arg_1_5_inst = mx_arg_1_5_left_inst->getUnmangledName();
                            if(mx_arg_1_5_inst == "__sub__"){
                                mx1_08 += "-";
                            } 
                            auto *part_1_add = cast<CallInstr>(mx_arg_1_5_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx1_08 += oss_part_1_add_str;                            
                        }
                        auto *mx_arg_1_5_part_2 = cast<CallInstr>(mx_arg_1_5)->back();                        
                        if(cast<CallInstr>(mx_arg_1_5_part_2) == NULL){
                            mx1_09 = "0";
                        }
                        else{
                            auto *mx_arg_1_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_1_5_part_2)->getCallee());
                            auto mx_arg_1_5_part_2_inst = mx_arg_1_5_left_part_2->getUnmangledName();;
                            if(mx_arg_1_5_part_2_inst == "__sub__"){
                                mx1_09 += "-";
                            }
                            auto *part_2_add = mx_arg_1_5->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx1_09 += oss_part_2_add_str;                                
                        }  
                        mx1_10 = "0";              
                    }                  
                    std::map<std::string, std::string> mx1_attributes{{"mx1_00", mx1_00}, {"mx1_01", mx1_01}, {"mx1_02", mx1_02}, {"mx1_03", mx1_03}, {"mx1_04", mx1_04}, {"mx1_05", mx1_05}, {"mx1_06", mx1_06}, {"mx1_07", mx1_07}, {"mx1_08", mx1_08}, {"mx1_09", mx1_09}, {"mx1_10", mx1_10}}; 
                    globalAttributes["mx1"] = mx1_attributes;                                           
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
                            std::ostringstream oss_arg1_const;  
                            oss_arg1_const << *arg1_const;
                            std::string oss_arg1_const_str = oss_arg1_const.str();                             
                            params_12 += oss_arg1_const_str;                             
                        }
                        else if (arg1_func_name == "__div__"){
                            params_12 = "1/";
                            std::ostringstream oss_arg1_const;  
                            oss_arg1_const << *arg1_const;
                            std::string oss_arg1_const_str = oss_arg1_const.str();                             
                            params_12 += oss_arg1_const_str;    
                        }
                        else{
                            params_12 = "1";
                        }                                                
                        std::string arg1_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg1_row_instr != NULL){
                            auto *arg1_row_func = util::getFunc(arg1_row_instr->getCallee());
                            auto arg1_row_func_name = arg1_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row_instr->front()->getUsedVariables();
                            arg1_row_var_name = arg1_row_var[0]->getName();                            
                            if (var_str_outer == arg1_row_var_name){
                                params_13 = "0";
                            }
                            else{
                                params_13 = "1";
                            }
                            if (arg1_row_func_name == "__sub__"){
                                params_14 += "-";
                            }
                            auto *arg1_num_row = arg1_row_instr->back();        
                            std::ostringstream oss_arg1_num_row;  
                            oss_arg1_num_row << *arg1_num_row;
                            std::string oss_arg1_num_row_str = oss_arg1_num_row.str();                             
                            params_14 += oss_arg1_num_row_str;                                                   
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row->getUsedVariables();            
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                params_13 = "0";
                            }
                            else{
                                params_13 = "1";
                            }
                            params_14 = "0";
                        }
                        std::string arg1_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg1_col_instr != NULL){                              
                            auto *arg1_num_col = arg1_col_instr->back();
                            auto *arg1_num_col_inst = cast<CallInstr>(arg1_num_col);                            
                            auto *arg1_col_func = util::getFunc(arg1_col_instr->getCallee());
                            auto arg1_col_func_name = arg1_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col_instr->front()->getUsedVariables();
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                params_15 = "0";
                            }
                            else{
                                params_15 = "1";
                            }                            
                            if (arg1_col_func_name == "__sub__"){
                                params_16 += "-";
                            }                            
                            if(arg1_num_col_inst == NULL){  
                                std::ostringstream oss_arg1_num_col;  
                                oss_arg1_num_col << *arg1_num_col;
                                std::string oss_arg1_num_col_str = oss_arg1_num_col.str();                             
                                params_16 += oss_arg1_num_col_str;                                                                           
                            }
                            else{
                                auto *arg_1_num_col_left = arg1_num_col_inst->front();
                                auto *arg_1_num_col_right = arg1_num_col_inst->back();
                                std::vector<codon::ir::Var *> arg_1_num_col_left_vars = arg_1_num_col_left->getUsedVariables();
                                auto arg_1_num_col_left_name = arg_1_num_col_left_vars[0]->getName();
                                if(arg_1_num_col_left_name == pf_arg1){
                                    params_16 += "0 ";
                                }
                                else{
                                    params_16 += "1 ";
                                }
                                auto *arg_1_num_col_right_inst = cast<CallInstr>(arg_1_num_col_right);
                                if(arg_1_num_col_right_inst == NULL){
                                    std::vector<codon::ir::Var *> arg_1_num_col_right_vars = arg_1_num_col_right->getUsedVariables();
                                    auto arg_1_num_col_right_name = arg_1_num_col_right_vars[0]->getName();
                                    if(arg_1_num_col_right_name == var_str_inner){
                                        params_16 += "0 0";
                                    }
                                    else{
                                        params_16 += "1 0";
                                    }
                                }
                                else{
                                    auto *arg1_num_col_right_left = arg_1_num_col_right_inst->front();
                                    std::vector<codon::ir::Var *> arg1_num_col_right_left_vars =  arg1_num_col_right_left->getUsedVariables();
                                    auto arg1_num_col_right_left_name = arg1_num_col_right_left_vars[0]->getName();
                                    if(arg1_num_col_right_left_name == var_str_inner){
                                        params_16 += "0 ";
                                    }
                                    else{
                                        params_16 += "1 ";
                                    }
                                    auto *arg1_num_col_right_right = arg_1_num_col_right_inst->back();
                                    auto *arg1_num_col_right_func = util::getFunc(arg_1_num_col_right_inst->getCallee());
                                    auto arg1_num_col_right_func_name = arg1_num_col_right_func->getUnmangledName();
                                    if(arg1_num_col_right_func_name == "__sub__"){
                                        params_16 += "-";
                                    }
                                    std::ostringstream oss_arg1_num_col_right_right;  
                                    oss_arg1_num_col_right_right << *arg1_num_col_right_right;
                                    std::string oss_arg1_num_col_right_right_str = oss_arg1_num_col_right_right.str();                             
                                    params_16 += oss_arg1_num_col_right_right_str;  
                                }
                            }
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col->getUsedVariables();            
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                params_15 = "0";
                            }
                            else{
                                params_15 = "1";
                            }
                            params_16 = "0";
                        }                             
                        if (arg1_func_name == "__sub__"){
                            params_17 += "-";
                            std::ostringstream oss_arg1_const;  
                            oss_arg1_const << *arg1_const;
                            std::string oss_arg1_const_str = oss_arg1_const.str();                             
                            params_17 += oss_arg1_const_str;  
                        }
                        else if (arg1_func_name == "__add__"){
                            if (cast<CallInstr>(arg1_const) == NULL){  
                                if(cast<TernaryInstr>(arg1_const) == NULL){
                                    std::ostringstream oss_arg1_const;  
                                    oss_arg1_const << *arg1_const;
                                    std::string oss_arg1_const_str = oss_arg1_const.str();                             
                                    params_17 += oss_arg1_const_str;  
                                }
                                else{
                                    params_17 = "0";
                                    auto *tmp_if_1 = cast<TernaryInstr>(arg1_const);                                    
                                    auto *m_call_1 = cast<CallInstr>(tmp_if_1->getCond());
                                    auto *op_1_call = util::getFunc(m_call_1->getCallee());
                                    auto op_1_call_name = op_1_call->getUnmangledName();  
                                    //auto *match_arg_1 = (cast<CallInstr>(m_call_1->front())->front());
                                    //auto *match_arg_2 = cast<CallInstr>(m_call_1->back())->front();
                                    auto *true_return_1 = tmp_if_1->getTrueValue();
                                    auto *false_return_1 = tmp_if_1->getFalseValue();                                                      
                                    std::string arg1_00 = "";
                                    std::string arg1_01 = "";
                                    std::string arg1_02 = "0";
                                    std::string arg1_03 = "";
                                    std::string arg1_04 = "";
                                    std::string arg1_05 = "";
                                    arg1_00 = "0";  
                                                             
                                    if(op_1_call_name == "__eq__"){
                                        std::ostringstream oss_a_arg;  
                                        oss_a_arg << *true_return_1;
                                        std::string oss_a_arg_str = oss_a_arg.str();                             
                                        arg1_01 += oss_a_arg_str;   

                                        std::ostringstream oss_b_arg;  
                                        oss_b_arg << *false_return_1;
                                        std::string oss_b_arg_str = oss_b_arg.str();                             
                                        arg1_03 += oss_b_arg_str;                                                                                                                 
                                    
                                    }                                
                                    else{
                                        std::ostringstream oss_a_arg;  
                                        oss_a_arg << *true_return_1;
                                        std::string oss_a_arg_str = oss_a_arg.str();                             
                                        arg1_03 += oss_a_arg_str;   

                                        std::ostringstream oss_b_arg;  
                                        oss_b_arg << *false_return_1;
                                        std::string oss_b_arg_str = oss_b_arg.str();                             
                                        arg1_01 += oss_b_arg_str;                               
                                    }  
                                    auto *arg_1_1_index = cast<CallInstr>(m_call_1->front())->back();
                                    if(cast<CallInstr>(arg_1_1_index) != NULL){
                                        auto *arg_1_1_f = cast<CallInstr>(arg_1_1_index);
                                        auto *arg_1_1_index_func = util::getFunc(arg_1_1_f->getCallee());
                                        auto arg_1_1_index_func_name = arg_1_1_index_func->getUnmangledName();                                    
                                        if(arg_1_1_index_func_name == "__sub__"){
                                            arg1_04 += "-";
                                        }
                                        auto *tst_1 = arg_1_1_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg1_04 += oss_tst_1_str;                                         
                                    }
                                    else{
                                        arg1_04 = "0";
                                    }
                                    auto *arg_1_2_index = cast<CallInstr>(m_call_1->back())->back();
                                    if(cast<CallInstr>(arg_1_2_index) != NULL){
                                        auto *arg_1_2_f = cast<CallInstr>(arg_1_2_index);
                                        auto *arg_1_2_index_func = util::getFunc(arg_1_2_f->getCallee());
                                        auto arg_1_2_index_func_name = arg_1_2_index_func->getUnmangledName();                                    
                                        if(arg_1_2_index_func_name == "__sub__"){
                                            arg1_05 += "-";
                                        }
                                        auto *tst_1 = arg_1_2_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg1_05 += oss_tst_1_str;                                        
                                    }
                                    else{
                                        arg1_05 = "0";
                                    }                                       
                                    std::map<std::string, std::string> arg1_attributes{{"arg1_00", arg1_00}, {"arg1_01", arg1_01}, {"arg1_02", arg1_02}, {"arg1_03", arg1_03}, {"arg1_04", arg1_04}, {"arg1_05", arg1_05}}; 
                                    globalAttributes["arg1"] = arg1_attributes;                             
                                }
                            }
                            else{
                                params_17 = "0";                     
                                auto *m_call_1 = cast<CallInstr>(arg1_const);
                                auto *m_call_1_func = util::getFunc(m_call_1->getCallee());
                                auto m_call_1_name = m_call_1_func->getUnmangledName();  
                                auto match_att_1 = util::hasAttribute(m_call_1_func, "std.vectron.dispatcher.vectron_cmp");                                                      
                                if (match_att_1){                          
                                    std::vector<codon::ir::Value *> body_1 = m_call_1_func->getUsedValues();
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
                                    std::vector<codon::ir::Var *> elif_arg_1 = elif_cond_1_call->front()->getUsedVariables();
                                    std::vector<codon::ir::Var *> elif_arg_2 = elif_cond_1_call->back()->getUsedVariables();
                                    auto elif_arg_1_name = elif_arg_1[0]->getName();
                                    auto elif_arg_2_name = elif_arg_2[0]->getName();
                                    std::string arg1_00 = "";
                                    std::string arg1_01 = "";
                                    std::string arg1_02 = "";
                                    std::string arg1_03 = "";
                                    std::string arg1_04 = "";
                                    std::string arg1_05 = "";
                                    arg1_00 = "0";                               
                                    std::vector<codon::ir::Value *> all_args_1 = m_call_1->getUsedValues();
                                    auto *a_arg = all_args_1[1];                                
                                    auto *b_arg = all_args_1[2];
                                    auto *am_arg = all_args_1[3];        
                                    if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                        auto *return_val_1_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_1)->back())->getValue();
                                        auto *return_val_1_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_1)->back())->getValue();
                                        auto *return_val_1_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_1)->back())->getValue(); 
                                        std::vector<codon::ir::Var *> ret_val_1 = return_val_1_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_2 = return_val_1_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_3 = return_val_1_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_01 += oss_a_arg_str;                                      
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_01 += oss_b_arg_str;                                                                                      
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_01 += oss_am_arg_str;                                                                                                  
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_02 += oss_a_arg_str;                                                                            
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_02 += oss_b_arg_str;                                                                                          
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_02 += oss_am_arg_str;                                                                                                 
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_03 += oss_a_arg_str;                                                                            
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_03 += oss_b_arg_str;                                                                                            
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_03 += oss_am_arg_str;                                                                                              
                                                }
                                            }
                                        }                                    
                                    
                                    }                                
                                    else{
                                        auto *return_val_1_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_1)->back())->getValue();
                                        auto *return_val_1_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_1)->back())->getValue();
                                        auto *return_val_1_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_1)->back())->getValue();  
                                        std::vector<codon::ir::Var *> ret_val_1 = return_val_1_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_2 = return_val_1_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_3 = return_val_1_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_01 += oss_a_arg_str;                                                                                             
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_01 += oss_b_arg_str;                                                                                                             
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_01 += oss_am_arg_str;                                                                                                  
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_02 += oss_a_arg_str;                                    
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_02 += oss_b_arg_str;                                                                                        
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_02 += oss_am_arg_str;                                                                                                  
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg1_03 += oss_a_arg_str;                                             
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg1_03 += oss_b_arg_str;                                                                                           
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg1_03 += oss_am_arg_str;                                                                                                       
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
                                            arg1_04 += "-";
                                        }
                                        auto *tst_1 = arg_1_1_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg1_04 += oss_tst_1_str;                                         
                                    }
                                    else{
                                        arg1_04 = "0";
                                    }
                                    auto *arg_1_2_index = cast<CallInstr>(m_call_1->back())->back();
                                    if(cast<CallInstr>(arg_1_2_index) != NULL){
                                        auto *arg_1_2_f = cast<CallInstr>(arg_1_2_index);
                                        auto *arg_1_2_index_func = util::getFunc(arg_1_2_f->getCallee());
                                        auto arg_1_2_index_func_name = arg_1_2_index_func->getUnmangledName();                                    
                                        if(arg_1_2_index_func_name == "__sub__"){
                                            arg1_05 += "-";
                                        }
                                        auto *tst_1 = arg_1_2_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg1_05 += oss_tst_1_str;                                        
                                    }
                                    else{
                                        arg1_05 = "0";
                                    }
                                    std::map<std::string, std::string> arg1_attributes{{"arg1_00", arg1_00}, {"arg1_01", arg1_01}, {"arg1_02", arg1_02}, {"arg1_03", arg1_03}, {"arg1_04", arg1_04}, {"arg1_05", arg1_05}}; 
                                    globalAttributes["arg1"] = arg1_attributes;    
                         
                                }
                                
                            }
                        }
                        else{
                            params_17 = "0";
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
                        params_12 = "1";   
                        std::string arg1_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg1_row_instr != NULL){
                            auto *arg1_row_func = util::getFunc(arg1_row_instr->getCallee());
                            auto arg1_row_func_name = arg1_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row_instr->front()->getUsedVariables();
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                params_13 = "0";
                            }
                            else{
                                params_13 = "1";
                            }
                            if (arg1_row_func_name == "__sub__"){
                                params_14 += "-";
                            }            
                            auto *arg1_num_row = arg1_row_instr->back();
                            std::ostringstream oss_arg1_num_row;  
                            oss_arg1_num_row << *arg1_num_row;
                            std::string oss_arg1_num_row_str = oss_arg1_num_row.str();                             
                            params_14 += oss_arg1_num_row_str;                              
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_row_var = arg1_row->getUsedVariables();            
                            arg1_row_var_name = arg1_row_var[0]->getName();
                            if (var_str_outer == arg1_row_var_name){
                                params_13 = "0";
                            }
                            else{
                                params_13 = "1";
                            }
                            params_14 = "0";
                        }
                        std::string arg1_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg1_col_instr != NULL){
                            auto *arg1_col_func = util::getFunc(arg1_col_instr->getCallee());
                            auto arg1_col_func_name = arg1_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col_instr->front()->getUsedVariables();
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                params_15 = "0";
                            }
                            else{
                                params_15 = "1";
                            }
                            if (arg1_col_func_name == "__sub__"){
                                params_16 += "-";
                            }            
                            auto *arg1_num_col = arg1_col_instr->back();
                            std::ostringstream oss_arg1_num_col;  
                            oss_arg1_num_col << *arg1_num_col;
                            std::string oss_arg1_num_col_str = oss_arg1_num_col.str();                             
                            params_16 += oss_arg1_num_col_str;                                
                        }
                        else{
                            std::vector<codon::ir::Var *> arg1_col_var = arg1_col->getUsedVariables();            
                            arg1_col_var_name = arg1_col_var[0]->getName();
                            if (var_str_outer == arg1_col_var_name){
                                params_15 = "0";
                            }
                            else{
                                params_15 = "1";
                            }
                            params_16 = "0";
                            params_17 = "0";
                        } ///// might have a problem with non-existing params_17
                    }
                }
            }
            if (arg2_inst == NULL){
                params_18 = "0";
                params_19 = "0";
                params_20 = "0";
                params_21 = "0";
                params_22 = "0";
                std::ostringstream oss_arg1_r_mid_1;  
                oss_arg1_r_mid_1 << *r_mid[1];
                std::string oss_arg1_r_mid_1_str = oss_arg1_r_mid_1.str();                             
                params_23 = oss_arg1_r_mid_1_str;                 
                arg2_flag = 1;
            }
            else{
                auto arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();
                auto max_st_5 = util::getFunc(arg2_inst->getCallee());
                auto max_att_5 = util::hasAttribute(max_st_5, "std.vectron.dispatcher.vectron_max");                                                                      
                if(max_att_5){
                    params_18 = "0";
                    params_19 = "0";
                    params_20 = "0";
                    params_21 = "0";
                    params_22 = "0";     
                    params_23 = "0";                     

                    std::string mx2_00 = "";
                    std::string mx2_01 = "";
                    std::string mx2_02 = "";
                    std::string mx2_03 = "";
                    std::string mx2_04 = "";
                    std::string mx2_05 = "";
                    std::string mx2_06 = "";
                    std::string mx2_07 = ""; 
                    std::string mx2_08 = ""; 
                    std::string mx2_09 = ""; 
                    std::string mx2_10 = ""; 
                    std::vector<codon::ir::Value *> mx_arg_2 = arg2_inst->getUsedValues();
                    std::vector<codon::ir::Var *> mx_arg_2_1 = mx_arg_2[0]->getUsedVariables();
                    auto mx_arg_2_1_name = mx_arg_2_1[0]->getName();
                    mx2_00 = mx_arg_2_1_name;
                    auto *mx_arg_2_2 = cast<CallInstr>(mx_arg_2[1]);
                    if(mx_arg_2_2 == NULL){
                        mx2_01 = "0";
                    }
                    else{
                        auto mx_arg_2_2_func = util::getFunc(mx_arg_2_2->getCallee())->getUnmangledName();
                        if(mx_arg_2_2_func == "__sub__"){
                            mx2_01 += "-";
                        }
                        auto *mx_arg_2_2_add = mx_arg_2_2->back();
                        std::ostringstream oss_mx_arg_2_2_add;  
                        oss_mx_arg_2_2_add << *mx_arg_2_2_add;
                        std::string oss_mx_arg_2_2_add_str = oss_mx_arg_2_2_add.str();                             
                        mx2_01 += oss_mx_arg_2_2_add_str;     
                    }
                    auto *mx_arg_2_3 = cast<CallInstr>(mx_arg_2[2]);
                    if(mx_arg_2_3 == NULL){
                        mx2_02 = "0";
                    }
                    else{
                        auto mx_arg_2_3_func = util::getFunc(mx_arg_2_3->getCallee())->getUnmangledName();
                        if(mx_arg_2_3_func == "__sub__"){
                            mx2_02 += "-";
                        }
                        auto *mx_arg_2_3_add = mx_arg_2_3->back();
                        std::ostringstream oss_mx_arg_2_3_add;  
                        oss_mx_arg_2_3_add << *mx_arg_2_3_add;
                        std::string oss_mx_arg_2_3_add_str = oss_mx_arg_2_3_add.str();                             
                        mx2_02 += oss_mx_arg_2_3_add_str;     
                    }                    
                    auto *mx_arg_2_4 = cast<CallInstr>(mx_arg_2[3]);       
                    auto mx_arg_2_4_name = util::getFunc(cast<CallInstr>(mx_arg_2_4)->getCallee())->getUnmangledName();
                    if(mx_arg_2_4_name == "__add__" || mx_arg_2_4_name == "__sub__"){
                        auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_4_left_part_1 = cast<CallInstr>(mx_arg_2_4_left)->front();
                        auto *mx_arg_2_4_left_part_1_1 = cast<CallInstr>(mx_arg_2_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx2_03 = part_1_lst_name;
                        auto *mx_arg_2_4_left_part_1_2 = cast<CallInstr>(mx_arg_2_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_left_part_1_2) == NULL){
                            mx2_04 = "0";
                        }
                        else{
                            auto mx_arg_2_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_4_left_part_1_2)->getCallee());
                            auto mx_arg_2_4_inst = mx_arg_2_4_left_inst->getUnmangledName();
                            if(mx_arg_2_4_inst == "__sub__"){
                                mx2_04 += "-";
                            }                            
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_4_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx2_04 += oss_part_1_add_str;                                 
                        }
                        auto *mx_arg_2_4_part_2 = cast<CallInstr>(mx_arg_2_4_left)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_part_2) == NULL){
                            mx2_05 = "0";
                        }
                        else{
                            auto *mx_arg_2_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_4_part_2)->getCallee());
                            auto mx_arg_2_4_part_2_inst = mx_arg_2_4_left_part_2->getUnmangledName();;
                            if(mx_arg_2_4_part_2_inst == "__sub__"){
                                mx2_05 += "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_2_4_part_2)->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx2_05 += oss_part_2_add_str;                                     
                        }
                        if(mx_arg_2_4_name == "__sub__"){
                            mx2_06 += "-";
                        }
                        auto *mx_arg_2_4_right = mx_arg_2_4->back(); // final value 
                        std::ostringstream oss_mx_arg_2_4_right;  
                        oss_mx_arg_2_4_right << *mx_arg_2_4_right;
                        std::string oss_mx_arg_2_4_right_str = oss_mx_arg_2_4_right.str();                             
                        mx2_06 += oss_mx_arg_2_4_right_str;                                           
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_4_left_part_1 = cast<CallInstr>(mx_arg_2_4)->front();
                        auto *mx_arg_2_4_left_part_1_1 = cast<CallInstr>(mx_arg_2_4_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_4_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx2_03 = part_1_lst_name;
                        auto *mx_arg_2_4_left_part_1_2 = cast<CallInstr>(mx_arg_2_4_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_left_part_1_2) == NULL){
                            mx2_04 = "0";
                        }
                        else{
                            auto mx_arg_2_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_4_left_part_1_2)->getCallee());
                            auto mx_arg_2_4_inst = mx_arg_2_4_left_inst->getUnmangledName();
                            if(mx_arg_2_4_inst == "__sub__"){
                                mx2_04 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_4_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx2_04 += oss_part_1_add_str;                                
                        }
                        auto *mx_arg_2_4_part_2 = cast<CallInstr>(mx_arg_2_4)->back();                        
                        if(cast<CallInstr>(mx_arg_2_4_part_2) == NULL){
                            mx2_05 = "0";
                        }
                        else{
                            auto *mx_arg_2_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_4_part_2)->getCallee());
                            auto mx_arg_2_4_part_2_inst = mx_arg_2_4_left_part_2->getUnmangledName();;
                            if(mx_arg_2_4_part_2_inst == "__sub__"){
                                mx2_05 += "-";
                            }
                            auto *part_2_add = mx_arg_2_4->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx2_05 += oss_part_2_add_str;                                  
                        }  
                        mx2_06 = "0";               
                    }

                    auto *mx_arg_2_5 = cast<CallInstr>(mx_arg_2[4]);       
                    auto mx_arg_2_5_name = util::getFunc(cast<CallInstr>(mx_arg_2_5)->getCallee())->getUnmangledName();
                    if(mx_arg_2_5_name == "__add__" || mx_arg_2_5_name == "__sub__"){
                        auto *mx_arg_2_5_left = mx_arg_2_5->front();
                        auto *mx_arg_2_5_left_part_1 = cast<CallInstr>(mx_arg_2_5_left)->front();
                        auto *mx_arg_2_5_left_part_1_1 = cast<CallInstr>(mx_arg_2_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx2_07 = part_1_lst_name;
                        auto *mx_arg_2_5_left_part_1_2 = cast<CallInstr>(mx_arg_2_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_left_part_1_2) == NULL){
                            mx2_08 = "0";
                        }
                        else{
                            auto mx_arg_2_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_5_left_part_1_2)->getCallee());
                            auto mx_arg_2_5_inst = mx_arg_2_5_left_inst->getUnmangledName();
                            if(mx_arg_2_5_inst == "__sub__"){
                                mx2_08 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_5_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx2_08 += oss_part_1_add_str;                                 
                        }
                        auto *mx_arg_2_5_part_2 = cast<CallInstr>(mx_arg_2_5_left)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_part_2) == NULL){
                            mx2_09 = "0";
                        }
                        else{
                            auto *mx_arg_2_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_5_part_2)->getCallee());
                            auto mx_arg_2_5_part_2_inst = mx_arg_2_5_left_part_2->getUnmangledName();;
                            if(mx_arg_2_5_part_2_inst == "__sub__"){
                                mx2_09 += "-";
                            }
                            auto *part_2_add = cast<CallInstr>(mx_arg_2_5_part_2)->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx2_09 += oss_part_2_add_str;          
                        }
                        if(mx_arg_2_5_name == "__sub__"){
                            mx2_10 = "-";
                        }
                        auto *mx_arg_2_5_right = mx_arg_2_5->back(); // final value               
                        std::ostringstream oss_mx_arg_2_5_right;  
                        oss_mx_arg_2_5_right << *mx_arg_2_5_right;
                        std::string oss_mx_arg_2_5_right_str = oss_mx_arg_2_5_right.str();                             
                        mx2_10 += oss_mx_arg_2_5_right_str;                              
                    }
                    else{
                        //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                        auto *mx_arg_2_5_left_part_1 = cast<CallInstr>(mx_arg_2_5)->front();
                        auto *mx_arg_2_5_left_part_1_1 = cast<CallInstr>(mx_arg_2_5_left_part_1)->front();
                        std::vector<codon::ir::Var *> part_1_lst = mx_arg_2_5_left_part_1_1->getUsedVariables();
                        auto part_1_lst_name = part_1_lst[0]->getName();
                        mx2_07 = part_1_lst_name;
                        auto *mx_arg_2_5_left_part_1_2 = cast<CallInstr>(mx_arg_2_5_left_part_1)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_left_part_1_2) == NULL){
                            mx2_08 = "0";
                        }
                        else{
                            auto mx_arg_2_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_2_5_left_part_1_2)->getCallee());
                            auto mx_arg_2_5_inst = mx_arg_2_5_left_inst->getUnmangledName();
                            if(mx_arg_2_5_inst == "__sub__"){
                                mx2_08 += "-";
                            }
                            auto *part_1_add = cast<CallInstr>(mx_arg_2_5_left_part_1_2)->back();
                            std::ostringstream oss_part_1_add;  
                            oss_part_1_add << *part_1_add;
                            std::string oss_part_1_add_str = oss_part_1_add.str();                             
                            mx2_08 += oss_part_1_add_str;                                 
                        }
                        auto *mx_arg_2_5_part_2 = cast<CallInstr>(mx_arg_2_5)->back();                        
                        if(cast<CallInstr>(mx_arg_2_5_part_2) == NULL){
                            mx2_09 = "0";
                        }
                        else{
                            auto *mx_arg_2_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_2_5_part_2)->getCallee());
                            auto mx_arg_2_5_part_2_inst = mx_arg_2_5_left_part_2->getUnmangledName();;
                            if(mx_arg_2_5_part_2_inst == "__sub__"){
                                mx2_09 += "-";
                            }
                            auto *part_2_add = mx_arg_2_5->back();
                            std::ostringstream oss_part_2_add;  
                            oss_part_2_add << *part_2_add;
                            std::string oss_part_2_add_str = oss_part_2_add.str();                             
                            mx2_09 += oss_part_2_add_str;                                          
                        }  
                        mx2_10 = "0";         
                    }                    
                    std::map<std::string, std::string> mx2_attributes{{"mx2_00", mx2_00}, {"mx2_01", mx2_01}, {"mx2_02", mx2_02}, {"mx2_03", mx2_03}, {"mx2_04", mx2_04}, {"mx2_05", mx2_05}, {"mx2_06", mx2_06}, {"mx2_07", mx2_07}, {"mx2_08", mx2_08}, {"mx2_09", mx2_09}, {"mx2_10", mx2_10}}; 
                    globalAttributes["mx2"] = mx2_attributes;        
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
                            std::ostringstream oss_arg2_const;  
                            oss_arg2_const << *arg2_const;
                            std::string oss_arg2_const_str = oss_arg2_const.str();                             
                            params_18 += oss_arg2_const_str;                                
                        }
                        else if (arg2_func_name == "__div__"){
                            params_18 += "1/";
                            std::ostringstream oss_arg2_const;  
                            oss_arg2_const << *arg2_const;
                            std::string oss_arg2_const_str = oss_arg2_const.str();                             
                            params_18 += oss_arg2_const_str;                             
                        }
                        else{
                            params_18 = "1";
                        }               
                        std::string arg2_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg2_row_instr != NULL){
                            auto *arg2_row_func = util::getFunc(arg2_row_instr->getCallee());
                            auto arg2_row_func_name = arg2_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row_instr->front()->getUsedVariables();
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                params_19 = "0";
                            }
                            else{
                                params_19 = "1";
                            }
                            if (arg2_row_func_name == "__sub__"){
                                params_20 += "-";
                            }            
                            auto *arg2_num_row = arg2_row_instr->back();
                            std::ostringstream oss_arg2_num_row;  
                            oss_arg2_num_row << *arg2_num_row;
                            std::string oss_arg2_num_row_str = oss_arg2_num_row.str();                             
                            params_20 += oss_arg2_num_row_str;                                                                        
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row->getUsedVariables();            
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                params_19 = "0";
                            }
                            else{
                                params_19 = "1";
                            }
                            params_20 = "0";
                        }
                        std::string arg2_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg2_col_instr != NULL){
                            auto *arg2_num_col = arg2_col_instr->back();
                            auto *arg2_num_col_inst = cast<CallInstr>(arg2_num_col);                            
                            auto *arg2_col_func = util::getFunc(arg2_col_instr->getCallee());
                            auto arg2_col_func_name = arg2_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col_instr->front()->getUsedVariables();
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                params_21 = "0";
                            }
                            else{
                                params_21 = "1";
                            }                            
                            if (arg2_col_func_name == "__sub__"){
                                params_22 += "-";
                            }                             
                            if(arg2_num_col_inst == NULL){  
                                std::ostringstream oss_arg2_num_col;  
                                oss_arg2_num_col << *arg2_num_col;
                                std::string oss_arg2_num_col_str = oss_arg2_num_col.str();                             
                                params_22 += oss_arg2_num_col_str;                                                                                    
                            }
                            else{
                                auto *arg_2_num_col_left = arg2_num_col_inst->front();
                                auto *arg_2_num_col_right = arg2_num_col_inst->back();
                                std::vector<codon::ir::Var *> arg_2_num_col_left_vars = arg_2_num_col_left->getUsedVariables();
                                auto arg_2_num_col_left_name = arg_2_num_col_left_vars[0]->getName();
                                if(arg_2_num_col_left_name == pf_arg1){
                                    params_22 += "0 ";
                                }
                                else{
                                    params_22 += "1 ";
                                }
                                auto *arg_2_num_col_right_inst = cast<CallInstr>(arg_2_num_col_right);
                                if(arg_2_num_col_right_inst == NULL){
                                    std::vector<codon::ir::Var *> arg_2_num_col_right_vars = arg_2_num_col_right->getUsedVariables();
                                    auto arg_2_num_col_right_name = arg_2_num_col_right_vars[0]->getName();
                                    if(arg_2_num_col_right_name == var_str_inner){
                                        params_22 += "0 0";
                                    }
                                    else{
                                        params_22 += "1 0";
                                    }
                                }
                                else{
                                    auto *arg2_num_col_right_left = arg_2_num_col_right_inst->front();
                                    std::vector<codon::ir::Var *> arg2_num_col_right_left_vars =  arg2_num_col_right_left->getUsedVariables();
                                    auto arg2_num_col_right_left_name = arg2_num_col_right_left_vars[0]->getName();
                                    if(arg2_num_col_right_left_name == var_str_inner){
                                        params_22 += "0 ";
                                    }
                                    else{
                                        params_22 += "1 ";
                                    }
                                    auto *arg2_num_col_right_right = arg_2_num_col_right_inst->back();
                                    auto *arg2_num_col_right_func = util::getFunc(arg_2_num_col_right_inst->getCallee());
                                    auto arg2_num_col_right_func_name = arg2_num_col_right_func->getUnmangledName();
                                    if(arg2_num_col_right_func_name == "__sub__"){
                                        params_22 += "-";
                                    }
                                    std::ostringstream oss_arg2_num_col_right_right;  
                                    oss_arg2_num_col_right_right << *arg2_num_col_right_right;
                                    std::string oss_arg2_num_col_right_right_str = oss_arg2_num_col_right_right.str();                             
                                    params_22 += oss_arg2_num_col_right_right_str;  
                                }
                            }          
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col->getUsedVariables();            
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                params_21 = "0";
                            }
                            else{
                                params_21 = "1";
                            }
                            params_22 = "0";
                        }
                        if (arg2_func_name == "__sub__"){
                            params_23 += "-";
                            std::ostringstream oss_arg2_const;  
                            oss_arg2_const << *arg2_const;
                            std::string oss_arg2_const_str = oss_arg2_const.str();                             
                            params_23 += oss_arg2_const_str;                              
                        }
                        else if (arg2_func_name == "__add__"){
                            if (cast<CallInstr>(arg2_const) == NULL){
                                if(cast<TernaryInstr>(arg2_const) == NULL){
                                    std::ostringstream oss_arg2_const;  
                                    oss_arg2_const << *arg2_const;
                                    std::string oss_arg2_const_str = oss_arg2_const.str();                             
                                    params_23 += oss_arg2_const_str;           
                                }
                                else{
                                    params_23 = "0";
                                    auto *tmp_if_2 = cast<TernaryInstr>(arg2_const); 
                                    auto *m_call_2 = cast<CallInstr>(tmp_if_2->getCond());
                                    auto *op_2_call = util::getFunc(m_call_2->getCallee());
                                    auto op_2_call_name = op_2_call->getUnmangledName();  
                                    //auto *match_arg_1 = (cast<CallInstr>(m_call_2->front())->front());
                                    //auto *match_arg_2 = cast<CallInstr>(m_call_2->back())->front();
                                    auto *true_return_2 = tmp_if_2->getTrueValue();
                                    auto *false_return_2 = tmp_if_2->getFalseValue();                                                      
                                    std::string arg2_00 = "";
                                    std::string arg2_01 = "";
                                    std::string arg2_02 = "0";
                                    std::string arg2_03 = "";
                                    std::string arg2_04 = "";
                                    std::string arg2_05 = "";
                                    arg2_00 = "0";  
                                                             
                                    if(op_2_call_name == "__eq__"){
                                        std::ostringstream oss_a_arg;  
                                        oss_a_arg << *true_return_2;
                                        std::string oss_a_arg_str = oss_a_arg.str();                             
                                        arg2_01 += oss_a_arg_str;   

                                        std::ostringstream oss_b_arg;  
                                        oss_b_arg << *false_return_2;
                                        std::string oss_b_arg_str = oss_b_arg.str();                             
                                        arg2_03 += oss_b_arg_str;                                                                                                                 
                                    
                                    }                                
                                    else{
                                        std::ostringstream oss_a_arg;  
                                        oss_a_arg << *true_return_2;
                                        std::string oss_a_arg_str = oss_a_arg.str();                             
                                        arg2_03 += oss_a_arg_str;   

                                        std::ostringstream oss_b_arg;  
                                        oss_b_arg << *false_return_2;
                                        std::string oss_b_arg_str = oss_b_arg.str();                             
                                        arg2_01 += oss_b_arg_str;                               
                                    }  
                                    auto *arg_2_1_index = cast<CallInstr>(m_call_2->front())->back();
                                    if(cast<CallInstr>(arg_2_1_index) != NULL){
                                        auto *arg_2_1_f = cast<CallInstr>(arg_2_1_index);
                                        auto *arg_2_1_index_func = util::getFunc(arg_2_1_f->getCallee());
                                        auto arg_2_1_index_func_name = arg_2_1_index_func->getUnmangledName();                                    
                                        if(arg_2_1_index_func_name == "__sub__"){
                                            arg2_04 += "-";
                                        }
                                        auto *tst_2 = arg_2_1_f->back(); 
                                        std::ostringstream oss_tst_2;  
                                        oss_tst_2 << *tst_2;
                                        std::string oss_tst_2_str = oss_tst_2.str();                             
                                        arg2_04 += oss_tst_2_str;                                         
                                    }
                                    else{
                                        arg2_04 = "0";
                                    }
                                    auto *arg_2_2_index = cast<CallInstr>(m_call_2->back())->back();
                                    if(cast<CallInstr>(arg_2_2_index) != NULL){
                                        auto *arg_2_2_f = cast<CallInstr>(arg_2_2_index);
                                        auto *arg_2_2_index_func = util::getFunc(arg_2_2_f->getCallee());
                                        auto arg_2_2_index_func_name = arg_2_2_index_func->getUnmangledName();                                    
                                        if(arg_2_2_index_func_name == "__sub__"){
                                            arg2_05 += "-";
                                        }
                                        auto *tst_2 = arg_2_2_f->back(); 
                                        std::ostringstream oss_tst_2;  
                                        oss_tst_2 << *tst_2;
                                        std::string oss_tst_2_str = oss_tst_2.str();                             
                                        arg2_05 += oss_tst_2_str;                                        
                                    }
                                    else{
                                        arg2_05 = "0";
                                    }                                       
                                    std::map<std::string, std::string> arg2_attributes{{"arg2_00", arg2_00}, {"arg2_01", arg2_01}, {"arg2_02", arg2_02}, {"arg2_03", arg2_03}, {"arg2_04", arg2_04}, {"arg2_05", arg2_05}}; 
                                    globalAttributes["arg2"] = arg2_attributes;    
                                }                                                                
                            }
                            else{
                                params_23 = "0";                               
                                auto *m_call_2 = cast<CallInstr>(arg2_const);
                                auto *m_call_2_func = util::getFunc(m_call_2->getCallee());
                                auto m_call_2_name = m_call_2_func->getUnmangledName();    
                                auto match_att_2 = util::hasAttribute(m_call_2_func, "std.vectron.dispatcher.vectron_cmp");                                                        
                                if (match_att_2){
                                    std::vector<codon::ir::Value *> body_2 = m_call_2_func->getUsedValues();
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
                                    std::vector<codon::ir::Var *> elif_arg_1 = elif_cond_2_call->front()->getUsedVariables();
                                    std::vector<codon::ir::Var *> elif_arg_2 = elif_cond_2_call->back()->getUsedVariables();
                                    auto elif_arg_1_name = elif_arg_1[0]->getName();
                                    auto elif_arg_2_name = elif_arg_2[0]->getName();
                                    std::string arg2_00 = "";
                                    std::string arg2_01 = "";
                                    std::string arg2_02 = "";
                                    std::string arg2_03 = "";
                                    std::string arg2_04 = "";
                                    std::string arg2_05 = "";                                    
                                    arg2_00 = "0";
                                    std::vector<codon::ir::Value *> all_args_2 = m_call_2->getUsedValues();
                                    auto *a_arg = all_args_2[1];                                
                                    auto *b_arg = all_args_2[2];
                                    auto *am_arg = all_args_2[3];                                                                    
                                    if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                        auto *return_val_2_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_2)->back())->getValue();
                                        auto *return_val_2_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_2)->back())->getValue();
                                        auto *return_val_2_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_2)->back())->getValue();
                                        std::vector<codon::ir::Var *> ret_val_1 = return_val_2_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_2 = return_val_2_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_3 = return_val_2_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_01 += oss_a_arg_str;                                                                               
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_01 += oss_b_arg_str;                                                                                                                                      
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_01 += oss_am_arg_str;                                                                                                                                             
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_02 += oss_a_arg_str;                                                                                 
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_02 += oss_b_arg_str;                                                                                     
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_02 += oss_am_arg_str;                                                                                                     
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_03 += oss_a_arg_str;                                                                                   
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_03 += oss_b_arg_str;                                                                                          
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_03 += oss_am_arg_str;                                                                                                   
                                                }
                                            }
                                        }                                     
                                    }                                
                                    else{
                                        auto *return_val_2_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_2)->back())->getValue();
                                        auto *return_val_2_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_2)->back())->getValue();
                                        auto *return_val_2_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_2)->back())->getValue();                                    
                                        std::vector<codon::ir::Var *> ret_val_1 = return_val_2_1->getUsedVariables();
                                        auto ret_val_1_name = ret_val_1[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_2 = return_val_2_2->getUsedVariables();
                                        auto ret_val_2_name = ret_val_2[0]->getName();
                                        std::vector<codon::ir::Var *> ret_val_3 = return_val_2_3->getUsedVariables();
                                        auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                        if(ret_val_1_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_01 += oss_a_arg_str;                                                                                   
                                        }
                                        else{
                                            if(ret_val_1_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_01 += oss_b_arg_str;                                                                                         
                                            }
                                            else{
                                                if(ret_val_1_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_01 += oss_am_arg_str;                                                                                                    
                                                }
                                            }
                                        }
                                        if(ret_val_2_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_02 += oss_a_arg_str;                                             
                                        }
                                        else{
                                            if(ret_val_2_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_02 += oss_b_arg_str;                                                  
                                            }
                                            else{
                                                if(ret_val_2_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_02 += oss_am_arg_str;                                                       
                                                }
                                            }
                                        }
                                        if(ret_val_3_name == "a"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *a_arg;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg2_03 += oss_a_arg_str;                                                  
                                        }
                                        else{
                                            if(ret_val_3_name == "b"){
                                                std::ostringstream oss_b_arg;  
                                                oss_b_arg << *b_arg;
                                                std::string oss_b_arg_str = oss_b_arg.str();                             
                                                arg2_03 += oss_b_arg_str;                                                    
                                            }
                                            else{
                                                if(ret_val_3_name == "am"){
                                                    std::ostringstream oss_am_arg;  
                                                    oss_am_arg << *am_arg;
                                                    std::string oss_am_arg_str = oss_am_arg.str();                             
                                                    arg2_03 += oss_am_arg_str;                                                        
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
                                            arg2_04 += "-";
                                        }
                                        auto *tst_1 = arg_2_1_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg2_04 += oss_tst_1_str;                                            
                                    }
                                    else{
                                        arg2_04 = "0";
                                    }
                                    auto *arg_2_2_index = cast<CallInstr>(m_call_2->back())->back();
                                    if(cast<CallInstr>(arg_2_2_index) != NULL){
                                        auto *arg_2_2_f = cast<CallInstr>(arg_2_2_index);
                                        auto *arg_2_2_index_func = util::getFunc(arg_2_2_f->getCallee());
                                        auto arg_2_2_index_func_name = arg_2_2_index_func->getUnmangledName();                                    
                                        if(arg_2_2_index_func_name == "__sub__"){
                                            arg2_05 += "-";
                                        }
                                        auto *tst_1 = arg_2_2_f->back(); 
                                        std::ostringstream oss_tst_1;  
                                        oss_tst_1 << *tst_1;
                                        std::string oss_tst_1_str = oss_tst_1.str();                             
                                        arg2_05 += oss_tst_1_str;                                              
                                    }
                                    else{
                                        arg2_05 = "0";
                                    }
                                    std::map<std::string, std::string> arg2_attributes{{"arg2_00", arg2_00}, {"arg2_01", arg2_01}, {"arg2_02", arg2_02}, {"arg2_03", arg2_03}, {"arg2_04", arg2_04}, {"arg2_05", arg2_05}}; 
                                    globalAttributes["arg2"] = arg2_attributes;                                      

                                }
                                
                            }
                        }
                        else{
                            params_23 = "0";
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
                        params_18 = "1";
                        std::string arg2_row_var_name = ""; // taking care of the row index of the first (left side) operand
                        if (arg2_row_instr != NULL){
                            auto *arg2_row_func = util::getFunc(arg2_row_instr->getCallee());
                            auto arg2_row_func_name = arg2_row_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row_instr->front()->getUsedVariables();
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                params_19 = "0";
                            }
                            else{
                                params_19 = "1";
                            }
                            if (arg2_row_func_name == "__sub__"){
                                params_20 += "-";
                            }            
                            auto *arg2_num_row = arg2_row_instr->back();
                            std::ostringstream oss_arg2_num_row;  
                            oss_arg2_num_row << *arg2_num_row;
                            std::string oss_arg2_num_row_str = oss_arg2_num_row.str();                             
                            params_20 += oss_arg2_num_row_str;                                     
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_row_var = arg2_row->getUsedVariables();            
                            arg2_row_var_name = arg2_row_var[0]->getName();
                            if (var_str_outer == arg2_row_var_name){
                                params_19 = "0";
                            }
                            else{
                                params_19 = "1";
                            }
                            params_20 = "0";
                        }
                        std::string arg2_col_var_name = ""; // taking care of the column index of the first (left side) operand
                        if (arg2_col_instr != NULL){
                            auto *arg2_col_func = util::getFunc(arg2_col_instr->getCallee());
                            auto arg2_col_func_name = arg2_col_func->getUnmangledName();
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col_instr->front()->getUsedVariables();
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                params_21 = "0";
                            }
                            else{
                                params_21 = "1";
                            }
                            if (arg2_col_func_name == "__sub__"){
                                params_22 += "-";
                            }            
                            auto *arg2_num_col = arg2_col_instr->back();
                            std::ostringstream oss_arg2_num_col;  
                            oss_arg2_num_col << *arg2_num_col;
                            std::string oss_arg2_num_col_str = oss_arg2_num_col.str();                             
                            params_22 += oss_arg2_num_col_str; 
                            params_23 = "0";                                   
                        }
                        else{
                            std::vector<codon::ir::Var *> arg2_col_var = arg2_col->getUsedVariables();            
                            arg2_col_var_name = arg2_col_var[0]->getName();
                            if (var_str_outer == arg2_col_var_name){
                                params_21 = "0";
                            }
                            else{
                                params_21 = "1";
                            }
                            params_22 = "0";
                            params_23 = "0";
                        }
                    }
                }
            }
            if (main_args != 0){
                auto *arg3 = cast<CallInstr>(right_side->front()); // the left side operand                
                auto *arg3_inst = cast<CallInstr>(arg3->back());
                if (arg3_inst == NULL){
                    params_24 = "0";
                    params_25 = "0";
                    params_26 = "0";
                    params_27 = "0";
                    params_28 = "0";
                    std::ostringstream oss_r_end;  
                    oss_r_end << *r_end;
                    std::string oss_r_end_str = oss_r_end.str();                             
                    params_29 += oss_r_end_str;                      
                    arg3_flag = 1;
                }
                else{
                    auto arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();
                    auto *max_st_6 = util::getFunc(arg3_inst->getCallee());
                    auto max_att_6 = util::hasAttribute(max_st_6, "std.vectron.dispatcher.vectron_max");                    
                    if(max_att_6){
                        params_24 = "0";
                        params_25 = "0";
                        params_26 = "0";
                        params_27 = "0";
                        params_28 = "0";
                        params_29 = "0";                        

                        std::string mx3_00 = "";
                        std::string mx3_01 = "";
                        std::string mx3_02 = "";
                        std::string mx3_03 = "";
                        std::string mx3_04 = "";
                        std::string mx3_05 = "";
                        std::string mx3_06 = "";
                        std::string mx3_07 = ""; 
                        std::string mx3_08 = ""; 
                        std::string mx3_09 = ""; 
                        std::string mx3_10 = "";                        
                        std::vector<codon::ir::Value *> mx_arg_3 = arg3_inst->getUsedValues();
                        std::vector<codon::ir::Var *> mx_arg_3_1 = mx_arg_3[0]->getUsedVariables();
                        auto mx_arg_3_1_name = mx_arg_3_1[0]->getName();
                        mx3_00 = mx_arg_3_1_name;
                        auto *mx_arg_3_2 = cast<CallInstr>(mx_arg_3[1]);
                        if(mx_arg_3_2 == NULL){
                            mx3_01 = "0";
                        }
                        else{
                            auto mx_arg_3_2_func = util::getFunc(mx_arg_3_2->getCallee())->getUnmangledName();
                            if(mx_arg_3_2_func == "__sub__"){
                                mx3_01 += "-";
                            }                            
                            auto *mx_arg_3_2_add = mx_arg_3_2->back();
                            std::ostringstream oss_mx_arg_3_2_add;  
                            oss_mx_arg_3_2_add << *mx_arg_3_2_add;
                            std::string oss_mx_arg_3_2_add_str = oss_mx_arg_3_2_add.str();                             
                            mx3_01 += oss_mx_arg_3_2_add_str;                                     
                        }
                        auto *mx_arg_3_3 = cast<CallInstr>(mx_arg_3[2]);
                        if(mx_arg_3_3 == NULL){
                            mx3_02 = "0";
                        }
                        else{
                            auto mx_arg_3_3_func = util::getFunc(mx_arg_3_3->getCallee())->getUnmangledName();
                            if(mx_arg_3_3_func == "__sub__"){
                                mx3_02 += "-";
                            }
                            auto *mx_arg_3_3_add = mx_arg_3_3->back();
                            std::ostringstream oss_mx_arg_3_3_add;  
                            oss_mx_arg_3_3_add << *mx_arg_3_3_add;
                            std::string oss_mx_arg_3_3_add_str = oss_mx_arg_3_3_add.str();                             
                            mx3_02 += oss_mx_arg_3_3_add_str;                                 
                        }                    
                        auto *mx_arg_3_4 = cast<CallInstr>(mx_arg_3[3]);       
                        auto mx_arg_3_4_name = util::getFunc(cast<CallInstr>(mx_arg_3_4)->getCallee())->getUnmangledName();
                        if(mx_arg_3_4_name == "__add__" || mx_arg_3_4_name == "__sub__"){
                            auto *mx_arg_3_4_left = mx_arg_3_4->front();
                            auto *mx_arg_3_4_left_part_1 = cast<CallInstr>(mx_arg_3_4_left)->front();
                            auto *mx_arg_3_4_left_part_1_1 = cast<CallInstr>(mx_arg_3_4_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_4_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx3_03 = part_1_lst_name;
                            auto *mx_arg_3_4_left_part_1_2 = cast<CallInstr>(mx_arg_3_4_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_left_part_1_2) == NULL){
                                mx3_04 = "0";
                            }
                            else{
                                auto mx_arg_3_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_4_left_part_1_2)->getCallee());
                                auto mx_arg_3_4_inst = mx_arg_3_4_left_inst->getUnmangledName();
                                if(mx_arg_3_4_inst == "__sub__"){
                                    mx3_04 += "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_4_left_part_1_2)->back();
                                std::ostringstream oss_part_1_add;  
                                oss_part_1_add << *part_1_add;
                                std::string oss_part_1_add_str = oss_part_1_add.str();                             
                                mx3_04 += oss_part_1_add_str;                                  
                            }
                            auto *mx_arg_3_4_part_2 = cast<CallInstr>(mx_arg_3_4_left)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_part_2) == NULL){
                                mx3_05 = "0";
                            }
                            else{
                                auto *mx_arg_3_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_4_part_2)->getCallee());
                                auto mx_arg_3_4_part_2_inst = mx_arg_3_4_left_part_2->getUnmangledName();;
                                if(mx_arg_3_4_part_2_inst == "__sub__"){
                                    mx3_05 += "-";
                                }
                                auto *part_2_add = cast<CallInstr>(mx_arg_3_4_part_2)->back();
                                std::ostringstream oss_part_2_add;  
                                oss_part_2_add << *part_2_add;
                                std::string oss_part_2_add_str = oss_part_2_add.str();                             
                                mx3_05 += oss_part_2_add_str;                                 
                            }
                            if(mx_arg_3_4_name == "__sub__"){
                                mx3_06 += "-";
                            }
                            auto *mx_arg_3_4_right = mx_arg_3_4->back(); // final value     
                            std::ostringstream oss_mx_arg_3_4_right;  
                            oss_mx_arg_3_4_right << *mx_arg_3_4_right;
                            std::string oss_mx_arg_3_4_right_str = oss_mx_arg_3_4_right.str();                             
                            mx3_06 += oss_mx_arg_3_4_right_str;                                         
                        }
                        else{
                            //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                            auto *mx_arg_3_4_left_part_1 = cast<CallInstr>(mx_arg_3_4)->front();
                            auto *mx_arg_3_4_left_part_1_1 = cast<CallInstr>(mx_arg_3_4_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_4_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx3_03 = part_1_lst_name;
                            auto *mx_arg_3_4_left_part_1_2 = cast<CallInstr>(mx_arg_3_4_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_left_part_1_2) == NULL){
                                mx3_04 = "0";
                            }
                            else{
                                auto mx_arg_3_4_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_4_left_part_1_2)->getCallee());
                                auto mx_arg_3_4_inst = mx_arg_3_4_left_inst->getUnmangledName();
                                if(mx_arg_3_4_inst == "__sub__"){
                                    mx3_04 += "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_4_left_part_1_2)->back();
                                std::ostringstream oss_part_1_add;  
                                oss_part_1_add << *part_1_add;
                                std::string oss_part_1_add_str = oss_part_1_add.str();                             
                                mx3_04 += oss_part_1_add_str;                                   
                            }
                            auto *mx_arg_3_4_part_2 = cast<CallInstr>(mx_arg_3_4)->back();                        
                            if(cast<CallInstr>(mx_arg_3_4_part_2) == NULL){
                                mx3_05 = "0";
                            }
                            else{
                                auto *mx_arg_3_4_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_4_part_2)->getCallee());
                                auto mx_arg_3_4_part_2_inst = mx_arg_3_4_left_part_2->getUnmangledName();;
                                if(mx_arg_3_4_part_2_inst == "__sub__"){
                                    mx3_05 += "-";
                                }
                                auto *part_2_add = mx_arg_3_4->back();
                                std::ostringstream oss_part_2_add;  
                                oss_part_2_add << *part_2_add;
                                std::string oss_part_2_add_str = oss_part_2_add.str();                             
                                mx3_05 += oss_part_2_add_str;                                        
                            }  
                            mx3_06 = "0";
                        }

                        auto *mx_arg_3_5 = cast<CallInstr>(mx_arg_3[4]);       
                        auto mx_arg_3_5_name = util::getFunc(cast<CallInstr>(mx_arg_3_5)->getCallee())->getUnmangledName();
                        if(mx_arg_3_5_name == "__add__" || mx_arg_3_5_name == "__sub__"){
                            auto *mx_arg_3_5_left = mx_arg_3_5->front();
                            auto *mx_arg_3_5_left_part_1 = cast<CallInstr>(mx_arg_3_5_left)->front();
                            auto *mx_arg_3_5_left_part_1_1 = cast<CallInstr>(mx_arg_3_5_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_5_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx3_07 = part_1_lst_name;
                            auto *mx_arg_3_5_left_part_1_2 = cast<CallInstr>(mx_arg_3_5_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_left_part_1_2) == NULL){
                                mx3_08 = "0";
                            }
                            else{
                                auto mx_arg_3_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_5_left_part_1_2)->getCallee());
                                auto mx_arg_3_5_inst = mx_arg_3_5_left_inst->getUnmangledName();
                                if(mx_arg_3_5_inst == "__sub__"){
                                    mx3_08 += "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_5_left_part_1_2)->back();
                                std::ostringstream oss_part_1_add;  
                                oss_part_1_add << *part_1_add;
                                std::string oss_part_1_add_str = oss_part_1_add.str();                             
                                mx3_08 += oss_part_1_add_str;                                   
                            }
                            auto *mx_arg_3_5_part_2 = cast<CallInstr>(mx_arg_3_5_left)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_part_2) == NULL){
                                mx3_09 = "0";
                            }
                            else{
                                auto *mx_arg_3_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_5_part_2)->getCallee());
                                auto mx_arg_3_5_part_2_inst = mx_arg_3_5_left_part_2->getUnmangledName();;
                                if(mx_arg_3_5_part_2_inst == "__sub__"){
                                    mx3_09 += "-";
                                }
                                auto *part_2_add = cast<CallInstr>(mx_arg_3_5_part_2)->back();
                                std::ostringstream oss_part_2_add;  
                                oss_part_2_add << *part_2_add;
                                std::string oss_part_2_add_str = oss_part_2_add.str();                             
                                mx3_09 += oss_part_2_add_str;                                     
                            }
                            if(mx_arg_3_5_name == "__sub__"){
                                mx3_10 += "-";                                
                            }
                            auto *mx_arg_3_5_right = mx_arg_3_5->back(); // final value         
                            std::ostringstream oss_mx_arg_3_5_right;  
                            oss_mx_arg_3_5_right << *mx_arg_3_5_right;
                            std::string oss_mx_arg_3_5_right_str = oss_mx_arg_3_5_right.str();                             
                            mx3_10 += oss_mx_arg_3_5_right_str;                                       
                        }
                        else{
                            //auto *mx_arg_2_4_left = mx_arg_2_4->front();
                            auto *mx_arg_3_5_left_part_1 = cast<CallInstr>(mx_arg_3_5)->front();
                            auto *mx_arg_3_5_left_part_1_1 = cast<CallInstr>(mx_arg_3_5_left_part_1)->front();
                            std::vector<codon::ir::Var *> part_1_lst = mx_arg_3_5_left_part_1_1->getUsedVariables();
                            auto part_1_lst_name = part_1_lst[0]->getName();
                            mx3_07 = part_1_lst_name;
                            auto *mx_arg_3_5_left_part_1_2 = cast<CallInstr>(mx_arg_3_5_left_part_1)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_left_part_1_2) == NULL){
                                mx3_08 = "0";
                            }
                            else{
                                auto mx_arg_3_5_left_inst = util::getFunc(cast<CallInstr>(mx_arg_3_5_left_part_1_2)->getCallee());
                                auto mx_arg_3_5_inst = mx_arg_3_5_left_inst->getUnmangledName();
                                if(mx_arg_3_5_inst == "__sub__"){
                                    mx3_08 += "-";
                                }
                                auto *part_1_add = cast<CallInstr>(mx_arg_3_5_left_part_1_2)->back();
                                std::ostringstream oss_part_1_add;
                                oss_part_1_add << *part_1_add;
                                std::string oss_part_1_add_str = oss_part_1_add.str();                             
                                mx3_08 += oss_part_1_add_str;                                 
                            }
                            auto *mx_arg_3_5_part_2 = cast<CallInstr>(mx_arg_3_5)->back();                        
                            if(cast<CallInstr>(mx_arg_3_5_part_2) == NULL){
                                mx3_09 = "0";
                            }
                            else{
                                auto *mx_arg_3_5_left_part_2 = util::getFunc(cast<CallInstr>(mx_arg_3_5_part_2)->getCallee());
                                auto mx_arg_3_5_part_2_inst = mx_arg_3_5_left_part_2->getUnmangledName();;
                                if(mx_arg_3_5_part_2_inst == "__sub__"){
                                    mx3_09 += "-";
                                }
                                auto *part_2_add = mx_arg_3_5->back();
                                std::ostringstream oss_part_2_add;  
                                oss_part_2_add << *part_2_add;
                                std::string oss_part_2_add_str = oss_part_2_add.str();                             
                                mx3_09 += oss_part_2_add_str;                                        
                            } 
                            mx3_10 = "0"; 
                        } 
                        std::map<std::string, std::string> mx3_attributes{{"mx3_00", mx3_00}, {"mx3_01", mx3_01}, {"mx3_02", mx3_02}, {"mx3_03", mx3_03}, {"mx3_04", mx3_04}, {"mx3_05", mx3_05}, {"mx3_06", mx3_06}, {"mx3_07", mx3_07}, {"mx3_08", mx3_08}, {"mx3_09", mx3_09}, {"mx3_10", mx3_10}}; 
                        globalAttributes["mx3"] = mx3_attributes;                                             
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
                                std::ostringstream oss_arg3_const;  
                                oss_arg3_const << *arg3_const;
                                std::string oss_arg3_const_str = oss_arg3_const.str();                             
                                params_24 = oss_arg3_const_str;                                       
                            }
                            else if (arg3_func_name == "__div__"){
                                params_24 = "1/";
                                std::ostringstream oss_arg3_const;  
                                oss_arg3_const << *arg3_const;
                                std::string oss_arg3_const_str = oss_arg3_const.str();                             
                                params_24 += oss_arg3_const_str;                                                                       
                            }
                            else{
                                params_24 = "1";
                            }               
                            std::string arg3_row_var_name = ""; // taking care of the row index of the first (left side) operand
                            if (arg3_row_instr != NULL){
                                auto *arg3_row_func = util::getFunc(arg3_row_instr->getCallee());
                                auto arg3_row_func_name = arg3_row_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row_instr->front()->getUsedVariables();
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    params_25 = "0";
                                }
                                else{
                                    params_25 = "1";
                                }
                                if (arg3_row_func_name == "__sub__"){
                                    params_26 += "-";
                                }            
                                auto *arg3_num_row = arg3_row_instr->back();
                                std::ostringstream oss_arg3_num_row;  
                                oss_arg3_num_row << *arg3_num_row;
                                std::string oss_arg3_num_row_str = oss_arg3_num_row.str();                             
                                params_26 += oss_arg3_num_row_str;                                            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row->getUsedVariables();            
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    params_25 = "0";
                                }
                                else{
                                    params_25 = "1";
                                }
                                params_26 = "0";
                            }
                            std::string arg3_col_var_name = ""; // taking care of the column index of the first (left side) operand
                            if (arg3_col_instr != NULL){
                                auto *arg3_num_col = arg3_col_instr->back();
                                auto *arg3_num_col_inst = cast<CallInstr>(arg3_num_col);                            
                                auto *arg3_col_func = util::getFunc(arg3_col_instr->getCallee());
                                auto arg3_col_func_name = arg3_col_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col_instr->front()->getUsedVariables();
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    params_27 = "0";
                                }
                                else{
                                    params_27 = "1";
                                } 
                                if (arg3_col_func_name == "__sub__"){
                                    params_28 += "-";
                                }                                                               
                                if(arg3_num_col_inst == NULL){   
                                    std::ostringstream oss_arg3_num_col;  
                                    oss_arg3_num_col << *arg3_num_col;
                                    std::string oss_arg3_num_col_str = oss_arg3_num_col.str();                             
                                    params_28 += oss_arg3_num_col_str;                                                                                                                    
                                }
                                else{
                                    auto *arg_3_num_col_left = arg3_num_col_inst->front();
                                    auto *arg_3_num_col_right = arg3_num_col_inst->back();
                                    std::vector<codon::ir::Var *> arg_3_num_col_left_vars = arg_3_num_col_left->getUsedVariables();
                                    auto arg_3_num_col_left_name = arg_3_num_col_left_vars[0]->getName();
                                    if(arg_3_num_col_left_name == pf_arg1){
                                        params_28 += "0 ";
                                    }
                                    else{
                                        params_28 += "1 ";
                                    }
                                    auto *arg_3_num_col_right_inst = cast<CallInstr>(arg_3_num_col_right);
                                    if(arg_3_num_col_right_inst == NULL){
                                        std::vector<codon::ir::Var *> arg_3_num_col_right_vars = arg_3_num_col_right->getUsedVariables();
                                        auto arg_3_num_col_right_name = arg_3_num_col_right_vars[0]->getName();
                                        if(arg_3_num_col_right_name == var_str_inner){
                                            params_28 += "0 0";
                                        }
                                        else{
                                            params_28 += "1 0";
                                        }
                                    }
                                    else{
                                        auto *arg3_num_col_right_left = arg_3_num_col_right_inst->front();
                                        std::vector<codon::ir::Var *> arg3_num_col_right_left_vars =  arg3_num_col_right_left->getUsedVariables();
                                        auto arg3_num_col_right_left_name = arg3_num_col_right_left_vars[0]->getName();
                                        if(arg3_num_col_right_left_name == var_str_inner){
                                            params_28 += "0 ";
                                        }
                                        else{
                                            params_28 += "1 ";
                                        }
                                        auto *arg3_num_col_right_right = arg_3_num_col_right_inst->back();
                                        auto *arg3_num_col_right_func = util::getFunc(arg_3_num_col_right_inst->getCallee());
                                        auto arg3_num_col_right_func_name = arg3_num_col_right_func->getUnmangledName();
                                        if(arg3_num_col_right_func_name == "__sub__"){
                                            params_28 += "-";
                                        }
                                        std::ostringstream oss_arg3_num_col_right_right;  
                                        oss_arg3_num_col_right_right << *arg3_num_col_right_right;
                                        std::string oss_arg3_num_col_right_right_str = oss_arg3_num_col_right_right.str();                             
                                        params_28 += oss_arg3_num_col_right_right_str;                                                   
                                    }
                                }        
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col->getUsedVariables();            
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    params_27 = "0";
                                }
                                else{
                                    params_27 = "1";
                                }
                                params_28 = "0";
                            }
                            if (arg3_func_name == "__sub__"){
                                params_29 += "-";
                                std::ostringstream oss_arg3_const;  
                                oss_arg3_const << *arg3_const;
                                std::string oss_arg3_const_str = oss_arg3_const.str();                             
                                params_29 += oss_arg3_const_str;                                    
                            }
                            else if (arg3_func_name == "__add__"){
                                if (cast<CallInstr>(arg3_const) == NULL){
                                    if(cast<TernaryInstr>(arg3_const) == NULL){
                                        std::ostringstream oss_arg3_const;  
                                        oss_arg3_const << *arg3_const;
                                        std::string oss_arg3_const_str = oss_arg3_const.str();                             
                                        params_29 += oss_arg3_const_str;  
                                    }
                                    else{
                                        params_29 = "0";
                                        auto *tmp_if_3 = cast<TernaryInstr>(arg3_const); 
                                        auto *m_call_3 = cast<CallInstr>(tmp_if_3->getCond());
                                        auto *op_3_call = util::getFunc(m_call_3->getCallee());
                                        auto op_3_call_name = op_3_call->getUnmangledName();  
                                        //auto *match_arg_1 = (cast<CallInstr>(m_call_3->front())->front());
                                        //auto *match_arg_2 = cast<CallInstr>(m_call_3->back())->front();
                                        auto *true_return_3 = tmp_if_3->getTrueValue();
                                        auto *false_return_3 = tmp_if_3->getFalseValue();                                                      
                                        std::string arg3_00 = "";
                                        std::string arg3_01 = "";
                                        std::string arg3_02 = "0";
                                        std::string arg3_03 = "";
                                        std::string arg3_04 = "";
                                        std::string arg3_05 = "";
                                        arg3_00 = "0";  
                                                                
                                        if(op_3_call_name == "__eq__"){
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *true_return_3;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg3_01 += oss_a_arg_str;   

                                            std::ostringstream oss_b_arg;  
                                            oss_b_arg << *false_return_3;
                                            std::string oss_b_arg_str = oss_b_arg.str();                             
                                            arg3_03 += oss_b_arg_str;                                                                                                                 
                                        
                                        }                                
                                        else{
                                            std::ostringstream oss_a_arg;  
                                            oss_a_arg << *true_return_3;
                                            std::string oss_a_arg_str = oss_a_arg.str();                             
                                            arg3_03 += oss_a_arg_str;   

                                            std::ostringstream oss_b_arg;  
                                            oss_b_arg << *false_return_3;
                                            std::string oss_b_arg_str = oss_b_arg.str();                             
                                            arg3_01 += oss_b_arg_str;                               
                                        }  
                                        auto *arg_3_1_index = cast<CallInstr>(m_call_3->front())->back();
                                        if(cast<CallInstr>(arg_3_1_index) != NULL){
                                            auto *arg_3_1_f = cast<CallInstr>(arg_3_1_index);
                                            auto *arg_3_1_index_func = util::getFunc(arg_3_1_f->getCallee());
                                            auto arg_3_1_index_func_name = arg_3_1_index_func->getUnmangledName();                                    
                                            if(arg_3_1_index_func_name == "__sub__"){
                                                arg3_04 += "-";
                                            }
                                            auto *tst_3 = arg_3_1_f->back(); 
                                            std::ostringstream oss_tst_3;  
                                            oss_tst_3 << *tst_3;
                                            std::string oss_tst_3_str = oss_tst_3.str();
                                            arg3_04 += oss_tst_3_str; 
                                        }
                                        else{
                                            arg3_04 = "0";
                                        }
                                        auto *arg_3_2_index = cast<CallInstr>(m_call_3->back())->back();
                                        if(cast<CallInstr>(arg_3_2_index) != NULL){
                                            auto *arg_3_2_f = cast<CallInstr>(arg_3_2_index);
                                            auto *arg_3_2_index_func = util::getFunc(arg_3_2_f->getCallee());
                                            auto arg_3_2_index_func_name = arg_3_2_index_func->getUnmangledName();                                    
                                            if(arg_3_2_index_func_name == "__sub__"){
                                                arg3_05 += "-";
                                            }
                                            auto *tst_3 = arg_3_2_f->back(); 
                                            std::ostringstream oss_tst_3;  
                                            oss_tst_3 << *tst_3;
                                            std::string oss_tst_3_str = oss_tst_3.str(); 
                                            arg3_05 += oss_tst_3_str;                                        
                                        }
                                        else{
                                            arg3_05 = "0";
                                        }                                       
                                        std::map<std::string, std::string> arg3_attributes{{"arg3_00", arg3_00}, {"arg3_01", arg3_01}, {"arg3_02", arg3_02}, {"arg3_03", arg3_03}, {"arg3_04", arg3_04}, {"arg3_05", arg3_05}}; 
                                        globalAttributes["arg3"] = arg3_attributes;                                                                                                                                             
                                    }                                                                                                              
                                }
                                else{
                                    params_29 = "0";
                                    auto *m_call_3 = cast<CallInstr>(arg3_const);
                                    auto *m_call_3_func = util::getFunc(m_call_3->getCallee());
                                    auto m_call_3_name = m_call_3_func->getUnmangledName();  
                                    auto match_att_3 = util::hasAttribute(m_call_3_func, "std.vectron.dispatcher.vectron_cmp");                                                              
                                    if (match_att_3){
                                        std::vector<codon::ir::Value *> body_3 = m_call_3_func->getUsedValues();
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
                                        std::vector<codon::ir::Var *> elif_arg_1 = elif_cond_3_call->front()->getUsedVariables();
                                        std::vector<codon::ir::Var *> elif_arg_2 = elif_cond_3_call->back()->getUsedVariables();
                                        auto elif_arg_1_name = elif_arg_1[0]->getName();
                                        auto elif_arg_2_name = elif_arg_2[0]->getName();
                                        std::string arg3_00 = "";
                                        std::string arg3_01 = "";
                                        std::string arg3_02 = "";
                                        std::string arg3_03 = "";
                                        std::string arg3_04 = "";
                                        std::string arg3_05 = "";  
                                        arg3_00 = "0";                             
                                        std::vector<codon::ir::Value *> all_args_3 = m_call_3->getUsedValues();
                                        auto *a_arg = all_args_3[1];                                
                                        auto *b_arg = all_args_3[2];
                                        auto *am_arg = all_args_3[3];     
                                        if(elif_cond_func_name == "__eq__" && (elif_arg_1_name == match_arg_1 || elif_arg_1_name == match_arg_2) && ((elif_arg_2_name == match_arg_1 || elif_arg_2_name == match_arg_2))){
                                            auto *return_val_3_1 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_3)->back())->getValue();
                                            auto *return_val_3_2 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_3)->back())->getValue();
                                            auto *return_val_3_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_3)->back())->getValue();
                                            std::vector<codon::ir::Var *> ret_val_1 = return_val_3_1->getUsedVariables();
                                            auto ret_val_1_name = ret_val_1[0]->getName();
                                            std::vector<codon::ir::Var *> ret_val_2 = return_val_3_2->getUsedVariables();
                                            auto ret_val_2_name = ret_val_2[0]->getName();
                                            std::vector<codon::ir::Var *> ret_val_3 = return_val_3_3->getUsedVariables();
                                            auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                            if(ret_val_1_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_01 = oss_a_arg_str;                                                                                   
                                            }
                                            else{
                                                if(ret_val_1_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_01 = oss_b_arg_str;                                                                                                                                              
                                                }
                                                else{
                                                    if(ret_val_1_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_01 = oss_am_arg_str;                                                                                                                                                       
                                                    }
                                                }
                                            }
                                            if(ret_val_2_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_02 = oss_a_arg_str;                                                                                          
                                            }
                                            else{
                                                if(ret_val_2_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_02 = oss_b_arg_str;                                                                                           
                                                }
                                                else{
                                                    if(ret_val_2_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_02 = oss_am_arg_str;                                                                                                        
                                                    }
                                                }
                                            }
                                            if(ret_val_3_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_03 = oss_a_arg_str;                                                                                                                              
                                            }
                                            else{
                                                if(ret_val_3_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_03 = oss_b_arg_str;                                                                                                
                                                }
                                                else{
                                                    if(ret_val_3_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_03 = oss_am_arg_str;                                                                                                         
                                                    }
                                                }
                                            }                                   
                                        }                                
                                        else{
                                            auto *return_val_3_2 = cast<ReturnInstr>(cast<SeriesFlow>(elif_return_3)->back())->getValue();
                                            auto *return_val_3_1 = cast<ReturnInstr>(cast<SeriesFlow>(true_return_3)->back())->getValue();
                                            auto *return_val_3_3 = cast<ReturnInstr>(cast<SeriesFlow>(else_branch_3)->back())->getValue();                                    
                                            std::vector<codon::ir::Var *> ret_val_1 = return_val_3_1->getUsedVariables();
                                            auto ret_val_1_name = ret_val_1[0]->getName();
                                            std::vector<codon::ir::Var *> ret_val_2 = return_val_3_2->getUsedVariables();
                                            auto ret_val_2_name = ret_val_2[0]->getName();
                                            std::vector<codon::ir::Var *> ret_val_3 = return_val_3_3->getUsedVariables();
                                            auto ret_val_3_name = ret_val_3[0]->getName();                                    

                                            if(ret_val_1_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_01 = oss_a_arg_str;                                                                                         
                                            }
                                            else{
                                                if(ret_val_1_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_01 = oss_b_arg_str;                                                      
                                                }
                                                else{
                                                    if(ret_val_1_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_01 = oss_am_arg_str;                                                                                                                                                     
                                                    }
                                                }
                                            }
                                            if(ret_val_2_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_02 = oss_a_arg_str;                                                                                 
                                            }
                                            else{
                                                if(ret_val_2_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_02 = oss_b_arg_str;                                                                                                  
                                                }
                                                else{
                                                    if(ret_val_2_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_02 = oss_am_arg_str;                                                                                                      
                                                    }
                                                }
                                            }
                                            if(ret_val_3_name == "a"){
                                                std::ostringstream oss_a_arg;  
                                                oss_a_arg << *a_arg;
                                                std::string oss_a_arg_str = oss_a_arg.str();                             
                                                arg3_03 = oss_a_arg_str;                                                                                       
                                            }
                                            else{
                                                if(ret_val_3_name == "b"){
                                                    std::ostringstream oss_b_arg;  
                                                    oss_b_arg << *b_arg;
                                                    std::string oss_b_arg_str = oss_b_arg.str();                             
                                                    arg3_03 = oss_b_arg_str;                                                                                              
                                                }
                                                else{
                                                    if(ret_val_3_name == "am"){
                                                        std::ostringstream oss_am_arg;  
                                                        oss_am_arg << *am_arg;
                                                        std::string oss_am_arg_str = oss_am_arg.str();                             
                                                        arg3_03 = oss_am_arg_str;                                                                                                             
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
                                                arg3_04 = "-";
                                            }
                                            auto *tst_1 = arg_3_1_f->back(); 
                                            std::ostringstream oss_tst_1;  
                                            oss_tst_1 << *tst_1;
                                            std::string oss_tst_1_str = oss_tst_1.str();                             
                                            arg3_04 += oss_tst_1_str;                                               
                                        }
                                        else{
                                            arg3_04 = "0";
                                        }
                                        auto *arg_3_2_index = cast<CallInstr>(m_call_3->back())->back();
                                        if(cast<CallInstr>(arg_3_2_index) != NULL){
                                            auto *arg_3_2_f = cast<CallInstr>(arg_3_2_index);
                                            auto *arg_3_2_index_func = util::getFunc(arg_3_2_f->getCallee());
                                            auto arg_3_2_index_func_name = arg_3_2_index_func->getUnmangledName();                                    
                                            if(arg_3_2_index_func_name == "__sub__"){
                                                arg3_05 += "-";
                                            }
                                            auto *tst_1 = arg_3_2_f->back(); 
                                            std::ostringstream oss_tst_1;  
                                            oss_tst_1 << *tst_1;
                                            std::string oss_tst_1_str = oss_tst_1.str();                             
                                            arg3_05 += oss_tst_1_str;                                            
                                        }
                                        else{
                                            arg3_05 = "0";
                                        }                  
                                        std::map<std::string, std::string> arg3_attributes{{"arg3_00", arg3_00}, {"arg3_01", arg3_01}, {"arg3_02", arg3_02}, {"arg3_03", arg3_03}, {"arg3_04", arg3_04}, {"arg3_05", arg3_05}}; 
                                        globalAttributes["arg3"] = arg3_attributes;                                                                           

                                    }
                                    
                                }
                            }
                            else{
                                params_29 = "0";
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
                            params_24 = "1";   
                            std::string arg3_row_var_name = ""; // taking care of the row index of the first (left side) operand
                            if (arg3_row_instr != NULL){
                                auto *arg3_row_func = util::getFunc(arg3_row_instr->getCallee());
                                auto arg3_row_func_name = arg3_row_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row_instr->front()->getUsedVariables();
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    params_25 = "0";
                                }
                                else{
                                    params_25 = "1";
                                }
                                if (arg3_row_func_name == "__sub__"){
                                    params_26 += "-";
                                }            
                                auto *arg3_num_row = arg3_row_instr->back();
                                std::ostringstream oss_arg3_num_row;  
                                oss_arg3_num_row << *arg3_num_row;
                                std::string oss_arg3_num_row_str = oss_arg3_num_row.str();                             
                                params_26 += oss_arg3_num_row_str;     
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_row_var = arg3_row->getUsedVariables();            
                                arg3_row_var_name = arg3_row_var[0]->getName();
                                if (var_str_outer == arg3_row_var_name){
                                    params_25 = "0";
                                }
                                else{
                                    params_25 = "1";
                                }
                                params_26 = "0";
                            }
                            std::string arg3_col_var_name = ""; // taking care of the column index of the first (left side) operand
                            if (arg3_col_instr != NULL){
                                auto *arg3_col_func = util::getFunc(arg3_col_instr->getCallee());
                                auto arg3_col_func_name = arg3_col_func->getUnmangledName();
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col_instr->front()->getUsedVariables();
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    params_27 = "0";
                                }
                                else{
                                    params_27 = "1";
                                }
                                if (arg3_col_func_name == "__sub__"){
                                    params_28 += "-";
                                }            
                                auto *arg3_num_col = arg3_col_instr->back();
                                std::ostringstream oss_arg3_num_col;  
                                oss_arg3_num_col << *arg3_num_col;
                                std::string oss_arg3_num_col_str = oss_arg3_num_col.str();                             
                                params_28 += oss_arg3_num_col_str;   
                                params_29 = "0";                                            
                            }
                            else{
                                std::vector<codon::ir::Var *> arg3_col_var = arg3_col->getUsedVariables();            
                                arg3_col_var_name = arg3_col_var[0]->getName();
                                if (var_str_outer == arg3_col_var_name){
                                    params_27 = "0";
                                }
                                else{
                                    params_27 = "1";
                                }
                                params_28 = "0";
                                params_29 = "0";
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
        bool max_att_7 = false;
        if(arg1_inst != NULL){
            arg1_func_name = util::getFunc(arg1_inst->getCallee())->getUnmangledName();  
            auto *max_st_7 = util::getFunc(arg1_inst->getCallee());
            max_att_7 = util::hasAttribute(max_st_7, "std.vectron.dispatcher.vectron_max");             
        }
        if (arg1_inst != NULL && !max_att_7){      
            auto *arg1_op = cast<CallInstr>(arg1_inst->front()); // the left side operand        
            auto *arg1_fr = cast<CallInstr>(arg1_op->front()); // the left side variable + its row index                         
            if(arg1_fr != NULL){
                auto *arg1_var = arg1_fr->front(); // the left side variable              
                std::vector<codon::ir::Var *> arg1_vars = arg1_var->getUsedVariables();
                auto arg1_var_name = arg1_vars[0]->getName(); 
                params_30 = arg1_var_name; 
            }
            else{
                std::vector<codon::ir::Var *> arg1_vars = arg1_op->front()->getUsedVariables();
                auto arg1_var_name = arg1_vars[0]->getName(); 
                params_30 = arg1_var_name;
            }
        }
        else{
            params_30 = "0";   
        }
        auto *r_mid_0 = cast<CallInstr>(right_side->front());
        auto r_mid = r_mid_0->begin();        
        auto *arg2_inst = cast<CallInstr>(r_mid[1]);     
        std::string arg2_func_name = "";
        bool max_att_8 = false;        
        if (arg2_inst != NULL){
            arg2_func_name = util::getFunc(arg2_inst->getCallee())->getUnmangledName();             
            auto *max_st_8 = util::getFunc(arg2_inst->getCallee());
            max_att_8 = util::hasAttribute(max_st_8, "std.vectron.dispatcher.vectron_max");             
        }          
        if (arg2_inst != NULL && !max_att_8){
            auto *arg2_op = cast<CallInstr>(arg2_inst->front()); // the left side operand
            auto *arg2_fr = cast<CallInstr>(arg2_op->front()); // the left side variable + its row index               
            if(arg2_fr != NULL){              
                auto *arg2_var = arg2_fr->front(); // the left side variable              
                std::vector<codon::ir::Var *> arg2_vars = arg2_var->getUsedVariables();            
                auto arg2_var_name = arg2_vars[0]->getName();
                params_31 = arg2_var_name;
            }
            else{
                std::vector<codon::ir::Var *> arg2_vars = arg2_op->front()->getUsedVariables();            
                auto arg2_var_name = arg2_vars[0]->getName();
                params_31 = arg2_var_name;         
            }
        }
        else{
            params_31 = "0";
        }
        auto *r_end = cast<CallInstr>(right_side->front())->back(); // the last argument of the main function (min or max)
        if (r_end != r_mid[1]){        
            auto *arg3 = cast<CallInstr>(right_side->front()); // the right side operand                         
            auto *arg3_inst = cast<CallInstr>(arg3->back()); 
            std::string arg3_func_name = "";
            bool max_att_9 = false;
            if(arg3_inst != NULL){
                arg3_func_name = util::getFunc(arg3_inst->getCallee())->getUnmangledName();   
                auto *max_st_9 = util::getFunc(arg3_inst->getCallee());
                max_att_9 = util::hasAttribute(max_st_9, "std.vectron.dispatcher.vectron_max");                  
            }       
            if (arg3_inst != NULL && !max_att_9){
                auto *arg3_op = cast<CallInstr>(arg3_inst->front()); // the left side operand
                auto *arg3_fr = cast<CallInstr>(arg3_op->front()); // the left side variable + its row index                       
                if(arg3_fr != NULL){
                    auto *arg3_var = arg3_fr->front(); // the left side variable                                                                                
                    std::vector<codon::ir::Var *> arg3_vars = arg3_var->getUsedVariables();            
                    auto arg3_var_name = arg3_vars[0]->getName();    
                    params_32 = arg3_var_name;                          
                }
                else{
                    std::vector<codon::ir::Var *> arg3_vars = arg3_op->front()->getUsedVariables();            
                    auto arg3_var_name = arg3_vars[0]->getName();      
                    params_32 = arg3_var_name;                                          
                }
            }
            else{
                params_32 = "0";
            }
        }     
        params_33 = left_side_name;          
        std::map<std::string, std::string> params_attributes{{"params_00", params_00}, {"params_01", params_01}, {"params_02", params_02}, {"params_03", params_03}, {"params_04", params_04}, {"params_05", params_05}, {"params_06", params_06}, {"params_07", params_07}, {"params_08", params_08}, {"params_09", params_09}, {"params_10", params_10}, {"params_11", params_11}, {"params_12", params_12}, {"params_13", params_13}, {"params_14", params_14}, {"params_15", params_15}, {"params_16", params_16}, {"params_17", params_17}, {"params_18", params_18}, {"params_19", params_19}, {"params_20", params_20}, {"params_21", params_21}, {"params_22", params_22}, {"params_23", params_23}, {"params_24", params_24}, {"params_25", params_25}, {"params_26", params_26}, {"params_27", params_27}, {"params_28", params_28}, {"params_29", params_29}, {"params_30", params_30}, {"params_31", params_31}, {"params_32", params_32}, {"params_33", params_33}}; 
        globalAttributes["params"] = params_attributes;                  
    }
    else{
        return;
    }
}


void LoopAnalyzer::handle(ImperativeForFlow *v) { transform(v); }

} // namespace vectron


