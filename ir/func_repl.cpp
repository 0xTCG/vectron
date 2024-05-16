#include "func_repl.h"
#include "codon/parser/cache.h"
#include "codon/compiler/compiler.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include "codon/parser/ast/stmt.h"
#include "codon/parser/peg/peg.h"
#include <iostream>
#include <fstream>

namespace vectron {

    using namespace codon::ir;


    void FuncReplacement::transform(CallInstr *v) {  
        auto *M = v->getModule(); 
        auto *orig = util::getFunc(v->getCallee());
        auto att_att = util::hasAttribute(orig, "vectron_init");
        if (!att_att)
            return;   
        else{
            std::ifstream var_file("var_type.txt");
            std::basic_string var_type = "";
            std::getline(var_file, var_type);         

            if (var_type == "\"i16\""){    
                std::cout << "i16\n"; 
                std::string code = 
                                    "from experimental.simd import *\n"
                                    "from bio import *\n"
                                    "import time\n"
                                    "import os\n"
                                    "@staticmethod\n"
                                    "@inline\n"
                                    "def match_vec(checker, x_vec, y_vec, a, b, am, params_const, n):\n"
                                    "    if checker == 0:\n"
                                    "        return Vec(i16(params_const), i16, 16)\n"
                                    "    return Vec.cmp_vec(x_vec, y_vec, a, b, am, n)\n"
                                    "@staticmethod\n"
                                    "@inline\n"
                                    "def y_gen(seqs):\n"
                                    "    num = 0\n"
                                    "    for row in range(num + 1, len(seqs)):\n"
                                    "        if len(seqs[row]) != len(seqs[num]):\n"
                                    "            yield seqs[num: row]\n"
                                    "            num = row\n"
                                    "    yield seqs[num:]\n"
                                    "@staticmethod\n"
                                    "@inline\n";
                std::ifstream loop_file("LoopInfo.txt");
                std::basic_string min_check = "";
                //std::string params = "";
                std::string temp_reader = "";
                std::string params_test[34];
                int params[30];
                int array_ind_1[4] = {2, 0, 0, 1};
                int array_ind_2[4] = {2, 0, 0, 1};
                int array_ind_3[4] = {2, 0, 0, 1};
                for(int i = 0; i < 34; i++){
                    if(loop_file.eof()){
                    break;
                    }        
                    std::getline(loop_file, temp_reader);
                    params_test[i] = temp_reader;
                    if(i == 8){
                    min_check = temp_reader;
                    }
                }
                std::string first_param = "";
                std::string second_param = "";
                std::string third_param = "";
                std::string left_param = "";      
                if(params_test[11] == "-1"){
                    third_param = "0";
                    first_param = params_test[24].substr(0, params_test[24].find('.'));
                    second_param = params_test[25].substr(0, params_test[25].find('.'));
                    left_param = params_test[26].substr(0, params_test[26].find('.'));
                    for(int i = 0; i < 24; i++){
                    params[i] = std::stoi(params_test[i]);
                    }
                }
                else{
                    first_param = params_test[30].substr(0, params_test[30].find('.'));
                    second_param = params_test[31].substr(0, params_test[31].find('.'));
                    third_param = params_test[32].substr(0, params_test[32].find('.'));
                    left_param = params_test[33].substr(0, params_test[33].find('.'));
                    for(int i = 0; i < 30; i++){
                    if(i == 16){            
                        if(params_test[i].length() > 2){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_1[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_1[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_1[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_1[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_1[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_1[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_1[2] = std::stoi(temp_reader);                                
                        }

                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }
                    }
                    if(i == 22){
                        if(params_test[i].length() > 3){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_2[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_2[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_2[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_2[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_2[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_2[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_2[2] = std::stoi(temp_reader);                                
                        }
                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }            
                    }   
                    if(i == 28){
                        if(params_test[i].length() > 3){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_3[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_3[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_3[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_3[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_3[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_3[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_3[2] = std::stoi(temp_reader);                                
                        }
                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }            
                    }
                    if(i != 16 || i != 22 || i != 28){                 
                        params[i] = std::stoi(params_test[i]);
                    }
                    }        

                }
                std::ifstream prep_file("Prep_info.txt");      
                int hyper_p[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                for(int i = 0; i < 8; i++){
                    std::getline(prep_file, temp_reader);
                    hyper_p[i] = std::stoi(temp_reader);
                }
                prep_file.close();
                std::vector<Value*> hyper_params_arr;    
                for(int i = 0; i < 8; i++){
                    hyper_params_arr.push_back(M->getInt(hyper_p[i]));
                }
                Value *hyper_params_ptr = util::makeTuple(hyper_params_arr, M);       

                std::vector<Value*> arr_params_arr1;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr1.push_back(M->getInt(array_ind_1[i]));
                }
                Value *arr1_params_ptr = util::makeTuple(arr_params_arr1, M);       

                std::vector<Value*> arr_params_arr2;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr2.push_back(M->getInt(array_ind_2[i]));
                }
                Value *arr2_params_ptr = util::makeTuple(arr_params_arr2, M); 

                std::vector<Value*> arr_params_arr3;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr3.push_back(M->getInt(array_ind_3[i]));
                }
                Value *arr3_params_ptr = util::makeTuple(arr_params_arr3, M);                                       

                std::vector<types::Type*> types_arr;    
                for(int i = 0; i < 30; i++){
                    types_arr.push_back(M->getIntType());
                }
                types::Type *typ_ptr30 = M->getTupleType(types_arr);

                std::vector<types::Type*> types_arr_int3;  
                for(int i = 0; i < 3; i++){
                    types_arr_int3.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int3 = M->getTupleType(types_arr_int3);

                std::vector<types::Type*> types_arr_int4;  
                for(int i = 0; i < 4; i++){
                    types_arr_int4.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int4 = M->getTupleType(types_arr_int4); 

                std::vector<types::Type*> types_arr6;  
                for(int i = 0; i < 6; i++){
                    types_arr6.push_back(M->getIntType());
                }
                types::Type *typ_ptr6 = M->getTupleType(types_arr6);

                std::vector<types::Type*> types_arr8;  
                for(int i = 0; i < 8; i++){
                    types_arr8.push_back(M->getIntType());
                }
                types::Type *typ_ptr8 = M->getTupleType(types_arr8);            

                std::vector<types::Type*> types_arr9;  
                for(int i = 0; i < 9; i++){
                    types_arr9.push_back(M->getIntType());
                }
                types::Type *typ_ptr9 = M->getTupleType(types_arr9);
                

                std::vector<types::Type*> types_arr10;  
                for(int i = 0; i < 10; i++){
                    types_arr10.push_back(M->getStringType());
                }
                types::Type *typ_ptr10 = M->getTupleType(types_arr10);    

                std::vector<types::Type*> types_arr_int10;  
                for(int i = 0; i < 10; i++){
                    types_arr_int10.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int10 = M->getTupleType(types_arr_int10);          

                std::vector<types::Type*> types_arr3;  
                for(int i = 0; i < 3; i++){
                    types_arr3.push_back(M->getStringType());
                }
                types::Type *typ_ptr3 = M->getTupleType(types_arr3);

                std::vector<Value*> params_arr;    
                for(int i = 0; i < 30; i++){
                    params_arr.push_back(M->getInt(params[i]));
                }
                Value *params_ptr = util::makeTuple(params_arr, M);      
                loop_file.close();     
                int checker_1 = 1;
                int checker_2 = 1;
                int checker_3 = 1;
                int args1_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg1_arr;    
                int args2_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg2_arr;    
                int args3_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg3_arr;        

                std::ifstream arg_1_file("arg_1.txt");
                if(!arg_1_file){
                    checker_1 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_1_file, temp_reader);
                    args1_test[i] = std::stoi(temp_reader);
                    }
                }
                arg_1_file.close();
                std::ifstream arg_2_file("arg_2.txt");
                if(!arg_2_file){
                    checker_2 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_2_file, temp_reader);
                    args2_test[i] = std::stoi(temp_reader);
                    }
                }     
                arg_2_file.close();
                std::ifstream arg_3_file("arg_3.txt");
                if(!arg_3_file){
                    checker_3 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_3_file, temp_reader);
                    args3_test[i] = std::stoi(temp_reader);
                    }
                } 
                arg_3_file.close();
                auto *checker_1_ptr = M->getInt(checker_1);
                auto *checker_2_ptr = M->getInt(checker_2);
                auto *checker_3_ptr = M->getInt(checker_3);
                for(int i = 0; i < 6; i++){
                    arg1_arr.push_back(M->getInt(args1_test[i]));
                    arg2_arr.push_back(M->getInt(args2_test[i]));
                    arg3_arr.push_back(M->getInt(args3_test[i]));
                }
                Value *args1_ptr = util::makeTuple(arg1_arr, M);    
                Value *args2_ptr = util::makeTuple(arg2_arr, M);    
                Value *args3_ptr = util::makeTuple(arg3_arr, M);    

                int lst1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst1_arr;        
                int lst2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst2_arr;  
                int lst3[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst3_arr;        
                std::string lst1_name = "";
                std::string lst2_name = "";
                std::string lst3_name = "";
                std::string lst1_eq = "";
                std::string lst2_eq = "[[0 for _ in range(SIZE + 1)] for __ in range(SIZE + 1)]";
                std::string lst3_eq = "[[0 for _ in range(SIZE + 1)] for __ in range(SIZE + 1)]";
                std::ifstream lst_1_file("lst_1.txt");
                if(lst_1_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_1_file, temp_reader);
                        lst1_name = temp_reader;
                    }
                    else{
                        std::getline(lst_1_file, temp_reader);
                        lst1_eq = temp_reader;
                    }
                    }
                }
                lst_1_file.close();
                std::ifstream lst_2_file("lst_2.txt");
                if(lst_2_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_2_file, temp_reader);
                        lst2_name = temp_reader;
                    }
                    else{
                        std::getline(lst_2_file, temp_reader);
                        lst2_eq = temp_reader;
                    }
                    }
                }
                lst_2_file.close();
                std::ifstream lst_3_file("lst_3.txt");
                if(lst_3_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_3_file, temp_reader);
                        lst3_name = temp_reader;
                    }
                    else{
                        std::getline(lst_3_file, temp_reader);
                        lst3_eq = temp_reader;
                    }
                    }
                }
                lst_3_file.close();
                //for(int i = 0; i < 10; i++){
                //    lst1_arr.push_back(M->getInt(lst1[i]));
                //    lst2_arr.push_back(M->getInt(lst2[i]));
                //    lst3_arr.push_back(M->getInt(lst3[i]));
               // }
                auto *lst1_ptr = M->getString(lst1_eq);
                auto *lst2_ptr = M->getString(lst2_eq);
                auto *lst3_ptr = M->getString(lst3_eq);
                auto *lst1_name_ptr = M->getString(lst1_name);
                auto *lst2_name_ptr = M->getString(lst2_name);
                auto *lst3_name_ptr = M->getString(lst3_name);          
                int byPass = -1;
                std::string byPass_mat = "";
                std::ifstream byPass_file("byPass.txt");
                if(byPass_file){
                    std::getline(byPass_file, temp_reader);
                    byPass = stoi(temp_reader);
                    std::getline(byPass_file, temp_reader);
                    byPass_mat = temp_reader;

                }
                auto *byPass_ptr = M->getInt(byPass);
                auto *args_1 = v->front();
                auto *args_2 = v->back();
                std::ifstream bw_file("bw.txt");
                int bw_temp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> bw_arr;  
                int bw = 0;
                int bw_param = 0;
                for(int i = 0; i < 9; i++){
                    std::getline(bw_file, temp_reader);
                    bw_temp[i] = std::stoi(temp_reader);
                    bw += bw_temp[i];
                    if(i == 0){
                    bw_param = bw_temp[i];
                    }
                }
                bw_file.close();  
                int t1_l = 0;
                int t2_l = 0;     
                int t3_l = 0;
                int t4_l = 0;
                int t1_r = 0;
                int t2_r = 0;
                int t3_r = 0;
                int t4_r = 0;
                int y_params_1 = 0;
                int y_params_2 = 0;
                int x_params_1 = 0;
                int x_params_2 = 0;      
                if(bw != 0){
                    if(bw_temp[2] == -2 || bw_temp[2] == 2){
                        t1_l = bw_temp[2];
                        t2_l = bw_temp[2] / 2;
                    }
                    else{
                        t1_l = 0;
                        t2_l = bw_temp[2];
                    }
                    if(bw_temp[3] == -2 || bw_temp[3] == 2){
                        t3_l = bw_temp[3];
                        t4_l = bw_temp[3] / 2;
                    }
                    else{
                        t3_l = 0;
                        t4_l = bw_temp[3];
                    }
                    if(bw_temp[6] == -2 || bw_temp[6] == 2){
                        t1_r = bw_temp[6];
                        t2_r = bw_temp[6] / 2;
                    }
                    else{
                        t1_r = 0;
                        t2_r = bw_temp[6];
                    }
                    if(bw_temp[7] == -2 || bw_temp[7] == 2){
                        t3_r = bw_temp[7];
                        t4_r = bw_temp[7] / 2;
                    }
                    else{
                        t3_r = 0;
                        t4_r = bw_temp[7];
                    }

                    y_params_1 = (bw_temp[2] - t2_l) + (bw_temp[3] - t4_l);
                    y_params_2 = (bw_temp[6] - t2_r) + (bw_temp[7] - t4_r);
                    x_params_1 = (bw_temp[2] - t1_l) + (bw_temp[3] - t3_l);
                    x_params_2 = (bw_temp[6] - t1_r) + (bw_temp[7] - t3_r);           
                }
                auto *y_params_1_ptr = M->getInt(y_params_1);
                auto *y_params_2_ptr = M->getInt(y_params_2);
                auto *x_params_1_ptr = M->getInt(x_params_1);
                auto *x_params_2_ptr = M->getInt(x_params_2);
            
                for(int i = 0; i < 9; i++){
                    bw_arr.push_back(M->getInt(bw_temp[i]));
                }
                Value *bw_ptr = util::makeTuple(bw_arr, M); 

                std::string mx_1_checker = "1";
                std::string mx_2_checker = "1";
                std::string mx_3_checker = "1";      

                std::string mx_1_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx1_arr_str;  
                std::string mx_2_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx2_arr_str;        
                std::string mx_3_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx3_arr_str;

                int mx_1_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx1_arr;  
                int mx_2_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx2_arr;  
                int mx_3_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx3_arr;                

                std::ifstream mx_1_file("mx_arg1.txt");
                if(!mx_1_file){
                    mx_1_checker = "0";
                }
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_1_file, temp_reader);
                    if(i == 0){
                        mx_1_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_1_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_1_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_1_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_1_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_1_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_1_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_1_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_1_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_1_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_1_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                }
                mx_1_file.close();
                std::ifstream mx_2_file("mx_arg2.txt");
                if(!mx_2_file){
                    mx_2_checker = "0";
                }      
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_2_file, temp_reader);
                    if(i == 0){
                        mx_2_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_2_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_2_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_2_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_2_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_2_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_2_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_2_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_2_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_2_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_2_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                } 
                mx_2_file.close();   
                std::ifstream mx_3_file("mx_arg3.txt");   
                if(!mx_3_file){
                    mx_3_checker = "0";
                }       
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_3_file, temp_reader);
                    if(i == 0){
                        mx_3_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_3_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_3_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_3_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_3_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_3_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_3_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_3_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_3_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_3_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_3_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                }
                mx_3_file.close();   
                for(int i = 0; i < 8; i++){
                    mx1_arr.push_back(M->getInt(mx_1_array[i]));
                    mx2_arr.push_back(M->getInt(mx_2_array[i]));
                    mx3_arr.push_back(M->getInt(mx_3_array[i]));
                }
                Value *mx1_ptr = util::makeTuple(mx1_arr, M);    
                Value *mx2_ptr = util::makeTuple(mx2_arr, M);    
                Value *mx3_ptr = util::makeTuple(mx3_arr, M);        

                for(int i = 0; i < 3; i++){
                    mx1_arr_str.push_back(M->getString(mx_1_array_str[i]));
                    mx2_arr_str.push_back(M->getString(mx_2_array_str[i]));
                    mx3_arr_str.push_back(M->getString(mx_3_array_str[i]));
                }
                Value *mx1_ptr_str = util::makeTuple(mx1_arr_str, M);    
                Value *mx2_ptr_str = util::makeTuple(mx2_arr_str, M);    
                Value *mx3_ptr_str = util::makeTuple(mx3_arr_str, M);    
                
                int mx1_names_final[3] = {-1, -1, -1};
                int mx2_names_final[3] = {-1, -1, -1};
                int mx3_names_final[3] = {-1, -1, -1};       

                int inds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int inds_H[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int inds_V[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int first_list = -1;
                int second_list = -1;
                int third_list = -1;
                if(left_param == lst1_name || (params[8] + params[9] + params[10] == 0)){
                    inds[0] = 1;
                    inds[1] = 1;
                    inds[2] = lst1[4];
                    inds[3] = lst1[5];
                    inds[4] = lst1[6];
                    inds[5] = lst1[7];
                    inds[6] = lst1[8];
                    inds[7] = lst1[9];
                    inds_H[0] = 2;
                    inds_H[1] = 2;
                    inds_H[2] = lst2[4];
                    inds_H[3] = lst2[5];
                    inds_H[4] = lst2[6];
                    inds_H[5] = lst2[7];
                    inds_H[6] = lst2[8];
                    inds_H[7] = lst2[9];          
                    inds_V[0] = 3;
                    inds_V[1] = 3;
                    inds_V[2] = lst3[4];
                    inds_V[3] = lst3[5];
                    inds_V[4] = lst3[6];
                    inds_V[5] = lst3[7];
                    inds_V[6] = lst3[8];
                    inds_V[7] = lst3[9];                    
                    if(first_param == lst1_name){
                        first_list = 0;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 1;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 2;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 0;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 1;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 2;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 0;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 1;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 2;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 0;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 1;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 2;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 0;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 1;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 2;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 0;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 1;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 2;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 0;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 1;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 2;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 0;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 1;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 2;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 0;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 1;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 2;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 0;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 1;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 2;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 0;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 1;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 2;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 0;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 1;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 2;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }

                }             
                else{
                    if(left_param == lst2_name){  
                    inds[0] = 2;
                    inds[1] = 2;
                    inds[2] = lst2[4];
                    inds[3] = lst2[5];
                    inds[4] = lst2[6];
                    inds[5] = lst2[7];
                    inds[6] = lst2[8];
                    inds[7] = lst2[9];
                    inds_H[0] = 1;
                    inds_H[1] = 1;
                    inds_H[2] = lst1[4];
                    inds_H[3] = lst1[5];
                    inds_H[4] = lst1[6];
                    inds_H[5] = lst1[7];
                    inds_H[6] = lst1[8];
                    inds_H[7] = lst1[9];          
                    inds_V[0] = 3;
                    inds_V[1] = 3;
                    inds_V[2] = lst3[4];
                    inds_V[3] = lst3[5];
                    inds_V[4] = lst3[6];
                    inds_V[5] = lst3[7];
                    inds_V[6] = lst3[8];
                    inds_V[7] = lst3[9];            
                    if(first_param == lst1_name){
                        first_list = 1;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 0;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 2;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 1;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 0;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 2;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 1;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 0;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 2;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 1;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 0;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 2;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 1;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 0;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 2;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 1;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 0;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 2;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 1;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 0;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 2;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 1;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 0;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 2;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 1;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 0;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 2;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 1;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 0;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 2;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 1;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 0;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 2;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 1;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 0;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 2;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }                
                    }
                    else{
                    inds[0] = 3;
                    inds[1] = 3;
                    inds[2] = lst3[4];
                    inds[3] = lst3[5];
                    inds[4] = lst3[6];
                    inds[5] = lst3[7];
                    inds[6] = lst3[8];
                    inds[7] = lst3[9];
                    inds_H[0] = 2;
                    inds_H[1] = 2;
                    inds_H[2] = lst2[4];
                    inds_H[3] = lst2[5];
                    inds_H[4] = lst2[6];
                    inds_H[5] = lst2[7];
                    inds_H[6] = lst2[8];
                    inds_H[7] = lst2[9];          
                    inds_V[0] = 1;
                    inds_V[1] = 1;
                    inds_V[2] = lst1[4];
                    inds_V[3] = lst1[5];
                    inds_V[4] = lst1[6];
                    inds_V[5] = lst1[7];
                    inds_V[6] = lst1[8];
                    inds_V[7] = lst1[9];            
                    if(first_param == lst1_name){
                        first_list = 2;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 1;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 0;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 2;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 1;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 0;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 2;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 1;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 0;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 2;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 1;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 0;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 2;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 1;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 0;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 2;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 1;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 0;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 2;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 1;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 0;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 2;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 1;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 0;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 2;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 1;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 0;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 2;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 1;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 0;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 2;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 1;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 0;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 2;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 1;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 0;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }     
                    }     
                }
                auto *first_list_ptr = M->getInt(first_list);
                auto *second_list_ptr = M->getInt(second_list);
                auto *third_list_ptr = M->getInt(third_list);
                std::vector<Value*> inds_arr;  
                std::vector<Value*> inds_H_arr;  
                std::vector<Value*> inds_V_arr;                    
                for(int i = 0; i < 8; i++){
                    inds_arr.push_back(M->getInt(inds[i]));
                    inds_H_arr.push_back(M->getInt(inds_H[i]));
                    inds_V_arr.push_back(M->getInt(inds_V[i]));
                }
                Value *inds_ptr = util::makeTuple(inds_arr, M);    
                Value *inds_H_ptr = util::makeTuple(inds_H_arr, M);    
                Value *inds_V_ptr = util::makeTuple(inds_V_arr, M);        

                std::vector<Value*> mx1_names_arr;  
                std::vector<Value*> mx2_names_arr;  
                std::vector<Value*> mx3_names_arr;              
                for(int i = 0; i < 3; i++){
                    mx1_names_arr.push_back(M->getInt(mx1_names_final[i]));
                    mx2_names_arr.push_back(M->getInt(mx2_names_final[i]));
                    mx3_names_arr.push_back(M->getInt(mx3_names_final[i]));
                }
                Value *mx1_names_ptr = util::makeTuple(mx1_names_arr, M);    
                Value *mx2_names_ptr = util::makeTuple(mx2_names_arr, M);    
                Value *mx3_names_ptr = util::makeTuple(mx3_names_arr, M);          

                std::string mx_final = mx_1_checker + mx_2_checker + mx_3_checker;
                int mx_final_i = stoi(mx_final);    
                switch(mx_final_i){
                    case 0:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    a_1 = Vec(i16(args1[1]), i16, 16)\n"
                                "    b_1 = Vec(i16(args1[3]), i16, 16)\n"
                                "    am_1 = Vec(i16(args1[2]), i16, 16)\n"
                                "    x_ind_1 = args1[4]\n"
                                "    y_ind_1 = args1[5]\n"
                                "    a_2 = Vec(i16(args2[1]), i16, 16)\n"
                                "    b_2 = Vec(i16(args2[3]), i16, 16)\n"
                                "    am_2 = Vec(i16(args2[2]), i16, 16)\n"
                                "    x_ind_2 = args2[4]\n"
                                "    y_ind_2 = args2[5]\n"
                                "    a_3 = Vec(i16(args3[1]), i16, 16)\n"
                                "    b_3 = Vec(i16(args3[3]), i16, 16)\n"
                                "    am_3 = Vec(i16(args3[2]), i16, 16)\n"
                                "    x_ind_3 = args3[4]\n"
                                "    y_ind_3 = args3[5]\n"                        
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"
                                "    arg_1_c0 = (m_c0_x if params[14] == -1 else m_c1_x) if first_list == 0 else (v_c0_x if params[14] == -1 else v_c1_x) if first_list == 2 else (h_c0_x if params[14] == -1 else h_c1_x) if first_list == 1 else (m_c0_x if params[14] == -1 else m_c1_x)\n"
                                "    arg_1_c1 = (m_c1_x if params[14] == -1 else m_c0_x) if first_list == 0 else (v_c1_x if params[14] == -1 else v_c0_x) if first_list == 2 else (h_c1_x if params[14] == -1 else h_c0_x) if first_list == 1 else (m_c1_x if params[14] == -1 else m_c0_x)\n"
                                "    arg_2_c0 = (m_c0_x if params[20] == -1 else m_c1_x) if second_list == 0 else (v_c0_x if params[20] == -1 else v_c1_x) if second_list == 2 else (h_c0_x if params[20] == -1 else h_c1_x) if second_list == 1 else (m_c0_x if params[20] == -1 else m_c1_x)\n"
                                "    arg_2_c1 = (m_c1_x if params[20] == -1 else m_c0_x) if second_list == 0 else (v_c1_x if params[20] == -1 else v_c0_x) if second_list == 2 else (h_c1_x if params[20] == -1 else h_c0_x) if second_list == 1 else (m_c1_x if params[20] == -1 else m_c1_x)\n"
                                "    if params[11] != -1:\n"
                                "        arg_3_c0 = (m_c0_x if params[26] == -1 else m_c1_x) if third_list == 0 else (v_c0_x if params[26] == -1 else v_c1_x) if third_list == 2 else (h_c0_x if params[26] == -1 else h_c1_x) if third_list == 1 else (m_c0_x if params[26] == -1 else m_c1_x)\n"
                                "        arg_3_c1 = (m_c1_x if params[26] == -1 else m_c0_x) if third_list == 0 else (v_c1_x if params[26] == -1 else v_c0_x) if third_list == 2 else (h_c1_x if params[26] == -1 else h_c0_x) if third_list == 1 else (m_c1_x if params[26] == -1 else m_c0_x)\n"
                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    len_m = len(matrices[c1])\n"
                                "    len_h = len(H[c1])\n"
                                "    len_v = len(V[c1])\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        x_vec_ind_1 = current_x + x_ind_1\n"
                                "        x_vec_ind_2 = current_x + x_ind_2\n"
                                "        x_vec_ind_3 = current_x + x_ind_3\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";                                      
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        if len_m > current_y:\n"
                                    "                            m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_v > current_y:\n"
                                    "                            v_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_h > current_y:\n"
                                    "                            h_c1_x[current_y][i_y] = infinitizer\n"                            
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                arg_3_ind = current_y + params[28]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";

                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        if len_m > current_y:\n"
                                    "                            m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_v > current_y:\n"
                                    "                            v_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_h > current_y:\n"
                                    "                            h_c1_x[current_y][i_y] = infinitizer\n" 
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                arg_3_ind = current_y + params[28]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                          
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        if len_m > current_y:\n"
                                    "                            m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_v > current_y:\n"
                                    "                            v_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        if len_h > current_y:\n"
                                    "                            h_c1_x[current_y][i_y] = infinitizer\n" 
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                arg_3_ind = current_y + params[28]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                                                          
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                        Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";
                            }
                            else{
                                code +=
                                "                        Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";                        
                            }
                        }
                        else{
                            code +=
                            "            s_1 = seqs_t[1][current_y + y_ind_1]\n"
                            "            s_2 = seqs_t[1][current_y + y_ind_2]\n"
                            "            s_3 = seqs_t[1][current_y + y_ind_3]\n"
                            "            arg_1_ind = current_y + params[16]\n"
                            "            arg_2_ind = current_y + params[22]\n"
                            "            arg_3_ind = current_y + params[28]\n"
                            "            for ind_y in range(0, check, 2):\n"
                            "                if params[11] != -1:\n";
                            if(min_check == "1"){
                                code +=
                                "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";
                            }
                            else{
                                code +=
                                "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";                                   
                            }

                        }    
                code += 
                    "        for i in range(check):\n"
                    "           if count >= outer_stop:\n"
                    "               break\n"                                
                    "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                    "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                    "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                    "        count += 1\n"
                    "        c1, c0, m_c1_x, m_c0_x, arg_1_c0, arg_1_c1, arg_2_c0, arg_2_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_1_c1, arg_1_c0, arg_2_c1, arg_2_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n"
                    "        if params[11] != -1:\n"
                    "            arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0\n";
                        break;
                    }
                    case 1:{
                        code +=
                            "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                            "    count = 2\n"
                            "    a_1 = Vec(i16(args1[1]), i16, 16)\n"
                            "    b_1 = Vec(i16(args1[3]), i16, 16)\n"
                            "    am_1 = Vec(i16(args1[2]), i16, 16)\n"
                            "    x_ind_1 = args1[4]\n"
                            "    y_ind_1 = args1[5]\n"
                            "    a_2 = Vec(i16(args2[1]), i16, 16)\n"
                            "    b_2 = Vec(i16(args2[3]), i16, 16)\n"
                            "    am_2 = Vec(i16(args2[2]), i16, 16)\n"
                            "    x_ind_2 = args2[4]\n"
                            "    y_ind_2 = args2[5]\n"                      
                            "    SEQ_NO_Q = len(squery)\n"
                            "    REG_SIZE = 16\n"
                            "    c0 = 0\n"
                            "    c1 = 1\n"
                            "    m_c0_x = matrices[c0].arr.ptr\n"
                            "    m_c1_x = matrices[c1].arr.ptr\n"
                            "    v_c0_x = V[c0].arr.ptr\n"
                            "    v_c1_x = V[c1].arr.ptr\n"
                            "    h_c0_x = H[c0].arr.ptr\n"
                            "    h_c1_x = H[c1].arr.ptr\n"
                            "    mx3_arg2_vec = Vec(i16(inds_mx3[4]), i16, 16)\n"
                            "    mx3_arg3_vec = Vec(i16(inds_mx3[7]), i16, 16)\n"
                            "    mx3_arg1_c0 = (m_c0_x if inds_mx3[1] == -1 else m_c1_x) if mx3_names[0] == 0 else (v_c0_x if inds_mx3[1] == -1 else v_c1_x) if mx3_names[0] == 2 else (h_c0_x if inds_mx3[1] == -1 else h_c1_x)\n"
                            "    mx3_arg1_c1 = (m_c1_x if inds_mx3[1] == -1 else m_c0_x) if mx3_names[0] == 0 else (v_c1_x if inds_mx3[1] == -1 else v_c0_x) if mx3_names[0] == 2 else (h_c1_x if inds_mx3[1] == -1 else h_c0_x)\n"
                            "    mx3_arg2_c0 = (m_c0_x if inds_mx3[3] == -1 else m_c1_x) if mx3_names[1] == 0 else (v_c0_x if inds_mx3[3] == -1 else v_c1_x) if mx3_names[1] == 2 else (h_c0_x if inds_mx3[3] == -1 else h_c1_x)\n"      
                            "    mx3_arg2_c1 = (m_c1_x if inds_mx3[3] == -1 else m_c0_x) if mx3_names[1] == 0 else (v_c1_x if inds_mx3[3] == -1 else v_c0_x) if mx3_names[1] == 2 else (h_c1_x if inds_mx3[3] == -1 else h_c0_x)\n"
                            "    mx3_arg3_c0 = (m_c0_x if inds_mx3[6] == -1 else m_c1_x) if mx3_names[2] == 0 else (v_c0_x if inds_mx3[6] == -1 else v_c1_x) if mx3_names[2] == 2 else (h_c0_x if inds_mx3[6] == -1 else h_c1_x)\n"
                            "    mx3_arg3_c1 = (m_c1_x if inds_mx3[6] == -1 else m_c0_x) if mx3_names[2] == 0 else (v_c1_x if inds_mx3[6] == -1 else v_c0_x) if mx3_names[2] == 2 else (h_c1_x if inds_mx3[6] == -1 else h_c0_x)\n"
                            "    arg_1_c0 = (m_c0_x if params[14] == -1 else m_c1_x) if first_list == 0 else (v_c0_x if params[14] == -1 else v_c1_x) if first_list == 2 else (h_c0_x if params[14] == -1 else h_c1_x)\n"
                            "    arg_1_c1 = (m_c1_x if params[14] == -1 else m_c0_x) if first_list == 0 else (v_c1_x if params[14] == -1 else v_c0_x) if first_list == 2 else (h_c1_x if params[14] == -1 else h_c0_x)\n"
                            "    arg_2_c0 = (m_c0_x if params[20] == -1 else m_c1_x) if second_list == 0 else (v_c0_x if params[20] == -1 else v_c1_x) if second_list == 2 else (h_c0_x if params[20] == -1 else h_c1_x)\n"
                            "    arg_2_c1 = (m_c1_x if params[20] == -1 else m_c0_x) if second_list == 0 else (v_c1_x if params[20] == -1 else v_c0_x) if second_list == 2 else (h_c1_x if params[20] == -1 else h_c0_x)\n"
                            "    x_vec = x.arr.ptr\n"
                            "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                            "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                            "    L = bw_params[4]\n"
                            "    U = bw_params[8]\n"
                            "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                            "        x_1_arg = x_params_1 * current_x\n"
                            "        x_2_arg = x_params_2 * current_x\n"
                            "        x_vec_ind_1 = current_x + x_ind_1\n"
                            "        x_vec_ind_2 = current_x + x_ind_2\n"
                            "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                            "            y_1_arg = y_params_1 * current_y\n"
                            "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";

                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_1_ind = current_y + params[16]\n"
                                    "                arg_2_ind = current_y + params[22]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";                                                         
                                    break;
                                }
                            }
                        }
                        else{
                            code +=
                            "            s_1 = seqs_t[1][current_y + y_ind_1]\n"
                            "            s_2 = seqs_t[1][current_y + y_ind_2]\n"
                            "            arg_1_ind = current_y + params[16]\n"
                            "            arg_2_ind = current_y + params[22]\n"
                            "            mx_3_ind_1 = current_y + inds_mx3[0]\n"
                            "            mx_3_ind_2 = current_y + inds_mx3[2]\n"
                            "            mx_3_ind_3 = current_y + inds_mx3[5]\n"
                            "            for ind_y in range(0, check, 2):\n";              

                        }                
                        if(min_check == "1"){
                            code +=
                                "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";
                        }
                        else{
                            code +=
                                "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";                                    
                        }       
                        code += 
                            "        for i in range(check):\n"
                            "           if count >= outer_stop:\n"
                            "               break\n"                                
                            "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                            "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                            "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                            "        count += 1\n"
                            "        c1, c0, m_c1_x, m_c0_x, arg_1_c0, arg_1_c1, arg_2_c0, arg_2_c1, mx3_arg1_c0, mx3_arg1_c1, mx3_arg2_c0, mx3_arg2_c1, mx3_arg3_c0, mx3_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_1_c1, arg_1_c0, arg_2_c1, arg_2_c0, mx3_arg1_c1, mx3_arg1_c0, mx3_arg2_c1, mx3_arg2_c0, mx3_arg3_c1, mx3_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n";
                        break;
                    }
                    case 10:{
                        code +=
                            "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, H_col, H_col_mul, mat_col, mat_col_mul, V_col, V_col_mul, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                            "    count = 2\n"
                            "    a_1 = Vec(i16(args1[1]), i16, 16)\n"
                            "    b_1 = Vec(i16(args1[3]), i16, 16)\n"
                            "    am_1 = Vec(i16(args1[2]), i16, 16)\n"
                            "    x_ind_1 = args1[4]\n"
                            "    y_ind_1 = args1[5]\n"
                            "    a_3 = Vec(i16(args3[1]), i16, 16)\n"
                            "    b_3 = Vec(i16(args3[3]), i16, 16)\n"
                            "    am_3 = Vec(i16(args3[2]), i16, 16)\n"
                            "    x_ind_3 = args3[4]\n"
                            "    y_ind_3 = args3[5]\n"                        
                            "    SEQ_NO_Q = len(squery)\n"
                            "    REG_SIZE = 16\n"
                            "    c0 = 0\n"
                            "    c1 = 1\n"
                            "    m_c0_x = matrices[c0].arr.ptr\n"
                            "    m_c1_x = matrices[c1].arr.ptr\n"
                            "    v_c0_x = V[c0].arr.ptr\n"
                            "    v_c1_x = V[c1].arr.ptr\n"
                            "    h_c0_x = H[c0].arr.ptr\n"
                            "    h_c1_x = H[c1].arr.ptr\n"
                            "    mx2_arg2_vec = Vec(i16(inds_mx2[4]), i16, 16)\n"
                            "    mx2_arg3_vec = Vec(i16(inds_mx2[7]), i16, 16)\n"
                            "    mx2_arg1_c0 = (m_c0_x if inds_mx2[1] == -1 else m_c1_x) if mx2_names[0] == 0 else (v_c0_x if inds_mx2[1] == -1 else v_c1_x) if mx2_names[0] == 2 else (h_c0_x if inds_mx2[1] == -1 else h_c1_x)\n"
                            "    mx2_arg1_c1 = (m_c1_x if inds_mx2[1] == -1 else m_c0_x) if mx2_names[0] == 0 else (v_c1_x if inds_mx2[1] == -1 else v_c0_x) if mx2_names[0] == 2 else (h_c1_x if inds_mx2[1] == -1 else h_c0_x)\n"
                            "    mx2_arg2_c0 = (m_c0_x if inds_mx2[3] == -1 else m_c1_x) if mx2_names[1] == 0 else (v_c0_x if inds_mx2[3] == -1 else v_c1_x) if mx2_names[1] == 2 else (h_c0_x if inds_mx2[3] == -1 else h_c1_x)\n"
                            "    mx2_arg2_c1 = (m_c1_x if inds_mx2[3] == -1 else m_c0_x) if mx2_names[1] == 0 else (v_c1_x if inds_mx2[3] == -1 else v_c0_x) if mx2_names[1] == 2 else (h_c1_x if inds_mx2[3] == -1 else h_c0_x)\n"
                            "    mx2_arg3_c0 = (m_c0_x if inds_mx2[6] == -1 else m_c1_x) if mx2_names[2] == 0 else (v_c0_x if inds_mx2[6] == -1 else v_c1_x) if mx2_names[2] == 2 else (h_c0_x if inds_mx2[6] == -1 else h_c1_x)\n"
                            "    mx2_arg3_c1 = (m_c1_x if inds_mx2[6] == -1 else m_c0_x) if mx2_names[2] == 0 else (v_c1_x if inds_mx2[6] == -1 else v_c0_x) if mx2_names[2] == 2 else (h_c1_x if inds_mx2[6] == -1 else h_c0_x)\n"
                            "    arg_1_c0 = (m_c0_x if params[14] == -1 else m_c1_x) if first_list == 0 else (v_c0_x if params[14] == -1 else v_c1_x) if first_list == 2 else (h_c0_x if params[14] == -1 else h_c1_x)\n"
                            "    arg_1_c1 = (m_c1_x if params[14] == -1 else m_c0_x) if first_list == 0 else (v_c1_x if params[14] == -1 else v_c0_x) if first_list == 2 else (h_c1_x if params[14] == -1 else h_c0_x)\n"
                            "    if params[11] != -1:\n"
                            "        arg_3_c0 = (m_c0_x if params[28] == -1 else m_c1_x) if third_list == 0 else (v_c0_x if params[28] == -1 else v_c1_x) if third_list == 2 else (h_c0_x if params[28] == -1 else h_c1_x)\n"
                            "        arg_3_c1 = (m_c1_x if params[28] == -1 else m_c0_x) if third_list == 0 else (v_c1_x if params[28] == -1 else v_c0_x) if third_list == 2 else (h_c1_x if params[28] == -1 else h_c0_x)\n"
                            "    x_vec = x.arr.ptr\n"
                            "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                            "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                            "    L = bw_params[4]\n"
                            "    U = bw_params[8]\n"
                            "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                            "        x_1_arg = x_params_1 * current_x\n"
                            "        x_2_arg = x_params_2 * current_x\n"
                            "        x_vec_ind_1 = current_x + x_ind_1\n"
                            "        x_vec_ind_3 = current_x + x_ind_3\n"
                            "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                            "            y_1_arg = y_params_1 * current_y\n"
                            "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[14]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                                

                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[14]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                                                       
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_1_ind = current_y + params[14]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                        Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";
                            }                            
                            else{
                                code +=
                                "                        Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";                        
                            }
                        }
                        else{
                            code +=
                            "            s_1 = seqs_t[1][current_y + y_ind_1]\n"
                            "            s_3 = seqs_t[1][current_y + y_ind_3]\n"
                            "            arg_1_ind = current_y + params[14]\n"
                            "            arg_3_ind = current_y + params[26]\n"
                            "            mx_2_ind_1 = current_y + inds_mx2[0]\n"
                            "            mx_2_ind_2 = current_y + inds_mx2[2]\n"
                            "            mx_2_ind_3 = current_y + inds_mx2[5]\n"
                            "            for ind_y in range(0, check, 2):\n"
                            "                if params[11] != -1:\n";
                            if(min_check == "1"){
                                code +=
                                "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";
                            }                            
                            else{
                                code +=
                                "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";                        
                            }  

                        }        
                        code += 
                        "        for i in range(check):\n"
                        "           if count >= outer_stop:\n"
                        "               break\n"                                
                        "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                        "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                        "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                        "        count += 1\n"
                        "        c1, c0, m_c1_x, m_c0_x, arg_1_c0, arg_1_c1, mx2_arg1_c0, mx2_arg1_c1, mx2_arg2_c0, mx2_arg2_c1, mx2_arg3_c0, mx2_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_1_c1, arg_1_c0, mx2_arg1_c1, mx2_arg1_c0, mx2_arg2_c1, mx2_arg2_c0, mx2_arg3_c1, mx2_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n"
                        "        if params[11] != -1:\n"
                        "            arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0\n";
                        break;
                    }
                    case 11:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    a_1 = Vec(i16(args1[1]), i16, 16)\n"
                                "    b_1 = Vec(i16(args1[3]), i16, 16)\n"
                                "    am_1 = Vec(i16(args1[2]), i16, 16)\n"
                                "    x_ind_1 = args1[4]\n"
                                "    y_ind_1 = args1[5]\n"
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"

                                "    mx2_arg2_vec = Vec(i16(inds_mx2[4]), i16, 16)\n"
                                "    mx2_arg3_vec = Vec(i16(inds_mx2[7]), i16, 16)\n"
                                "    mx2_arg1_c0 = (m_c0_x if inds_mx2[1] == -1 else m_c1_x) if mx2_names[0] == 0 else (v_c0_x if inds_mx2[1] == -1 else v_c1_x) if mx2_names[0] == 2 else (h_c0_x if inds_mx2[1] == -1 else h_c1_x)\n"
                                "    mx2_arg1_c1 = (m_c1_x if inds_mx2[1] == -1 else m_c0_x) if mx2_names[0] == 0 else (v_c1_x if inds_mx2[1] == -1 else v_c0_x) if mx2_names[0] == 2 else (h_c1_x if inds_mx2[1] == -1 else h_c0_x)\n"
                                "    mx2_arg2_c0 = (m_c0_x if inds_mx2[3] == -1 else m_c1_x) if mx2_names[1] == 0 else (v_c0_x if inds_mx2[3] == -1 else v_c1_x) if mx2_names[1] == 2 else (h_c0_x if inds_mx2[3] == -1 else h_c1_x)\n"
                                "    mx2_arg2_c1 = (m_c1_x if inds_mx2[3] == -1 else m_c0_x) if mx2_names[1] == 0 else (v_c1_x if inds_mx2[3] == -1 else v_c0_x) if mx2_names[1] == 2 else (h_c1_x if inds_mx2[3] == -1 else h_c0_x)\n"
                                "    mx2_arg3_c0 = (m_c0_x if inds_mx2[6] == -1 else m_c1_x) if mx2_names[2] == 0 else (v_c0_x if inds_mx2[6] == -1 else v_c1_x) if mx2_names[2] == 2 else (h_c0_x if inds_mx2[6] == -1 else h_c1_x)\n"
                                "    mx2_arg3_c1 = (m_c1_x if inds_mx2[6] == -1 else m_c0_x) if mx2_names[2] == 0 else (v_c1_x if inds_mx2[6] == -1 else v_c0_x) if mx2_names[2] == 2 else (h_c1_x if inds_mx2[6] == -1 else h_c0_x)\n"

                                "    mx3_arg2_vec = Vec(i16(inds_mx3[4]), i16, 16)\n"
                                "    mx3_arg3_vec = Vec(i16(inds_mx3[7]), i16, 16)\n"
                                "    mx3_arg1_c0 = (m_c0_x if inds_mx3[1] == -1 else m_c1_x) if mx3_names[0] == 0 else (v_c0_x if inds_mx3[1] == -1 else v_c1_x) if mx3_names[0] == 2 else (h_c0_x if inds_mx3[1] == -1 else h_c1_x)\n"
                                "    mx3_arg1_c1 = (m_c1_x if inds_mx3[1] == -1 else m_c0_x) if mx3_names[0] == 0 else (v_c1_x if inds_mx3[1] == -1 else v_c0_x) if mx3_names[0] == 2 else (h_c1_x if inds_mx3[1] == -1 else h_c0_x)\n" 
                                "    mx3_arg2_c0 = (m_c0_x if inds_mx3[3] == -1 else m_c1_x) if mx3_names[1] == 0 else (v_c0_x if inds_mx3[3] == -1 else v_c1_x) if mx3_names[1] == 2 else (h_c0_x if inds_mx3[3] == -1 else h_c1_x)\n"      
                                "    mx3_arg2_c1 = (m_c1_x if inds_mx3[3] == -1 else m_c0_x) if mx3_names[1] == 0 else (v_c1_x if inds_mx3[3] == -1 else v_c0_x) if mx3_names[1] == 2 else (h_c1_x if inds_mx3[3] == -1 else h_c0_x)\n"
                                "    mx3_arg3_c0 = (m_c0_x if inds_mx3[6] == -1 else m_c1_x) if mx3_names[2] == 0 else (v_c0_x if inds_mx3[6] == -1 else v_c1_x) if mx3_names[2] == 2 else (h_c0_x if inds_mx3[6] == -1 else h_c1_x)\n"
                                "    mx3_arg3_c1 = (m_c1_x if inds_mx3[6] == -1 else m_c0_x) if mx3_names[2] == 0 else (v_c1_x if inds_mx3[6] == -1 else v_c0_x) if mx3_names[2] == 2 else (h_c1_x if inds_mx3[6] == -1 else h_c0_x)\n"
                                            
                                "    arg_1_c0 = (m_c0_x if params[14] == -1 else m_c1_x) if first_list == 0 else (v_c0_x if params[14] == -1 else v_c1_x) if first_list == 2 else (h_c0_x if params[14] == -1 else h_c1_x)\n"
                                "    arg_1_c1 = (m_c1_x if params[14] == -1 else m_c0_x) if first_list == 0 else (v_c1_x if params[14] == -1 else v_c0_x) if first_list == 2 else (h_c1_x if params[14] == -1 else h_c0_x)\n"

                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"                            
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        x_vec_ind_1 = current_x + x_ind_1\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code += 
                                        "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                        "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                        "                    for i_y in range(check):\n"
                                        "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                        "            else:\n"
                                        "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                        "                arg_1_ind = current_y + params[14]\n"  
                                        "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                        "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                        "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                        "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                        "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                        "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                        "                for ind_y in range(0, check, 2):\n";                                    
                                    break;
                                }
                                case 1:{
                                    code += 
                                        "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                        "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                        "                    for i_y in range(check):\n"
                                        "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                        "            else:\n"
                                        "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                        "                arg_1_ind = current_y + params[14]\n"  
                                        "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                        "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                        "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                        "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                        "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                        "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                        "                for ind_y in range(0, check, 2):\n";                                                               
                                    break;
                                }
                                case 0:{
                                    code += 
                                        "            if (x_1_arg + y_1_arg) <= L:\n"
                                        "                if (x_1_arg + y_1_arg) == L:\n"
                                        "                    for i_y in range(check):\n"
                                        "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                        "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                        "            else:\n"
                                        "                s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                        "                arg_1_ind = current_y + params[14]\n"  
                                        "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                        "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                        "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                        "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                        "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                        "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                        "                for ind_y in range(0, check, 2):\n";                                                              
                                    break;
                                }
                            }
                        }
                        else{
                            code += 
                                "            s_1 = seqs_t[1][current_y + y_ind_1]\n"
                                "            arg_1_ind = current_y + params[14]\n"  
                                "            mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                "            mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                "            mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                "            mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                "            mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                "            mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                "            for ind_y in range(0, check, 2):\n";                                                     

                        }
                        if(min_check == "1"){
                            code += 
                            "                    Vec.max_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";                                    
                        }
                        else{
                            code += 
                            "                    Vec.min_2(((arg_1_c0[arg_1_ind][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], ((arg_1_c0[arg_1_ind][ind_y + 1] * i16(params[9])) + match_vec(checker_1, x_vec[x_vec_ind_1], s_1[ind_y + 1], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";                                                            
                        }
                        code += 
                                "        for i in range(check):\n"
                                "           if count >= outer_stop:\n"
                                "               break\n"                                
                                "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                                "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                                "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                                "        count += 1\n"
                                "        c1, c0, m_c1_x, m_c0_x, arg_1_c0, arg_1_c1, mx2_arg1_c0, mx2_arg1_c1, mx2_arg2_c0, mx2_arg2_c1, mx2_arg3_c0, mx2_arg3_c1, mx3_arg1_c0, mx3_arg1_c1, mx3_arg2_c0, mx3_arg2_c1, mx3_arg3_c0, mx3_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_1_c1, arg_1_c0, mx2_arg1_c1, mx2_arg1_c0, mx2_arg2_c1, mx2_arg2_c0, mx2_arg3_c1, mx2_arg3_c0, mx3_arg1_c1, mx3_arg1_c0, mx3_arg2_c1, mx3_arg2_c0, mx3_arg3_c1, mx3_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n";
                        break;
                    }
                    case 100:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    a_2 = Vec(i16(args2[1]), i16, 16)\n"
                                "    b_2 = Vec(i16(args2[3]), i16, 16)\n"
                                "    am_2 = Vec(i16(args2[2]), i16, 16)\n"
                                "    x_ind_2 = args2[4]\n"
                                "    y_ind_2 = args2[5]\n"
                                "    a_3 = Vec(i16(args3[1]), i16, 16)\n"
                                "    b_3 = Vec(i16(args3[3]), i16, 16)\n"
                                "    am_3 = Vec(i16(args3[2]), i16, 16)\n"
                                "    x_ind_3 = args3[4]\n"
                                "    y_ind_3 = args3[5]\n"                        
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"
                                "    mx1_arg2_vec = Vec(i16(inds_mx1[4]), i16, 16)\n"
                                "    mx1_arg3_vec = Vec(i16(inds_mx1[7]), i16, 16)\n"
                                "    mx1_arg1_c0 = (m_c0_x if inds_mx1[1] == -1 else m_c1_x) if mx1_names[0] == 0 else (v_c0_x if inds_mx1[1] == -1 else v_c1_x) if mx1_names[0] == 2 else (h_c0_x if inds_mx1[1] == -1 else h_c1_x)\n"
                                "    mx1_arg1_c1 = (m_c1_x if inds_mx1[1] == -1 else m_c0_x) if mx1_names[0] == 0 else (v_c1_x if inds_mx1[1] == -1 else v_c0_x) if mx1_names[0] == 2 else (h_c1_x if inds_mx1[1] == -1 else h_c0_x)\n"
                                "    mx1_arg2_c0 = (m_c0_x if inds_mx1[3] == -1 else m_c1_x) if mx1_names[1] == 0 else (v_c0_x if inds_mx1[3] == -1 else v_c1_x) if mx1_names[1] == 2 else (h_c0_x if inds_mx1[3] == -1 else h_c1_x)\n"
                                "    mx1_arg2_c1 = (m_c1_x if inds_mx1[3] == -1 else m_c0_x) if mx1_names[1] == 0 else (v_c1_x if inds_mx1[3] == -1 else v_c0_x) if mx1_names[1] == 2 else (h_c1_x if inds_mx1[3] == -1 else h_c0_x)\n"
                                "    mx1_arg3_c0 = (m_c0_x if inds_mx1[6] == -1 else m_c1_x) if mx1_names[2] == 0 else (v_c0_x if inds_mx1[6] == -1 else v_c1_x) if mx1_names[2] == 2 else (h_c0_x if inds_mx1[6] == -1 else h_c1_x)\n"
                                "    mx1_arg3_c1 = (m_c1_x if inds_mx1[6] == -1 else m_c0_x) if mx1_names[2] == 0 else (v_c1_x if inds_mx1[6] == -1 else v_c0_x) if mx1_names[2] == 2 else (h_c1_x if inds_mx1[6] == -1 else h_c0_x)\n"
                                "    arg_2_c0 = (m_c0_x if params[20] == -1 else m_c1_x) if second_list == 0 else (v_c0_x if params[20] == -1 else v_c1_x) if second_list == 2 else (h_c0_x if params[20] == -1 else h_c1_x)\n"
                                "    arg_2_c1 = (m_c1_x if params[20] == -1 else m_c0_x) if second_list == 0 else (v_c1_x if params[20] == -1 else v_c0_x) if second_list == 2 else (h_c1_x if params[20] == -1 else h_c0_x)\n"
                                "    if params[11] != -1:\n"
                                "        arg_3_c0 = (m_c0_x if params[28] == -1 else m_c1_x) if third_list == 0 else (v_c0_x if params[28] == -1 else v_c1_x) if third_list == 2 else (h_c0_x if params[28] == -1 else h_c1_x)\n"
                                "        arg_3_c1 = (m_c1_x if params[28] == -1 else m_c0_x) if third_list == 0 else (v_c1_x if params[28] == -1 else v_c0_x) if third_list == 2 else (h_c1_x if params[28] == -1 else h_c0_x)\n"
                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        x_vec_ind_2 = current_x + x_ind_2\n"
                                "        x_vec_ind_3 = current_x + x_ind_3\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";

                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                          
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                                                          
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                        Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";                                              
                            }
                            else{
                                code +=
                                "                        Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                    else:\n"
                                "                        Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";                                     
                            }
                        }
                        else{ 
                            code +=
                            "            s_2 = seqs_t[1][current_y + y_ind_2]\n"
                            "            s_3 = seqs_t[1][current_y + y_ind_3]\n"
                            "            arg_2_ind = current_y + params[20]\n"
                            "            arg_3_ind = current_y + params[26]\n"
                            "            mx_1_ind_1 = current_y + inds_mx1[0]\n"
                            "            mx_1_ind_2 = current_y + inds_mx1[2]\n"
                            "            mx_1_ind_3 = current_y + inds_mx1[5]\n"
                            "            for ind_y in range(0, check, 2):\n"
                            "                if params[11] != -1:\n";                                   
                            if(min_check == "1"){
                                code +=
                                "                    Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";                                              
                            }
                            else{
                                code +=
                                "                    Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)) * i16(params[24]))\n"
                                "                else:\n"
                                "                    Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]))\n";             
                            }     

                        }
                        code +=
                        "        for i in range(check):\n"
                        "           if count >= outer_stop:\n"
                        "               break\n"                                
                        "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                        "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                        "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                        "        count += 1\n"
                        "        c1, c0, m_c1_x, m_c0_x, arg_2_c0, arg_2_c1, mx1_arg1_c0, mx1_arg1_c1, mx1_arg2_c0, mx1_arg2_c1, mx1_arg3_c0, mx1_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_2_c1, arg_2_c0, mx1_arg1_c1, mx1_arg1_c0, mx1_arg2_c1, mx1_arg2_c0, mx1_arg3_c1, mx1_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n"
                        "        if params[11] != -1:\n"
                        "            arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0\n";
                        break;
                    }
                    case 101:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    a_2 = Vec(i16(args2[1]), i16, 16)\n"
                                "    b_2 = Vec(i16(args2[3]), i16, 16)\n"
                                "    am_2 = Vec(i16(args2[2]), i16, 16)\n"
                                "    x_ind_2 = args2[4]\n"
                                "    y_ind_2 = args2[5]\n"                       
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"
                                "    mx1_arg2_vec = Vec(i16(inds_mx1[4]), i16, 16)\n"
                                "    mx1_arg3_vec = Vec(i16(inds_mx1[7]), i16, 16)\n"
                                "    mx1_arg1_c0 = (m_c0_x if inds_mx1[1] == -1 else m_c1_x) if mx1_names[0] == 0 else (v_c0_x if inds_mx1[1] == -1 else v_c1_x) if mx1_names[0] == 2 else (h_c0_x if inds_mx1[1] == -1 else h_c1_x)\n"
                                "    mx1_arg1_c1 = (m_c1_x if inds_mx1[1] == -1 else m_c0_x) if mx1_names[0] == 0 else (v_c1_x if inds_mx1[1] == -1 else v_c0_x) if mx1_names[0] == 2 else (h_c1_x if inds_mx1[1] == -1 else h_c0_x)\n"
                                "    mx1_arg2_c0 = (m_c0_x if inds_mx1[3] == -1 else m_c1_x) if mx1_names[1] == 0 else (v_c0_x if inds_mx1[3] == -1 else v_c1_x) if mx1_names[1] == 2 else (h_c0_x if inds_mx1[3] == -1 else h_c1_x)\n"
                                "    mx1_arg2_c1 = (m_c1_x if inds_mx1[3] == -1 else m_c0_x) if mx1_names[1] == 0 else (v_c1_x if inds_mx1[3] == -1 else v_c0_x) if mx1_names[1] == 2 else (h_c1_x if inds_mx1[3] == -1 else h_c0_x)\n"
                                "    mx1_arg3_c0 = (m_c0_x if inds_mx1[6] == -1 else m_c1_x) if mx1_names[2] == 0 else (v_c0_x if inds_mx1[6] == -1 else v_c1_x) if mx1_names[2] == 2 else (h_c0_x if inds_mx1[6] == -1 else h_c1_x)\n"
                                "    mx1_arg3_c1 = (m_c1_x if inds_mx1[6] == -1 else m_c0_x) if mx1_names[2] == 0 else (v_c1_x if inds_mx1[6] == -1 else v_c0_x) if mx1_names[2] == 2 else (h_c1_x if inds_mx1[6] == -1 else h_c0_x)\n"
                                "    mx3_arg2_vec = Vec(i16(inds_mx3[4]), i16, 16)\n"
                                "    mx3_arg3_vec = Vec(i16(inds_mx3[7]), i16, 16)\n"
                                "    mx3_arg1_c0 = (m_c0_x if inds_mx3[1] == -1 else m_c1_x) if mx3_names[0] == 0 else (v_c0_x if inds_mx3[1] == -1 else v_c1_x) if mx3_names[0] == 2 else (h_c0_x if inds_mx3[1] == -1 else h_c1_x)\n"
                                "    mx3_arg1_c1 = (m_c1_x if inds_mx3[1] == -1 else m_c0_x) if mx3_names[0] == 0 else (v_c1_x if inds_mx3[1] == -1 else v_c0_x) if mx3_names[0] == 2 else (h_c1_x if inds_mx3[1] == -1 else h_c0_x)\n"
                                "    mx3_arg2_c0 = (m_c0_x if inds_mx3[3] == -1 else m_c1_x) if mx3_names[1] == 0 else (v_c0_x if inds_mx3[3] == -1 else v_c1_x) if mx3_names[1] == 2 else (h_c0_x if inds_mx3[3] == -1 else h_c1_x)\n"
                                "    mx3_arg2_c1 = (m_c1_x if inds_mx3[3] == -1 else m_c0_x) if mx3_names[1] == 0 else (v_c1_x if inds_mx3[3] == -1 else v_c0_x) if mx3_names[1] == 2 else (h_c1_x if inds_mx3[3] == -1 else h_c0_x)\n"
                                "    mx3_arg3_c0 = (m_c0_x if inds_mx3[6] == -1 else m_c1_x) if mx3_names[2] == 0 else (v_c0_x if inds_mx3[6] == -1 else v_c1_x) if mx3_names[2] == 2 else (h_c0_x if inds_mx3[6] == -1 else h_c1_x)\n"
                                "    mx3_arg3_c1 = (m_c1_x if inds_mx3[6] == -1 else m_c0_x) if mx3_names[2] == 0 else (v_c1_x if inds_mx3[6] == -1 else v_c0_x) if mx3_names[2] == 2 else (h_c1_x if inds_mx3[6] == -1 else h_c0_x)\n"
                                "    arg_2_c0 = (m_c0_x if params[20] == -1 else m_c1_x) if second_list == 0 else (v_c0_x if params[20] == -1 else v_c1_x) if second_list == 2 else (h_c0_x if params[20] == -1 else h_c1_x)\n"
                                "    arg_2_c1 = (m_c1_x if params[20] == -1 else m_c0_x) if second_list == 0 else (v_c1_x if params[20] == -1 else v_c0_x) if second_list == 2 else (h_c1_x if params[20] == -1 else h_c0_x)\n"
                                "    if params[11] != -1:\n"
                                "        arg_3_c0 = (m_c0_x if params[28] == -1 else m_c1_x) if third_list == 0 else (v_c0_x if params[28] == -1 else v_c1_x) if third_list == 2 else (h_c0_x if params[28] == -1 else h_c1_x)\n"
                                "        arg_3_c1 = (m_c1_x if params[28] == -1 else m_c0_x) if third_list == 0 else (v_c1_x if params[28] == -1 else v_c0_x) if third_list == 2 else (h_c1_x if params[28] == -1 else h_c0_x)\n"
                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        x_vec_ind_2 = current_x + x_ind_2\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code += 
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";

                                    break;
                                }
                                case 1:{
                                    code += 
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";                                
                            
                                    break;
                                }
                                case 0:{
                                    code += 
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_2 = seqs_t[1][current_y + y_ind_2]\n"
                                    "                arg_2_ind = current_y + params[20]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(0, check, 2):\n";                         
                                    break;
                                }
                            }
                        }
                        else{                 
                            code += 
                            "            s_2 = seqs_t[1][current_y + y_ind_2]\n"
                            "            arg_2_ind = current_y + params[20]\n"
                            "            mx_1_ind_1 = current_y + inds_mx1[0]\n"
                            "            mx_1_ind_2 = current_y + inds_mx1[2]\n"
                            "            mx_1_ind_3 = current_y + inds_mx1[5]\n"
                            "            mx_3_ind_1 = current_y + inds_mx3[0]\n"
                            "            mx_3_ind_2 = current_y + inds_mx3[2]\n"
                            "            mx_3_ind_3 = current_y + inds_mx3[5]\n"
                            "            for ind_y in range(0, check, 2):\n";
                        }      
                        if(min_check == "1"){
                            code +=
                            "                         Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";                                             
                        }
                        else{             
                            code +=
                            "                         Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), ((arg_2_c0[arg_2_ind][ind_y + 1] * i16(params[10])) + match_vec(checker_2, x_vec[x_vec_ind_2], s_2[ind_y + 1], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";                                                                     
                        }
                        code +=
                        "        for i in range(check):\n"
                        "           if count >= outer_stop:\n"
                        "               break\n"                                
                        "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                        "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                        "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                        "        count += 1\n"
                        "        c1, c0, m_c1_x, m_c0_x, arg_2_c0, arg_2_c1, mx1_arg1_c0, mx1_arg1_c1, mx1_arg2_c0, mx1_arg2_c1, mx1_arg3_c0, mx1_arg3_c1, mx3_arg1_c0, mx3_arg1_c1, mx3_arg2_c0, mx3_arg2_c1, mx3_arg3_c0, mx3_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, arg_2_c1, arg_2_c0, mx1_arg1_c1, mx1_arg1_c0, mx1_arg2_c1, mx1_arg2_c0, mx1_arg3_c1, mx1_arg3_c0, mx3_arg1_c1, mx3_arg1_c0, mx3_arg2_c1, mx3_arg2_c0, mx3_arg3_c1, mx3_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n";
                        break;
                    }
                    case 110:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    a_3 = Vec(i16(args3[1]), i16, 16)\n"
                                "    b_3 = Vec(i16(args3[3]), i16, 16)\n"
                                "    am_3 = Vec(i16(args3[2]), i16, 16)\n"
                                "    x_ind_3 = args3[4]\n"
                                "    y_ind_3 = args3[5]\n"
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"
                                "    mx1_arg2_vec = Vec(i16(inds_mx1[4]), i16, 16)\n"
                                "    mx1_arg3_vec = Vec(i16(inds_mx1[7]), i16, 16)\n"
                                "    mx1_arg1_c0 = (m_c0_x if inds_mx1[1] == -1 else m_c1_x) if mx1_names[0] == 0 else (v_c0_x if inds_mx1[1] == -1 else v_c1_x) if mx1_names[0] == 2 else (h_c0_x if inds_mx1[1] == -1 else h_c1_x)\n"
                                "    mx1_arg1_c1 = (m_c1_x if inds_mx1[1] == -1 else m_c0_x) if mx1_names[0] == 0 else (v_c1_x if inds_mx1[1] == -1 else v_c0_x) if mx1_names[0] == 2 else (h_c1_x if inds_mx1[1] == -1 else h_c0_x)\n"
                                "    mx1_arg2_c0 = (m_c0_x if inds_mx1[3] == -1 else m_c1_x) if mx1_names[1] == 0 else (v_c0_x if inds_mx1[3] == -1 else v_c1_x) if mx1_names[1] == 2 else (h_c0_x if inds_mx1[3] == -1 else h_c1_x)\n"
                                "    mx1_arg2_c1 = (m_c1_x if inds_mx1[3] == -1 else m_c0_x) if mx1_names[1] == 0 else (v_c1_x if inds_mx1[3] == -1 else v_c0_x) if mx1_names[1] == 2 else (h_c1_x if inds_mx1[3] == -1 else h_c0_x)\n"
                                "    mx1_arg3_c0 = (m_c0_x if inds_mx1[6] == -1 else m_c1_x) if mx1_names[2] == 0 else (v_c0_x if inds_mx1[6] == -1 else v_c1_x) if mx1_names[2] == 2 else (h_c0_x if inds_mx1[6] == -1 else h_c1_x)\n"
                                "    mx1_arg3_c1 = (m_c1_x if inds_mx1[6] == -1 else m_c0_x) if mx1_names[2] == 0 else (v_c1_x if inds_mx1[6] == -1 else v_c0_x) if mx1_names[2] == 2 else (h_c1_x if inds_mx1[6] == -1 else h_c0_x)\n"
                                "    mx2_arg2_vec = Vec(i16(inds_mx2[4]), i16, 16)\n"
                                "    mx2_arg3_vec = Vec(i16(inds_mx2[7]), i16, 16)\n"
                                "    mx2_arg1_c0 = (m_c0_x if inds_mx2[1] == -1 else m_c1_x) if mx2_names[0] == 0 else (v_c0_x if inds_mx2[1] == -1 else v_c1_x) if mx2_names[0] == 2 else (h_c0_x if inds_mx2[1] == -1 else h_c1_x)\n"
                                "    mx2_arg1_c1 = (m_c1_x if inds_mx2[1] == -1 else m_c0_x) if mx2_names[0] == 0 else (v_c1_x if inds_mx2[1] == -1 else v_c0_x) if mx2_names[0] == 2 else (h_c1_x if inds_mx2[1] == -1 else h_c0_x)\n"
                                "    mx2_arg2_c0 = (m_c0_x if inds_mx2[3] == -1 else m_c1_x) if mx2_names[1] == 0 else (v_c0_x if inds_mx2[3] == -1 else v_c1_x) if mx2_names[1] == 2 else (h_c0_x if inds_mx2[3] == -1 else h_c1_x)\n"
                                "    mx2_arg2_c1 = (m_c1_x if inds_mx2[3] == -1 else m_c0_x) if mx2_names[1] == 0 else (v_c1_x if inds_mx2[3] == -1 else v_c0_x) if mx2_names[1] == 2 else (h_c1_x if inds_mx2[3] == -1 else h_c0_x)\n"
                                "    mx2_arg3_c0 = (m_c0_x if inds_mx2[6] == -1 else m_c1_x) if mx2_names[2] == 0 else (v_c0_x if inds_mx2[6] == -1 else v_c1_x) if mx2_names[2] == 2 else (h_c0_x if inds_mx2[6] == -1 else h_c1_x)\n"
                                "    mx2_arg3_c1 = (m_c1_x if inds_mx2[6] == -1 else m_c0_x) if mx2_names[2] == 0 else (v_c1_x if inds_mx2[6] == -1 else v_c0_x) if mx2_names[2] == 2 else (h_c1_x if inds_mx2[6] == -1 else h_c0_x)\n"
                                "    if params[11] != -1:\n"
                                "        arg_3_c0 = (m_c0_x if params[28] == -1 else m_c1_x) if third_list == 0 else (v_c0_x if params[28] == -1 else v_c1_x) if third_list == 2 else (h_c0_x if params[28] == -1 else h_c1_x)\n"
                                "        arg_3_c1 = (m_c1_x if params[28] == -1 else m_c0_x) if third_list == 0 else (v_c1_x if params[28] == -1 else v_c0_x) if third_list == 2 else (h_c1_x if params[28] == -1 else h_c0_x)\n"
                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        x_vec_ind_3 = current_x + x_ind_3\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                                
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                s_3 = seqs_t[1][current_y + y_ind_3]\n"
                                    "                arg_3_ind = current_y + params[26]\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                for ind_y in range(0, check, 2):\n"
                                    "                    if params[11] != -1:\n";                       
                                    break;
                                }
                            }
                            if(min_check == "1"){   
                                code +=
                                "                        Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)))\n"
                                "                    else:\n"
                                "                        Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";
                            }
                            else{     
                                code +=
                                "                        Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)))\n"
                                "                    else:\n"
                                "                        Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";                               
                            }                        
                        }
                        else{
                            code +=
                            "            s_3 = seqs_t[1][current_y + y_ind_3]\n"
                            "            arg_3_ind = current_y + params[26]\n"
                            "            mx_1_ind_1 = current_y + inds_mx1[0]\n"
                            "            mx_1_ind_2 = current_y + inds_mx1[2]\n"
                            "            mx_1_ind_3 = current_y + inds_mx1[5]\n"
                            "            mx_2_ind_1 = current_y + inds_mx2[0]\n"
                            "            mx_2_ind_2 = current_y + inds_mx2[2]\n"
                            "            mx_2_ind_3 = current_y + inds_mx2[5]\n"
                            "            for ind_y in range(0, check, 2):\n"
                            "                if params[11] != -1:\n";                
                            if(min_check == "1"){   
                                code +=
                                "                    Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)))\n"
                                "                else:\n"
                                "                    Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";
                            }
                            else{     
                                code +=
                                "                    Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), ((arg_3_c0[arg_3_ind][ind_y + 1] * i16(params[11])) + match_vec(checker_3, x_vec[x_vec_ind_3], s_3[ind_y + 1], a_3, b_3, am_3, params[29], n)))\n"
                                "                else:\n"
                                "                    Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)))\n";                               
                            }                        
                        }   

                        code +=
                        "        for i in range(check):\n"
                        "           if count >= outer_stop:\n"
                        "               break\n"                                
                        "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                        "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                        "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                        "        count += 1\n"
                        "        c1, c0, m_c1_x, m_c0_x, mx1_arg1_c0, mx1_arg1_c1, mx1_arg2_c0, mx1_arg2_c1, mx1_arg3_c0, mx1_arg3_c1, mx2_arg1_c0, mx2_arg1_c1, mx2_arg2_c0, mx2_arg2_c1, mx2_arg3_c0, mx2_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, mx1_arg1_c1, mx1_arg1_c0, mx1_arg2_c1, mx1_arg2_c0, mx1_arg3_c1, mx1_arg3_c0, mx2_arg1_c1, mx2_arg1_c0, mx2_arg2_c1, mx2_arg2_c0, mx2_arg3_c1, mx2_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n"
                        "        if params[11] != -1:\n"
                        "            arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0\n";
                        break;
                    }
                    case 111:{
                        code +=
                                "def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv):\n"
                                "    count = 2\n"
                                "    SEQ_NO_Q = len(squery)\n"
                                "    REG_SIZE = 16\n"
                                "    c0 = 0\n"
                                "    c1 = 1\n"
                                "    m_c0_x = matrices[c0].arr.ptr\n"
                                "    m_c1_x = matrices[c1].arr.ptr\n"
                                "    v_c0_x = V[c0].arr.ptr\n"
                                "    v_c1_x = V[c1].arr.ptr\n"
                                "    h_c0_x = H[c0].arr.ptr\n"
                                "    h_c1_x = H[c1].arr.ptr\n"
                                "    mx1_arg2_vec = Vec(i16(inds_mx1[4]), i16, 16)\n"
                                "    mx1_arg3_vec = Vec(i16(inds_mx1[7]), i16, 16)\n"
                                "    mx1_arg1_c0 = (m_c0_x if inds_mx1[1] == -1 else m_c1_x) if mx1_names[0] == 0 else (v_c0_x if inds_mx1[1] == -1 else v_c1_x) if mx1_names[0] == 2 else (h_c0_x if inds_mx1[1] == -1 else h_c1_x)\n"
                                "    mx1_arg1_c1 = (m_c1_x if inds_mx1[1] == -1 else m_c0_x) if mx1_names[0] == 0 else (v_c1_x if inds_mx1[1] == -1 else v_c0_x) if mx1_names[0] == 2 else (h_c1_x if inds_mx1[1] == -1 else h_c0_x)\n"
                                "    mx1_arg2_c0 = (m_c0_x if inds_mx1[3] == -1 else m_c1_x) if mx1_names[1] == 0 else (v_c0_x if inds_mx1[3] == -1 else v_c1_x) if mx1_names[1] == 2 else (h_c0_x if inds_mx1[3] == -1 else h_c1_x)\n"
                                "    mx1_arg2_c1 = (m_c1_x if inds_mx1[3] == -1 else m_c0_x) if mx1_names[1] == 0 else (v_c1_x if inds_mx1[3] == -1 else v_c0_x) if mx1_names[1] == 2 else (h_c1_x if inds_mx1[3] == -1 else h_c0_x)\n"
                                "    mx1_arg3_c0 = (m_c0_x if inds_mx1[6] == -1 else m_c1_x) if mx1_names[2] == 0 else (v_c0_x if inds_mx1[6] == -1 else v_c1_x) if mx1_names[2] == 2 else (h_c0_x if inds_mx1[6] == -1 else h_c1_x)\n"
                                "    mx1_arg3_c1 = (m_c1_x if inds_mx1[6] == -1 else m_c0_x) if mx1_names[2] == 0 else (v_c1_x if inds_mx1[6] == -1 else v_c0_x) if mx1_names[2] == 2 else (h_c1_x if inds_mx1[6] == -1 else h_c0_x)\n"
                                "    mx2_arg2_vec = Vec(i16(inds_mx2[4]), i16, 16)\n"
                                "    mx2_arg3_vec = Vec(i16(inds_mx2[7]), i16, 16)\n"
                                "    mx2_arg1_c0 = (m_c0_x if inds_mx2[1] == -1 else m_c1_x) if mx2_names[0] == 0 else (v_c0_x if inds_mx2[1] == -1 else v_c1_x) if mx2_names[0] == 2 else (h_c0_x if inds_mx2[1] == -1 else h_c1_x)\n"
                                "    mx2_arg1_c1 = (m_c1_x if inds_mx2[1] == -1 else m_c0_x) if mx2_names[0] == 0 else (v_c1_x if inds_mx2[1] == -1 else v_c0_x) if mx2_names[0] == 2 else (h_c1_x if inds_mx2[1] == -1 else h_c0_x)\n"
                                "    mx2_arg2_c0 = (m_c0_x if inds_mx2[3] == -1 else m_c1_x) if mx2_names[1] == 0 else (v_c0_x if inds_mx2[3] == -1 else v_c1_x) if mx2_names[1] == 2 else (h_c0_x if inds_mx2[3] == -1 else h_c1_x)\n"
                                "    mx2_arg2_c1 = (m_c1_x if inds_mx2[3] == -1 else m_c0_x) if mx2_names[1] == 0 else (v_c1_x if inds_mx2[3] == -1 else v_c0_x) if mx2_names[1] == 2 else (h_c1_x if inds_mx2[3] == -1 else h_c0_x)\n"
                                "    mx2_arg3_c0 = (m_c0_x if inds_mx2[6] == -1 else m_c1_x) if mx2_names[2] == 0 else (v_c0_x if inds_mx2[6] == -1 else v_c1_x) if mx2_names[2] == 2 else (h_c0_x if inds_mx2[6] == -1 else h_c1_x)\n"
                                "    mx2_arg3_c1 = (m_c1_x if inds_mx2[6] == -1 else m_c0_x) if mx2_names[2] == 0 else (v_c1_x if inds_mx2[6] == -1 else v_c0_x) if mx2_names[2] == 2 else (h_c1_x if inds_mx2[6] == -1 else h_c0_x)\n"
                                "    mx3_arg2_vec = Vec(i16(inds_mx3[4]), i16, 16)\n"
                                "    mx3_arg3_vec = Vec(i16(inds_mx3[7]), i16, 16)\n"
                                "    mx3_arg1_c0 = (m_c0_x if inds_mx3[1] == -1 else m_c1_x) if mx3_names[0] == 0 else (v_c0_x if inds_mx3[1] == -1 else v_c1_x) if mx3_names[0] == 2 else (h_c0_x if inds_mx3[1] == -1 else h_c1_x)\n"
                                "    mx3_arg1_c1 = (m_c1_x if inds_mx3[1] == -1 else m_c0_x) if mx3_names[0] == 0 else (v_c1_x if inds_mx3[1] == -1 else v_c0_x) if mx3_names[0] == 2 else (h_c1_x if inds_mx3[1] == -1 else h_c0_x)\n"
                                "    mx3_arg2_c0 = (m_c0_x if inds_mx3[3] == -1 else m_c1_x) if mx3_names[1] == 0 else (v_c0_x if inds_mx3[3] == -1 else v_c1_x) if mx3_names[1] == 2 else (h_c0_x if inds_mx3[3] == -1 else h_c1_x)\n"
                                "    mx3_arg2_c1 = (m_c1_x if inds_mx3[3] == -1 else m_c0_x) if mx3_names[1] == 0 else (v_c1_x if inds_mx3[3] == -1 else v_c0_x) if mx3_names[1] == 2 else (h_c1_x if inds_mx3[3] == -1 else h_c0_x)\n"
                                "    mx3_arg3_c0 = (m_c0_x if inds_mx3[6] == -1 else m_c1_x) if mx3_names[2] == 0 else (v_c0_x if inds_mx3[6] == -1 else v_c1_x) if mx3_names[2] == 2 else (h_c0_x if inds_mx3[6] == -1 else h_c1_x)\n"
                                "    mx3_arg3_c1 = (m_c1_x if inds_mx3[6] == -1 else m_c0_x) if mx3_names[2] == 0 else (v_c1_x if inds_mx3[6] == -1 else v_c0_x) if mx3_names[2] == 2 else (h_c1_x if inds_mx3[6] == -1 else h_c0_x)\n"
                                "    x_vec = x.arr.ptr\n"
                                "    n = Vec(u8(ord(\"N\")), u8, 16)\n"
                                "    infinitizer = Vec(i16(-10000), i16, 16)\n"
                                "    L = bw_params[4]\n"
                                "    U = bw_params[8]\n"
                                "    for current_x in range(outer_start, outer_stop, outer_step):\n"
                                "        x_1_arg = x_params_1 * current_x\n"
                                "        x_2_arg = x_params_2 * current_x\n"
                                "        for current_y in range(inner_start, inner_stop, inner_step):\n"
                                "            y_1_arg = y_params_1 * current_y\n"
                                "            y_2_arg = y_params_2 * current_y\n";
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(check):\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                    "                if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(check):\n";                                                            
                                    break;
                                }
                                case 0:{
                                    code +=
                                    "            if (x_1_arg + y_1_arg) <= L:\n"
                                    "                if (x_1_arg + y_1_arg) == L:\n"
                                    "                    for i_y in range(check):\n"
                                    "                        m_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        h_c1_x[current_y][i_y] = infinitizer\n"
                                    "                        v_c1_x[current_y][i_y] = infinitizer\n"
                                    "            else:\n"
                                    "                mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "                mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "                mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "                mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "                mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "                mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "                mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "                mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "                mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "                for ind_y in range(check):\n";                       
                                    break;
                                }
                            }
                        }
                        else{
                                    code +=
                                    "             mx_1_ind_1 = current_y + inds_mx1[0]\n"
                                    "             mx_1_ind_2 = current_y + inds_mx1[2]\n"
                                    "             mx_1_ind_3 = current_y + inds_mx1[5]\n"
                                    "             mx_2_ind_1 = current_y + inds_mx2[0]\n"
                                    "             mx_2_ind_2 = current_y + inds_mx2[2]\n"
                                    "             mx_2_ind_3 = current_y + inds_mx2[5]\n"
                                    "             mx_3_ind_1 = current_y + inds_mx3[0]\n"
                                    "             mx_3_ind_2 = current_y + inds_mx3[2]\n"
                                    "             mx_3_ind_3 = current_y + inds_mx3[5]\n"
                                    "             for ind_y in range(check):\n";                                        
                        }    
                        if(min_check == "1"){
                            code +=
                            "                         Vec.max_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";
                        }
                        else{
                            code +=
                            "                         Vec.min_2((Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y], (Vec.max_store_vec(mx1_arg1_c0[mx_1_ind_1], ind_y + 1, mx1_arg2_c0[mx_1_ind_2], mx1_arg2_vec, mx1_arg3_c0[mx_1_ind_3], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[mx_2_ind_1], ind_y + 1, mx2_arg2_c0[mx_2_ind_2], mx2_arg2_vec, mx2_arg3_c0[mx_2_ind_3], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[mx_3_ind_1], ind_y + 1, mx3_arg2_c0[mx_3_ind_2], mx3_arg2_vec, mx3_arg3_c0[mx_3_ind_3], mx3_arg3_vec)))\n";         
                        }  
                        code +=
                        "        for i in range(check):\n"
                        "           if count >= outer_stop:\n"
                        "               break\n"                                
                        "           m_c0_x[0][i] = Vec(i16(matrices_nv[0][count]), i16, 16)\n"
                        "           h_c0_x[0][i] = Vec(i16(H_nv[0][count]), i16, 16)\n"
                        "           v_c0_x[0][i] = Vec(i16(V_nv[0][count]), i16, 16)\n"
                        "        count += 1\n"
                        "        c1, c0, m_c1_x, m_c0_x, mx1_arg1_c0, mx1_arg1_c1, mx1_arg2_c0, mx1_arg2_c1, mx1_arg3_c0, mx1_arg3_c1, mx2_arg1_c0, mx2_arg1_c1, mx2_arg2_c0, mx2_arg2_c1, mx2_arg3_c0, mx2_arg3_c1, mx3_arg1_c0, mx3_arg1_c1, mx3_arg2_c0, mx3_arg2_c1, mx3_arg3_c0, mx3_arg3_c1, h_c0_x, h_c1_x, v_c0_x, v_c1_x  = c0, c1, m_c0_x, m_c1_x, mx1_arg1_c1, mx1_arg1_c0, mx1_arg2_c1, mx1_arg2_c0, mx1_arg3_c1, mx1_arg3_c0, mx2_arg1_c1, mx2_arg1_c0, mx2_arg2_c1, mx2_arg2_c0, mx2_arg3_c1, mx2_arg3_c0, mx3_arg1_c1, mx3_arg1_c0, mx3_arg2_c1, mx3_arg2_c0, mx3_arg3_c1, mx3_arg3_c0, h_c1_x, h_c0_x, v_c1_x, v_c0_x\n";
                        break;
                    }
                        

                }
                code += 
                        "    if z_value != i16(-1):\n"
                        "        for i in range(SEQ_NO_Q):\n"
                        "            ind = i // REG_SIZE\n"
                        "            remain = i - (ind * REG_SIZE)\n"
                        "            Vec.assign_am(matrices[c0][-1], ind, remain, scores, i, max_score, z_value)\n"
                        "            score = scores[i]\n"
                        "            scores[i] = " + byPass_mat + "\n"
                        "            print(" + byPass_mat + ")\n"
                        "    else:\n"
                        "        for i in range(SEQ_NO_Q):\n"
                        "            ind = i // REG_SIZE\n"
                        "            remain = i - (ind * REG_SIZE)   \n"
                        "            Vec.assign(matrices[c0][-1], ind, remain, scores, i)\n"
                        "            score = scores[i]\n"
                        "            scores[i] = " + byPass_mat + "\n"                        
                        "            print(" + byPass_mat + ")\n"
                        "    return scores\n"
                        "def sumOne(s_x, s_y, byPass, params, args1, args2, args3, bw_params, inds_mx1, inds_mx2, inds_mx3, checker_1, checker_2, checker_3, y_params_1, y_params_2, x_params_1, x_params_2, inds_ptr, inds_H_ptr, inds_V_ptr, first_list, second_list, third_list, mx1_names_final, mx2_names_final, mx3_names_final, hyper_params):\n"
                        "    SIZE = len(s_x[0])\n"
                        "    z_value = i16(byPass)\n"
                        "    REG_SIZE = 16\n"
                        "    outer_start = params[0]\n"
                        "    outer_step = params[1]\n"
                        "    outer_stop = 0\n"
                        "    if params[2] == -1:\n"
                        "        outer_stop += len(s_x[0])\n"
                        "    elif params[2] == -2:\n"
                        "        outer_stop += len(s_y[0])\n"
                        "    outer_stop += params[3]\n"
                        "    inner_start = params[4]\n"
                        "    inner_step = params[5]\n"
                        "    inner_stop = 0\n"
                        "    if params[6] == -1:\n"
                        "        inner_stop += len(s_x[0])\n"
                        "    elif params[6] == -2:\n"
                        "        inner_stop += len(s_y[0])\n"
                        "    inner_stop += params[7]\n"                             
                        "    lens = [len(s_x[0]), len(s_y[0])]\n"
                        "    major_outer_start = hyper_params[0]\n"
                        "    major_outer_step = hyper_params[1]\n"
                        "    major_outer_inst = hyper_params[3]\n"
                        "    major_inner_start = hyper_params[4]\n"
                        "    major_inner_step = hyper_params[5]\n"
                        "    major_inner_inst = hyper_params[7]\n"
                        "    major_params_1 = s_x[major_outer_start: len(s_x) + major_outer_inst: major_outer_step]\n"
                        "    major_params_2 = s_y[major_inner_start: len(s_y) + major_inner_inst: major_inner_step]\n"
                        "    matrices_nv = " + lst1_eq + "\n"
                        "    H_nv = " + lst2_eq + "\n"
                        "    V_nv = " + lst3_eq + "\n"                      
                        "    if hyper_params[2] == -2:\n"
                        "        major_params_1 = s_y[major_outer_start: len(s_y) + major_outer_inst: major_outer_step]\n"
                        "    if hyper_params[6] == -1:\n"
                        "        major_params_2 = s_x[major_inner_start: len(s_x) + major_inner_inst: major_inner_step]\n"
                        "    SEQ_NO_Q = len(major_params_2)\n"
                        "    for x in major_params_1:\n"
                        "        score = [i16(0) for _ in range(len(major_params_2))]\n" 
                        "        x_vec = [Vec(u8(ord(str(x[_]))), u8, 16) for _ in range(len(x))]\n"
                        "        for row in y_gen(major_params_2):\n"
                        "            MAX_LENGTH_Q = len(row[0])\n"
                        "            check = ((len(row)) // REG_SIZE) if len(row) % 16 == 0 else ((len(row)) // REG_SIZE) + 1\n"
                        "            max_score = [Vec(i16(0), i16, 16) for _ in range(check)]\n"
                        "            matrices = [[[Vec(i16(matrices_nv[__][___]), i16, 16) for _ in range(check)] for __ in range(SIZE + 1)] for ___ in range(2)]\n"
                        "            H = [[[Vec(i16(H_nv[__][___]), i16, 16) for _ in range(check)] for __ in range(SIZE + 1)] for ___ in range(2)]\n"
                        "            V = [[[Vec(i16(V_nv[__][___]), i16, 16) for _ in range(check)] for __ in range(SIZE + 1)] for ___ in range(2)]\n"              
                        "            t = [[u8(ord(str(row[_][__]))) if _ < SEQ_NO_Q else u8(0) for _ in range(SEQ_NO_Q + REG_SIZE)] for __ in range(MAX_LENGTH_Q)]\n"
                        "            seqs_t = tuple(list([[Vec(t[__][(_ * REG_SIZE): (_ + 1) * REG_SIZE], u8, 16) for _ in range(check)] for __ in range(MAX_LENGTH_Q)]))\n"
                        "            score = calculate(x, row, x_vec, seqs_t, matrices, H, V, check, params, score, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names_final, inds_mx1, mx2_names_final, inds_mx2, mx3_names_final, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, matrices_nv, H_nv, V_nv)\n"
                        "    return score\n";
                std::ofstream MyFile("code.py");
                MyFile << code << "\n";
                MyFile.close();
                M->parseCode(code);
                auto *sumOne = M->getOrRealizeFunc("sumOne", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                auto *sumOneCall = util::call(sumOne, {args_1, args_2, byPass_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, bw_ptr, mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});                        
                v->replaceAll(sumOneCall);                                             
            }
            else{
                std::cout << "F32\n"; 
                std::string code = "";    
                std::ifstream loop_file("LoopInfo.txt");
                std::basic_string min_check = "";
                //std::string params = "";
                std::string temp_reader = "";
                std::string params_test[34];
                int params[30];
                int array_ind_1[4] = {2, 0, 0, 1};
                int array_ind_2[4] = {2, 0, 0, 1};
                int array_ind_3[4] = {2, 0, 0, 1};
                for(int i = 0; i < 34; i++){
                    if(loop_file.eof()){
                    break;
                    }        
                    std::getline(loop_file, temp_reader);
                    params_test[i] = temp_reader;
                    if(i == 8){
                    min_check = temp_reader;
                    }
                }
                std::string first_param = "";
                std::string second_param = "";
                std::string third_param = "";
                std::string left_param = "";      
                if(params_test[11] == "-1"){
                    third_param = "0";
                    first_param = params_test[24].substr(0, params_test[24].find('.'));
                    second_param = params_test[25].substr(0, params_test[25].find('.'));
                    left_param = params_test[26].substr(0, params_test[26].find('.'));
                    for(int i = 0; i < 24; i++){
                    params[i] = std::stoi(params_test[i]);
                    }
                }
                else{
                    first_param = params_test[30].substr(0, params_test[30].find('.'));
                    second_param = params_test[31].substr(0, params_test[31].find('.'));
                    third_param = params_test[32].substr(0, params_test[32].find('.'));
                    left_param = params_test[33].substr(0, params_test[33].find('.'));
                    for(int i = 0; i < 30; i++){
                    if(i == 16){            
                        if(params_test[i].length() > 2){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_1[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_1[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_1[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_1[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_1[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_1[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_1[2] = std::stoi(temp_reader);                                
                        }

                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }
                    }
                    if(i == 22){
                        if(params_test[i].length() > 3){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_2[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_2[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_2[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_2[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_2[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_2[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_2[2] = std::stoi(temp_reader);                                
                        }
                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }            
                    }   
                    if(i == 28){
                        if(params_test[i].length() > 3){
                        temp_reader = params_test[i][0];
                        if(temp_reader == "-"){
                            array_ind_3[3] = -1;
                            temp_reader = params_test[i][1];
                            array_ind_3[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][3];
                            array_ind_3[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][5];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][6];
                            }
                            array_ind_3[2] = std::stoi(temp_reader);                
                        }
                        else{
                            array_ind_3[0] = std::stoi(temp_reader);
                            temp_reader = params_test[i][2];
                            array_ind_3[1] = std::stoi(temp_reader);
                            temp_reader = params_test[i][4];
                            if(temp_reader == "-"){
                            temp_reader += params_test[i][5];
                            }
                            array_ind_3[2] = std::stoi(temp_reader);                                
                        }
                        }
                        else{
                        params[i] = std::stoi(params_test[i]);
                        }            
                    }
                    if(i != 16 || i != 22 || i != 28){                 
                        params[i] = std::stoi(params_test[i]);
                    }
                    }        

                }
                std::ifstream prep_file("Prep_info.txt");      
                int hyper_p[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                for(int i = 0; i < 8; i++){
                    std::getline(prep_file, temp_reader);
                    hyper_p[i] = std::stoi(temp_reader);
                }
                prep_file.close();
                std::vector<Value*> hyper_params_arr;    
                for(int i = 0; i < 8; i++){
                    hyper_params_arr.push_back(M->getInt(hyper_p[i]));
                }
                Value *hyper_params_ptr = util::makeTuple(hyper_params_arr, M);       

                std::vector<Value*> arr_params_arr1;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr1.push_back(M->getInt(array_ind_1[i]));
                }
                Value *arr1_params_ptr = util::makeTuple(arr_params_arr1, M);       

                std::vector<Value*> arr_params_arr2;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr2.push_back(M->getInt(array_ind_2[i]));
                }
                Value *arr2_params_ptr = util::makeTuple(arr_params_arr2, M); 

                std::vector<Value*> arr_params_arr3;    
                for(int i = 0; i < 4; i++){
                    arr_params_arr3.push_back(M->getInt(array_ind_3[i]));
                }
                Value *arr3_params_ptr = util::makeTuple(arr_params_arr3, M);                                       

                std::vector<types::Type*> types_arr;    
                for(int i = 0; i < 30; i++){
                    types_arr.push_back(M->getIntType());
                }
                types::Type *typ_ptr30 = M->getTupleType(types_arr);

                std::vector<types::Type*> types_arr_int3;  
                for(int i = 0; i < 3; i++){
                    types_arr_int3.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int3 = M->getTupleType(types_arr_int3);

                std::vector<types::Type*> types_arr_int4;  
                for(int i = 0; i < 4; i++){
                    types_arr_int4.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int4 = M->getTupleType(types_arr_int4); 

                std::vector<types::Type*> types_arr6;  
                for(int i = 0; i < 6; i++){
                    types_arr6.push_back(M->getIntType());
                }
                types::Type *typ_ptr6 = M->getTupleType(types_arr6);

                std::vector<types::Type*> types_arr8;  
                for(int i = 0; i < 8; i++){
                    types_arr8.push_back(M->getIntType());
                }
                types::Type *typ_ptr8 = M->getTupleType(types_arr8);            

                std::vector<types::Type*> types_arr9;  
                for(int i = 0; i < 9; i++){
                    types_arr9.push_back(M->getIntType());
                }
                types::Type *typ_ptr9 = M->getTupleType(types_arr9);
                

                std::vector<types::Type*> types_arr10;  
                for(int i = 0; i < 10; i++){
                    types_arr10.push_back(M->getStringType());
                }
                types::Type *typ_ptr10 = M->getTupleType(types_arr10);    

                std::vector<types::Type*> types_arr_int10;  
                for(int i = 0; i < 10; i++){
                    types_arr_int10.push_back(M->getIntType());
                }
                types::Type *typ_ptr_int10 = M->getTupleType(types_arr_int10);          

                std::vector<types::Type*> types_arr3;  
                for(int i = 0; i < 3; i++){
                    types_arr3.push_back(M->getStringType());
                }
                types::Type *typ_ptr3 = M->getTupleType(types_arr3);

                std::vector<Value*> params_arr;    
                for(int i = 0; i < 30; i++){
                    params_arr.push_back(M->getInt(params[i]));
                }
                Value *params_ptr = util::makeTuple(params_arr, M);      
                loop_file.close();     
                int checker_1 = 1;
                int checker_2 = 1;
                int checker_3 = 1;
                int args1_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg1_arr;    
                int args2_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg2_arr;    
                int args3_test[6] = {0, 0, 0, 0, 0, -1};
                std::vector<Value*> arg3_arr;        

                std::ifstream arg_1_file("arg_1.txt");
                if(!arg_1_file){
                    checker_1 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_1_file, temp_reader);
                    args1_test[i] = std::stoi(temp_reader);
                    }
                }
                arg_1_file.close();
                std::ifstream arg_2_file("arg_2.txt");
                if(!arg_2_file){
                    checker_2 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_2_file, temp_reader);
                    args2_test[i] = std::stoi(temp_reader);
                    }
                }     
                arg_2_file.close();
                std::ifstream arg_3_file("arg_3.txt");
                if(!arg_3_file){
                    checker_3 = 0;
                }
                else{
                    for(int i = 0; i < 6; i++){
                    std::getline(arg_3_file, temp_reader);
                    args3_test[i] = std::stoi(temp_reader);
                    }
                } 
                arg_3_file.close();
                auto *checker_1_ptr = M->getInt(checker_1);
                auto *checker_2_ptr = M->getInt(checker_2);
                auto *checker_3_ptr = M->getInt(checker_3);
                for(int i = 0; i < 6; i++){
                    arg1_arr.push_back(M->getInt(args1_test[i]));
                    arg2_arr.push_back(M->getInt(args2_test[i]));
                    arg3_arr.push_back(M->getInt(args3_test[i]));
                }
                Value *args1_ptr = util::makeTuple(arg1_arr, M);    
                Value *args2_ptr = util::makeTuple(arg2_arr, M);    
                Value *args3_ptr = util::makeTuple(arg3_arr, M);    

                int lst1[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst1_arr;        
                int lst2[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst2_arr;  
                int lst3[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> lst3_arr;        
                std::string lst1_name = "";
                std::string lst2_name = "";
                std::string lst3_name = "";
                std::string lst1_eq = "";
                std::string lst2_eq = "[[0 for _ in range(SIZE + 1)] for __ in range(SIZE + 1)]";
                std::string lst3_eq = "[[0 for _ in range(SIZE + 1)] for __ in range(SIZE + 1)]";
                std::ifstream lst_1_file("lst_1.txt");
                if(lst_1_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_1_file, temp_reader);
                        lst1_name = temp_reader;
                    }
                    else{
                        std::getline(lst_1_file, temp_reader);
                        lst1_eq = temp_reader;
                    }
                    }
                }
                lst_1_file.close();
                std::ifstream lst_2_file("lst_2.txt");
                if(lst_2_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_2_file, temp_reader);
                        lst2_name = temp_reader;
                    }
                    else{
                        std::getline(lst_2_file, temp_reader);
                        lst2_eq = temp_reader;
                    }
                    }
                }
                lst_2_file.close();
                std::ifstream lst_3_file("lst_3.txt");
                if(lst_3_file){
                    for(int i = 0; i < 2; i++){
                    if(i == 0){
                        std::getline(lst_3_file, temp_reader);
                        lst3_name = temp_reader;
                    }
                    else{
                        std::getline(lst_3_file, temp_reader);
                        lst3_eq = temp_reader;
                    }
                    }
                }
                lst_3_file.close();
                //for(int i = 0; i < 10; i++){
                //    lst1_arr.push_back(M->getInt(lst1[i]));
                //    lst2_arr.push_back(M->getInt(lst2[i]));
                //    lst3_arr.push_back(M->getInt(lst3[i]));
               // }
                auto *lst1_ptr = M->getString(lst1_eq);
                auto *lst2_ptr = M->getString(lst2_eq);
                auto *lst3_ptr = M->getString(lst3_eq);
                auto *lst1_name_ptr = M->getString(lst1_name);
                auto *lst2_name_ptr = M->getString(lst2_name);
                auto *lst3_name_ptr = M->getString(lst3_name);          
                int byPass = -1;
                std::string byPass_mat = "";
                std::ifstream byPass_file("byPass.txt");
                if(byPass_file){
                    std::getline(byPass_file, temp_reader);
                    byPass = stoi(temp_reader);
                    std::getline(byPass_file, temp_reader);
                    byPass_mat = temp_reader;

                }
                auto *byPass_ptr = M->getInt(byPass);
                auto *args_1 = v->front();
                auto *args_2 = v->back();
                std::ifstream bw_file("bw.txt");
                int bw_temp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> bw_arr;  
                int bw = 0;
                int bw_param = 0;
                for(int i = 0; i < 9; i++){
                    std::getline(bw_file, temp_reader);
                    bw_temp[i] = std::stoi(temp_reader);
                    bw += bw_temp[i];
                    if(i == 0){
                    bw_param = bw_temp[i];
                    }
                }
                bw_file.close();  
                int t1_l = 0;
                int t2_l = 0;     
                int t3_l = 0;
                int t4_l = 0;
                int t1_r = 0;
                int t2_r = 0;
                int t3_r = 0;
                int t4_r = 0;
                int y_params_1 = 0;
                int y_params_2 = 0;
                int x_params_1 = 0;
                int x_params_2 = 0;      
                if(bw != 0){
                    if(bw_temp[2] == -2 || bw_temp[2] == 2){
                        t1_l = bw_temp[2];
                        t2_l = bw_temp[2] / 2;
                    }
                    else{
                        t1_l = 0;
                        t2_l = bw_temp[2];
                    }
                    if(bw_temp[3] == -2 || bw_temp[3] == 2){
                        t3_l = bw_temp[3];
                        t4_l = bw_temp[3] / 2;
                    }
                    else{
                        t3_l = 0;
                        t4_l = bw_temp[3];
                    }
                    if(bw_temp[6] == -2 || bw_temp[6] == 2){
                        t1_r = bw_temp[6];
                        t2_r = bw_temp[6] / 2;
                    }
                    else{
                        t1_r = 0;
                        t2_r = bw_temp[6];
                    }
                    if(bw_temp[7] == -2 || bw_temp[7] == 2){
                        t3_r = bw_temp[7];
                        t4_r = bw_temp[7] / 2;
                    }
                    else{
                        t3_r = 0;
                        t4_r = bw_temp[7];
                    }

                    y_params_1 = (bw_temp[2] - t2_l) + (bw_temp[3] - t4_l);
                    y_params_2 = (bw_temp[6] - t2_r) + (bw_temp[7] - t4_r);
                    x_params_1 = (bw_temp[2] - t1_l) + (bw_temp[3] - t3_l);
                    x_params_2 = (bw_temp[6] - t1_r) + (bw_temp[7] - t3_r);           
                }
                auto *y_params_1_ptr = M->getInt(y_params_1);
                auto *y_params_2_ptr = M->getInt(y_params_2);
                auto *x_params_1_ptr = M->getInt(x_params_1);
                auto *x_params_2_ptr = M->getInt(x_params_2);
            
                for(int i = 0; i < 9; i++){
                    bw_arr.push_back(M->getInt(bw_temp[i]));
                }
                Value *bw_ptr = util::makeTuple(bw_arr, M); 

                std::string mx_1_checker = "1";
                std::string mx_2_checker = "1";
                std::string mx_3_checker = "1";      

                std::string mx_1_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx1_arr_str;  
                std::string mx_2_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx2_arr_str;        
                std::string mx_3_array_str[3] = {"0", "0", "0"};
                std::vector<Value*> mx3_arr_str;

                int mx_1_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx1_arr;  
                int mx_2_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx2_arr;  
                int mx_3_array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                std::vector<Value*> mx3_arr;                

                std::ifstream mx_1_file("mx_arg1.txt");
                if(!mx_1_file){
                    mx_1_checker = "0";
                }
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_1_file, temp_reader);
                    if(i == 0){
                        mx_1_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_1_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_1_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_1_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_1_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_1_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_1_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_1_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_1_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_1_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_1_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                }
                mx_1_file.close();
                std::ifstream mx_2_file("mx_arg2.txt");
                if(!mx_2_file){
                    mx_2_checker = "0";
                }      
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_2_file, temp_reader);
                    if(i == 0){
                        mx_2_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_2_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_2_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_2_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_2_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_2_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_2_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_2_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_2_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_2_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_2_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                } 
                mx_2_file.close();   
                std::ifstream mx_3_file("mx_arg3.txt");   
                if(!mx_3_file){
                    mx_3_checker = "0";
                }       
                else{
                    for(int i = 0; i < 11; i++){
                    std::getline(mx_3_file, temp_reader);
                    if(i == 0){
                        mx_3_array_str[0] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 3){
                        mx_3_array_str[1] = temp_reader.substr(0, temp_reader.find('.'));
                    }          
                    if(i == 7){
                        mx_3_array_str[2] = temp_reader.substr(0, temp_reader.find('.'));
                    }
                    if(i == 1){
                        mx_3_array[0] = std::stoi(temp_reader);
                    }
                    if(i == 2){
                        mx_3_array[1] = std::stoi(temp_reader);
                    }
                    if(i == 4){
                        mx_3_array[2] = std::stoi(temp_reader);
                    }
                    if(i == 5){
                        mx_3_array[3] = std::stoi(temp_reader);
                    }
                    if(i == 6){
                        mx_3_array[4] = std::stoi(temp_reader);
                    }
                    if(i == 8){
                        mx_3_array[5] = std::stoi(temp_reader);
                    }
                    if(i == 9){
                        mx_3_array[6] = std::stoi(temp_reader);
                    }
                    if(i == 10){
                        mx_3_array[7] = std::stoi(temp_reader);
                    }                                                                                
                    }
                }
                mx_3_file.close();   
                for(int i = 0; i < 8; i++){
                    mx1_arr.push_back(M->getInt(mx_1_array[i]));
                    mx2_arr.push_back(M->getInt(mx_2_array[i]));
                    mx3_arr.push_back(M->getInt(mx_3_array[i]));
                }
                Value *mx1_ptr = util::makeTuple(mx1_arr, M);    
                Value *mx2_ptr = util::makeTuple(mx2_arr, M);    
                Value *mx3_ptr = util::makeTuple(mx3_arr, M);        

                for(int i = 0; i < 3; i++){
                    mx1_arr_str.push_back(M->getString(mx_1_array_str[i]));
                    mx2_arr_str.push_back(M->getString(mx_2_array_str[i]));
                    mx3_arr_str.push_back(M->getString(mx_3_array_str[i]));
                }
                Value *mx1_ptr_str = util::makeTuple(mx1_arr_str, M);    
                Value *mx2_ptr_str = util::makeTuple(mx2_arr_str, M);    
                Value *mx3_ptr_str = util::makeTuple(mx3_arr_str, M);    
                
                int mx1_names_final[3] = {-1, -1, -1};
                int mx2_names_final[3] = {-1, -1, -1};
                int mx3_names_final[3] = {-1, -1, -1};       

                int inds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int inds_H[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int inds_V[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int first_list = -1;
                int second_list = -1;
                int third_list = -1;
                if(left_param == lst1_name || (params[8] + params[9] + params[10] == 0)){
                    inds[0] = 1;
                    inds[1] = 1;
                    inds[2] = lst1[4];
                    inds[3] = lst1[5];
                    inds[4] = lst1[6];
                    inds[5] = lst1[7];
                    inds[6] = lst1[8];
                    inds[7] = lst1[9];
                    inds_H[0] = 2;
                    inds_H[1] = 2;
                    inds_H[2] = lst2[4];
                    inds_H[3] = lst2[5];
                    inds_H[4] = lst2[6];
                    inds_H[5] = lst2[7];
                    inds_H[6] = lst2[8];
                    inds_H[7] = lst2[9];          
                    inds_V[0] = 3;
                    inds_V[1] = 3;
                    inds_V[2] = lst3[4];
                    inds_V[3] = lst3[5];
                    inds_V[4] = lst3[6];
                    inds_V[5] = lst3[7];
                    inds_V[6] = lst3[8];
                    inds_V[7] = lst3[9];                    
                    if(first_param == lst1_name){
                        first_list = 0;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 1;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 2;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 0;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 1;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 2;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 0;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 1;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 2;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 0;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 1;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 2;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 0;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 1;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 2;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 0;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 1;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 2;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 0;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 1;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 2;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 0;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 1;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 2;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 0;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 1;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 2;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 0;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 1;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 2;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 0;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 1;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 2;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 0;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 1;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 2;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }

                }             
                else{
                    if(left_param == lst2_name){  
                    inds[0] = 2;
                    inds[1] = 2;
                    inds[2] = lst2[4];
                    inds[3] = lst2[5];
                    inds[4] = lst2[6];
                    inds[5] = lst2[7];
                    inds[6] = lst2[8];
                    inds[7] = lst2[9];
                    inds_H[0] = 1;
                    inds_H[1] = 1;
                    inds_H[2] = lst1[4];
                    inds_H[3] = lst1[5];
                    inds_H[4] = lst1[6];
                    inds_H[5] = lst1[7];
                    inds_H[6] = lst1[8];
                    inds_H[7] = lst1[9];          
                    inds_V[0] = 3;
                    inds_V[1] = 3;
                    inds_V[2] = lst3[4];
                    inds_V[3] = lst3[5];
                    inds_V[4] = lst3[6];
                    inds_V[5] = lst3[7];
                    inds_V[6] = lst3[8];
                    inds_V[7] = lst3[9];            
                    if(first_param == lst1_name){
                        first_list = 1;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 0;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 2;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 1;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 0;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 2;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 1;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 0;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 2;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 1;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 0;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 2;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 1;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 0;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 2;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 1;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 0;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 2;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 1;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 0;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 2;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 1;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 0;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 2;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 1;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 0;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 2;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 1;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 0;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 2;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 1;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 0;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 2;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 1;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 0;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 2;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }                
                    }
                    else{
                    inds[0] = 3;
                    inds[1] = 3;
                    inds[2] = lst3[4];
                    inds[3] = lst3[5];
                    inds[4] = lst3[6];
                    inds[5] = lst3[7];
                    inds[6] = lst3[8];
                    inds[7] = lst3[9];
                    inds_H[0] = 2;
                    inds_H[1] = 2;
                    inds_H[2] = lst2[4];
                    inds_H[3] = lst2[5];
                    inds_H[4] = lst2[6];
                    inds_H[5] = lst2[7];
                    inds_H[6] = lst2[8];
                    inds_H[7] = lst2[9];          
                    inds_V[0] = 1;
                    inds_V[1] = 1;
                    inds_V[2] = lst1[4];
                    inds_V[3] = lst1[5];
                    inds_V[4] = lst1[6];
                    inds_V[5] = lst1[7];
                    inds_V[6] = lst1[8];
                    inds_V[7] = lst1[9];            
                    if(first_param == lst1_name){
                        first_list = 2;
                    }
                    else{
                        if(first_param == lst2_name){
                        first_list = 1;
                        }
                        else{
                        if(first_param == lst3_name){
                            first_list = 0;
                        }
                        else{
                            first_list = -1;
                        }
                        }
                    }
                    if(second_param == lst1_name){
                        second_list = 2;
                    }
                    else{
                        if(second_param == lst2_name){
                        second_list = 1;
                        }
                        else{
                        if(second_param == lst3_name){
                            second_list = 0;
                        }
                        else{
                            second_list = -1;
                        }
                        }
                    }      
                    if(third_param == lst1_name){
                        third_list = 2;
                    }
                    else{
                        if(third_param == lst2_name){
                        third_list = 1;
                        }
                        else{
                        if(third_param == lst3_name){
                            third_list = 0;
                        }
                        else{
                            third_list = -1;
                        }
                        }
                    }
                    if(mx_1_checker != "0"){
                        if(mx_1_array_str[0] == lst1_name){
                        mx1_names_final[0] = 2;
                        }
                        else{
                        if(mx_1_array_str[0] == lst2_name){
                            mx1_names_final[0] = 1;
                        }
                        else{
                            if(mx_1_array_str[0] == lst3_name){
                            mx1_names_final[0] = 0;
                            }
                            else{
                            mx1_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_1_array_str[1] == lst1_name){
                        mx1_names_final[1] = 2;
                        }
                        else{
                        if(mx_1_array_str[1] == lst2_name){
                            mx1_names_final[1] = 1;
                        }
                        else{
                            if(mx_1_array_str[1] == lst3_name){
                            mx1_names_final[1] = 0;
                            }
                            else{
                            mx1_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_1_array_str[2] == lst1_name){
                        mx1_names_final[2] = 2;
                        }
                        else{
                        if(mx_1_array_str[2] == lst2_name){
                            mx1_names_final[2] = 1;
                        }
                        else{
                            if(mx_1_array_str[2] == lst3_name){
                            mx1_names_final[2] = 0;
                            }
                            else{
                            mx1_names_final[2] = -1;
                            }
                        }
                        }

                    }

                    if(mx_2_checker != "0"){
                        if(mx_2_array_str[0] == lst1_name){
                        mx2_names_final[0] = 2;
                        }
                        else{
                        if(mx_2_array_str[0] == lst2_name){
                            mx2_names_final[0] = 1;
                        }
                        else{
                            if(mx_2_array_str[0] == lst3_name){
                            mx2_names_final[0] = 0;
                            }
                            else{
                            mx2_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_2_array_str[1] == lst1_name){
                        mx2_names_final[1] = 2;
                        }
                        else{
                        if(mx_2_array_str[1] == lst2_name){
                            mx2_names_final[1] = 1;
                        }
                        else{
                            if(mx_2_array_str[1] == lst3_name){
                            mx2_names_final[1] = 0;
                            }
                            else{
                            mx2_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_2_array_str[2] == lst1_name){
                        mx2_names_final[2] = 2;
                        }
                        else{
                        if(mx_2_array_str[2] == lst2_name){
                            mx2_names_final[2] = 1;
                        }
                        else{
                            if(mx_2_array_str[2] == lst3_name){
                            mx2_names_final[2] = 0;
                            }
                            else{
                            mx2_names_final[2] = -1;
                            }
                        }
                        }

                    }          
                    if(mx_3_checker != "0"){
                        if(mx_3_array_str[0] == lst1_name){
                        mx3_names_final[0] = 2;
                        }
                        else{
                        if(mx_3_array_str[0] == lst2_name){
                            mx3_names_final[0] = 1;
                        }
                        else{
                            if(mx_3_array_str[0] == lst3_name){
                            mx3_names_final[0] = 0;
                            }
                            else{
                            mx3_names_final[0] = -1;
                            }
                        }
                        }

                        if(mx_3_array_str[1] == lst1_name){
                        mx3_names_final[1] = 2;
                        }
                        else{
                        if(mx_3_array_str[1] == lst2_name){
                            mx3_names_final[1] = 1;
                        }
                        else{
                            if(mx_3_array_str[1] == lst3_name){
                            mx3_names_final[1] = 0;
                            }
                            else{
                            mx3_names_final[1] = -1;
                            }
                        }
                        }
                        if(mx_3_array_str[2] == lst1_name){
                        mx3_names_final[2] = 2;
                        }
                        else{
                        if(mx_3_array_str[2] == lst2_name){
                            mx3_names_final[2] = 1;
                        }
                        else{
                            if(mx_3_array_str[2] == lst3_name){
                            mx3_names_final[2] = 0;
                            }
                            else{
                            mx3_names_final[2] = -1;
                            }
                        }
                        }

                    }     
                    }     
                }
                auto *first_list_ptr = M->getInt(first_list);
                auto *second_list_ptr = M->getInt(second_list);
                auto *third_list_ptr = M->getInt(third_list);
                std::vector<Value*> inds_arr;  
                std::vector<Value*> inds_H_arr;  
                std::vector<Value*> inds_V_arr;                    
                for(int i = 0; i < 8; i++){
                    inds_arr.push_back(M->getInt(inds[i]));
                    inds_H_arr.push_back(M->getInt(inds_H[i]));
                    inds_V_arr.push_back(M->getInt(inds_V[i]));
                }
                Value *inds_ptr = util::makeTuple(inds_arr, M);    
                Value *inds_H_ptr = util::makeTuple(inds_H_arr, M);    
                Value *inds_V_ptr = util::makeTuple(inds_V_arr, M);        

                std::vector<Value*> mx1_names_arr;  
                std::vector<Value*> mx2_names_arr;  
                std::vector<Value*> mx3_names_arr;              
                for(int i = 0; i < 3; i++){
                    mx1_names_arr.push_back(M->getInt(mx1_names_final[i]));
                    mx2_names_arr.push_back(M->getInt(mx2_names_final[i]));
                    mx3_names_arr.push_back(M->getInt(mx3_names_final[i]));
                }
                Value *mx1_names_ptr = util::makeTuple(mx1_names_arr, M);    
                Value *mx2_names_ptr = util::makeTuple(mx2_names_arr, M);    
                Value *mx3_names_ptr = util::makeTuple(mx3_names_arr, M);          

                std::string mx_final = mx_1_checker + mx_2_checker + mx_3_checker;
                int mx_final_i = stoi(mx_final);                
                switch(mx_final_i){
                    case 0:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                                   
                                "        max_value = f32(0)\n" 
                                "        a_1 = f32(args1[1])\n"
                                "        b_1 = f32(args1[3])\n"
                                "        am_1 = f32(args1[2])\n"
                                "        x_ind_1 = args1[4]\n"
                                "        y_ind_1 = args1[5]\n"                                
                                "        a_2 = f32(args2[1])\n"
                                "        b_2 = f32(args2[3])\n"
                                "        am_2 = f32(args2[2])\n"
                                "        x_ind_2 = args2[4]\n"
                                "        y_ind_2 = args2[5]\n"                                                                
                                "        a_3 = f32(args3[1])\n"
                                "        b_3 = f32(args3[3])\n"
                                "        am_3 = f32(args3[2])\n"                                
                                "        x_ind_3 = args3[4]\n"
                                "        y_ind_3 = args3[5]\n"                                
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        arg_1 = matrices if first_list == 0 else matrices_left if first_list == 2 else matrices_top\n"
                                "        arg_2 = matrices if second_list == 0 else matrices_left if second_list == 2 else matrices_top\n"                                
                                "        if params[11] != -1:\n"
                                "            arg_3 = matrices if third_list == 0 else matrices_left if third_list == 2 else matrices_top\n"                                                                
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                       
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                                                       
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"                         
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                                                        
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"     
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"      
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }
                        }
                        else{
                            code +=
                                "                    arg_1_ind = j + params[16]\n"
                                "                    arg_2_ind = j + params[22]\n"
                                "                    arg_3_ind = j + params[28]\n"  
                                "                    if params[11] != -1:\n"; 
                            if(min_check == "1"){
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"                                  
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"    
                                "                        left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                        if checker_3 == 0:\n"                                 
                                "                            left_value += f32(params[29])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                left_value += am_3\n"
                                "                            else:\n"
                                "                                left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                        left_value *= f32(params[24])\n"  
                                "                        target_value = max(diagonal_value, top_value, left_value)\n"                                                                                                                               
                                "                    else:\n"
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"     
                                "                        target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"    
                                "                        left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                        if checker_3 == 0:\n"
                                "                            left_value += f32(params[29])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                left_value += am_3\n"
                                "                            else:\n"
                                "                                left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                        left_value *= f32(params[24])\n"  
                                "                        target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                    else:\n"
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"      
                                "                        target_value = min(diagonal_value, top_value)\n";
                            }

                        }    
                code += 
                    "                    arg_2[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"                    
                    "                    if params[11] != -1:\n"
                    "                        arg_3[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                    "                    arg_1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(target_value)\n"
                    "                    if target_value > max_value:\n"
                    "                        max_value = target_value\n";                    

                        break;
                    }
                    case 1:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        a_1 = f32(args1[1])\n"
                                "        b_1 = f32(args1[3])\n"
                                "        am_1 = f32(args1[2])\n"
                                "        x_ind_1 = args1[4]\n"
                                "        y_ind_1 = args1[5]\n"                                
                                "        a_2 = f32(args2[1])\n"
                                "        b_2 = f32(args2[3])\n"
                                "        am_2 = f32(args2[2])\n"     
                                "        x_ind_2 = args2[4]\n"
                                "        y_ind_2 = args2[5]\n"                                                           
                                "        mx3_arg2_vec = f32(inds_mx3[4])\n"
                                "        mx3_arg3_vec = f32(inds_mx3[7])\n"                         
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        arg_1 = matrices if first_list == 0 else matrices_left if first_list == 2 else matrices_top\n"
                                "        arg_2 = matrices if second_list == 0 else matrices_left if second_list == 2 else matrices_top\n"                                
                                "        mx3_arg1 = matrices if mx3_names[0] == 0 else matrices_left if mx3_names[0] == 2 else matrices_top\n"
                                "        mx3_arg2 = matrices if mx3_names[1] == 0 else matrices_left if mx3_names[1] == 2 else matrices_top\n"                         
                                "        mx3_arg3 = matrices if mx3_names[2] == 0 else matrices_left if mx3_names[2] == 2 else matrices_top\n"
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";

                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"   
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"                                
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1])\n"                                   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";
                            }
                            else{
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"     
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"                                
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1])\n"                                   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";
                            }
                        }
                        else{
                            code +=
                                "                        arg_1_ind = j + params[16]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n";
                            if(min_check == "1"){
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"   
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"                                
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1])\n"                                   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";
                            }
                            else{
                                code +=
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"     
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"                                
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_3_ind_1])\n"                                   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";
                            }

                        }    
                code += 
                    "                        arg_2[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"                    
                    "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                    "                        arg_1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(target_value)\n"
                    "                        if target_value > max_value:\n"
                    "                            max_value = target_value\n";                    
                        break;
                    }
                    case 10:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, V_00, V_col, V_col_mul, V_row, V_row_mul, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        a_1 = f32(args1[1])\n"
                                "        b_1 = f32(args1[3])\n"
                                "        am_1 = f32(args1[2])\n"
                                "        x_ind_1 = args1[4]\n"
                                "        y_ind_1 = args1[5]\n"                                
                                "        mx2_arg2_vec = f32(inds_mx2[4])\n"
                                "        mx2_arg3_vec = f32(inds_mx2[7])\n"                                
                                "        a_3 = f32(args3[1])\n"
                                "        b_3 = f32(args3[3])\n"
                                "        am_3 = f32(args3[2])\n"  
                                "        x_ind_3 = args3[4]\n"
                                "        y_ind_3 = args3[5]\n"                                                              
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        arg_1 = matrices if first_list == 0 else matrices_left if first_list == 2 else matrices_top\n"
                                "        mx2_arg1 = matrices if mx2_names[0] == 0 else matrices_left if mx2_names[0] == 2 else matrices_top\n"
                                "        mx2_arg2 = matrices if mx2_names[1] == 0 else matrices_left if mx2_names[1] == 2 else matrices_top\n"
                                "        mx2_arg3 = matrices if mx2_names[2] == 0 else matrices_left if mx2_names[2] == 2 else matrices_top\n"                                
                                "        if params[11] != -1:\n"
                                "            arg_3 = matrices if third_list == 0 else matrices_left if third_list == 2 else matrices_top\n"                                                                
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                       
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                                                       
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }
                        }
                        else{
                            code +=
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n"; 
                            if(min_check == "1"){
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                            if checker_1 == 0:\n"
                                "                                diagonal_value += f32(params[17])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                    diagonal_value += am_1\n"
                                "                                else:\n"
                                "                                    diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                            diagonal_value *= f32(params[12])\n"
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }

                        }    
                code += 
                    "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"                           
                    "                        if params[11] != -1:\n"
                    "                            arg_3[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"                
                    "                        arg_1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(target_value)\n"
                    "                        if target_value > max_value:\n"
                    "                            max_value = target_value\n";

                        break;
                    }
                    case 11:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        a_1 = f32(args1[1])\n"
                                "        b_1 = f32(args1[3])\n"
                                "        am_1 = f32(args1[2])\n"
                                "        x_ind_1 = args1[4]\n"
                                "        y_ind_1 = args1[5]\n"
                                "        mx2_arg2_vec = f32(inds_mx2[4])\n"
                                "        mx2_arg3_vec = f32(inds_mx2[7])\n"
                                "        mx3_arg2_vec = f32(inds_mx3[4])\n"
                                "        mx3_arg3_vec = f32(inds_mx3[7])\n"
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        arg_1 = matrices if first_list == 0 else matrices_left if first_list == 2 else matrices_top\n"
                                "        mx2_arg1 = matrices if mx2_names[0] == 0 else matrices_left if mx2_names[0] == 2 else matrices_top\n"
                                "        mx2_arg2 = matrices if mx2_names[1] == 0 else matrices_left if mx2_names[1] == 2 else matrices_top\n"
                                "        mx2_arg3 = matrices if mx2_names[2] == 0 else matrices_left if mx2_names[2] == 2 else matrices_top\n"
                                "        mx3_arg1 = matrices if mx3_names[0] == 0 else matrices_left if mx3_names[0] == 2 else matrices_top\n"
                                "        mx3_arg2 = matrices if mx3_names[1] == 0 else matrices_left if mx3_names[1] == 2 else matrices_top\n"                         
                                "        mx3_arg3 = matrices if mx3_names[2] == 0 else matrices_left if mx3_names[2] == 2 else matrices_top\n"
                                "        @par(gpu=True, collapse=1)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                    
                                    break;
                                }
                                case 1:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                               
                                    break;
                                }
                                case 0:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                              
                                    break;
                                }
                            }
                        }
                        else{
                            code += 
                                "                        arg_1_ind = j + params[16]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                     

                        }
                        if(min_check == "1"){
                            code += 
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"                                
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";

                        }
                        else{
                            code += 
                                "                        diagonal_value = arg_1[t * (SIZE + 1) * (SIZE + 1) + (i + params[14]) * (SIZE + 1) + arg_1_ind] * f32(params[9])\n"
                                "                        if checker_1 == 0:\n"
                                "                            diagonal_value += f32(params[17])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_1] == 'N' or sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] == 'N':\n"
                                "                                diagonal_value += am_1\n"
                                "                            else:\n"
                                "                                diagonal_value += a_1 if sequences[t * SIZE * 2 + i + x_ind_1] == sequences[t * SIZE * 2 + j + y_ind_1 + SIZE] else b_1\n"                            
                                "                        diagonal_value *= f32(params[12])\n"
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"                                
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";
                        }
                        code += 
                                "                        arg_1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(target_value)\n"
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                                "                        if target_value > max_value:\n"
                                "                            max_value = target_value\n";                                
                        break;
                    }
                    case 100:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        mx1_arg2_vec = f32(inds_mx1[4])\n"
                                "        mx1_arg3_vec = f32(inds_mx1[7])\n"                                
                                "        a_2 = f32(args2[1])\n"
                                "        b_2 = f32(args2[3])\n"
                                "        am_2 = f32(args2[2])\n"                                
                                "        x_ind_2 = args2[4]\n"
                                "        y_ind_2 = args2[5]\n"                                
                                "        a_3 = f32(args3[1])\n"
                                "        b_3 = f32(args3[3])\n"
                                "        am_3 = f32(args3[2])\n"                                
                                "        x_ind_3 = args3[4]\n"
                                "        y_ind_3 = args3[5]\n"                                
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        mx1_arg1 = matrices if mx1_names[0] == 0 else matrices_left if mx1_names[0] == 2 else matrices_top\n"
                                "        mx1_arg2 = matrices if mx1_names[1] == 0 else matrices_left if mx1_names[1] == 2 else matrices_top\n"
                                "        mx1_arg3 = matrices if mx1_names[2] == 0 else matrices_left if mx1_names[2] == 2 else matrices_top\n"
                                "        arg_2 = matrices if second_list == 0 else matrices_left if second_list == 2 else matrices_top\n"                                
                                "        if params[11] != -1:\n"
                                "            arg_3 = matrices if third_list == 0 else matrices_left if third_list == 2 else matrices_top\n"                                                                
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                       
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                                                       
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=                            
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n" 
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"                                
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"     
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n" 
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"                                
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"     
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }
                        }
                        else{
                            code +=
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n"; 
                            if(min_check == "1"){
                                code +=
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n" 
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"                                
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"     
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"    
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n" 
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"                                
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i - 1) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                            if checker_2 == 0:\n"
                                "                                top_value += f32(params[23])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                    top_value += am_2\n"
                                "                                else:\n"
                                "                                    top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                            top_value *= f32(params[18])\n"     
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }

                        }    
                code += 
                    "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(diagonal_value)\n"                           
                    "                        if params[11] != -1:\n"
                    "                            arg_3[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"                    
                    "                        arg_2[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"
                    "                        if target_value > max_value:\n"
                    "                            max_value = target_value\n";                            

                        break;
                    }
                    case 101:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        mx1_arg2_vec = f32(inds_mx1[4])\n"
                                "        mx1_arg3_vec = f32(inds_mx1[7])\n"                                
                                "        a_2 = f32(args2[1])\n"
                                "        b_2 = f32(args2[3])\n"
                                "        am_2 = f32(args2[2])\n"   
                                "        x_ind_2 = args2[4]\n"
                                "        y_ind_2 = args2[5]\n"                                                             
                                "        mx3_arg2_vec = f32(inds_mx3[4])\n"
                                "        mx3_arg3_vec = f32(inds_mx3[7])\n"                              
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        mx1_arg1 = matrices if mx1_names[0] == 0 else matrices_left if mx1_names[0] == 2 else matrices_top\n"
                                "        mx1_arg2 = matrices if mx1_names[1] == 0 else matrices_left if mx1_names[1] == 2 else matrices_top\n"
                                "        mx1_arg3 = matrices if mx1_names[2] == 0 else matrices_left if mx1_names[2] == 2 else matrices_top\n"
                                "        arg_2 = matrices if second_list == 0 else matrices_left if second_list == 2 else matrices_top\n"                                
                                "        mx3_arg1 = matrices if mx3_names[0] == 0 else matrices_left if mx3_names[0] == 2 else matrices_top\n"
                                "        mx3_arg2 = matrices if mx3_names[1] == 0 else matrices_left if mx3_names[1] == 2 else matrices_top\n"                         
                                "        mx3_arg3 = matrices if mx3_names[2] == 0 else matrices_left if mx3_names[2] == 2 else matrices_top\n"
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";              
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                  
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"       
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";                                                         
                            }
                            else{
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"       
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";  
                            }
                        }
                        else{
                            code +=
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        arg_2_ind = j + params[22]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";
                            if(min_check == "1"){
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"       
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";                                                         
                            }
                            else{
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        top_value = arg_2[t * (SIZE + 1) * (SIZE + 1) + (i + params[20]) * (SIZE + 1) + arg_2_ind] * f32(params[10])\n"
                                "                        if checker_2 == 0:\n"
                                "                            top_value += f32(params[23])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_2] == 'N' or sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] == 'N':\n"
                                "                                top_value += am_2\n"
                                "                            else:\n"
                                "                                top_value += a_2 if sequences[t * SIZE * 2 + i + x_ind_2] == sequences[t * SIZE * 2 + j + y_ind_2 + SIZE] else b_2\n"                            
                                "                        top_value *= f32(params[18])\n"       
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";   
                            }

                        }    
                code += 
                    "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(diagonal_value)\n"                           
                    "                        arg_2[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"                    
                    "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                    "                        if target_value > max_value:\n"
                    "                            max_value = target_value\n";
                        break;
                    }
                    case 110:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        mx1_arg2_vec = f32(inds_mx1[4])\n"
                                "        mx1_arg3_vec = f32(inds_mx1[7])\n"                                
                                "        mx2_arg2_vec = f32(inds_mx2[4])\n"
                                "        mx2_arg3_vec = f32(inds_mx2[7])\n"
                                "        a_3 = f32(args3[1])\n"
                                "        b_3 = f32(args3[3])\n"
                                "        am_3 = f32(args3[2])\n"                                
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        mx1_arg1 = matrices if mx1_names[0] == 0 else matrices_left if mx1_names[0] == 2 else matrices_top\n"
                                "        mx1_arg2 = matrices if mx1_names[1] == 0 else matrices_left if mx1_names[1] == 2 else matrices_top\n"
                                "        mx1_arg3 = matrices if mx1_names[2] == 0 else matrices_left if mx1_names[2] == 2 else matrices_top\n"
                                "        mx2_arg1 = matrices if mx2_names[0] == 0 else matrices_left if mx2_names[0] == 2 else matrices_top\n"
                                "        mx2_arg2 = matrices if mx2_names[1] == 0 else matrices_left if mx2_names[1] == 2 else matrices_top\n"
                                "        mx2_arg3 = matrices if mx2_names[2] == 0 else matrices_left if mx2_names[2] == 2 else matrices_top\n"
                                "        if params[11] != -1:\n"
                                "            arg_3 = matrices if third_list == 0 else matrices_left if third_list == 2 else matrices_top\n"                                                                
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";
                                    break;
                                }
                                case 1:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                       
                                    break;
                                }
                                case 0:{
                                    code +=
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n";                                                       
                                    break;
                                }
                            }
                            if(min_check == "1"){
                                code +=
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                            if checker_3 == 0:\n"
                                "                                left_value += f32(params[29])\n"
                                "                            else:\n"
                                "                                if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                    left_value += am_3\n"
                                "                                else:\n"
                                "                                    left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                            left_value *= f32(params[24])\n"  
                                "                            target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                        else:\n"
                                "                            mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                            diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                            mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                            top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                            target_value = min(diagonal_value, top_value)\n";
                            }
                        }
                        else{
                            code +=
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        arg_3_ind = j + params[28]\n"  
                                "                        if params[11] != -1:\n"; 
                            if(min_check == "1"){
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                        if checker_3 == 0:\n"
                                "                            left_value += f32(params[29])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                left_value += am_3\n"
                                "                            else:\n"
                                "                                left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                        left_value *= f32(params[24])\n"  
                                "                        target_value = max(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                    else:\n"
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        target_value = max(diagonal_value, top_value)\n";
                            }
                            else{
                                code +=
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        left_value = arg_3[t * (SIZE + 1) * (SIZE + 1) + (i + params[26]) * (SIZE + 1) + arg_3_ind] * f32(params[11])\n"
                                "                        if checker_3 == 0:\n"
                                "                            left_value += f32(params[29])\n"
                                "                        else:\n"
                                "                            if sequences[t * SIZE * 2 + i + x_ind_3] == 'N' or sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] == 'N':\n"
                                "                                left_value += am_3\n"
                                "                            else:\n"
                                "                                left_value += a_3 if sequences[t * SIZE * 2 + i + x_ind_3] == sequences[t * SIZE * 2 + j + y_ind_3 + SIZE] else b_3\n"                            
                                "                        left_value *= f32(params[24])\n"  
                                "                        target_value = min(diagonal_value, top_value, left_value)\n"                                                                                              
                                "                    else:\n"
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        target_value = min(diagonal_value, top_value)\n";
                            }

                        }    
                code += 
                    "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(diagonal_value)\n"                           
                    "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"                           
                    "                        if params[11] != -1:\n"
                    "                            arg_3[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                    "                        if target_value > max_value:\n"
                    "                            max_value = target_value\n";                    
                        break;
                    }
                    case 111:{
                        code +=
                                "def align(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"
                                "    def kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY):\n"                                                         
                                "        SIZE = 512\n"                                                               
                                "        max_value = f32(0)\n" 
                                "        mx1_arg2_vec = f32(inds_mx1[4])\n"
                                "        mx1_arg3_vec = f32(inds_mx1[7])\n"  
                                "        mx2_arg2_vec = f32(inds_mx2[4])\n"
                                "        mx2_arg3_vec = f32(inds_mx2[7])\n"
                                "        mx3_arg2_vec = f32(inds_mx3[4])\n"
                                "        mx3_arg3_vec = f32(inds_mx3[7])\n"
                                "        L = bw_params[4]\n"
                                "        U = bw_params[8]\n"
                                "        mx1_arg1 = matrices if mx1_names[0] == 0 else matrices_left if mx1_names[0] == 2 else matrices_top\n"
                                "        mx1_arg2 = matrices if mx1_names[1] == 0 else matrices_left if mx1_names[1] == 2 else matrices_top\n"
                                "        mx1_arg3 = matrices if mx1_names[2] == 0 else matrices_left if mx1_names[2] == 2 else matrices_top\n"
                                "        mx2_arg1 = matrices if mx2_names[0] == 0 else matrices_left if mx2_names[0] == 2 else matrices_top\n"
                                "        mx2_arg2 = matrices if mx2_names[1] == 0 else matrices_left if mx2_names[1] == 2 else matrices_top\n"
                                "        mx2_arg3 = matrices if mx2_names[2] == 0 else matrices_left if mx2_names[2] == 2 else matrices_top\n"
                                "        mx3_arg1 = matrices if mx3_names[0] == 0 else matrices_left if mx3_names[0] == 2 else matrices_top\n"
                                "        mx3_arg2 = matrices if mx3_names[1] == 0 else matrices_left if mx3_names[1] == 2 else matrices_top\n"                         
                                "        mx3_arg3 = matrices if mx3_names[2] == 0 else matrices_left if mx3_names[2] == 2 else matrices_top\n"
                                "        @par(gpu=True)\n"                                  
                                "        for t in range(QUANTITY):\n"
                                "            for i in range(outer_start, outer_stop, outer_step):\n"                                                    
                                "                x_1_arg = x_params_1 * i\n"
                                "                x_2_arg = x_params_2 * i\n"
                                "                for j in range(inner_start, inner_stop, inner_step):\n"
                                "                    y_1_arg = y_params_1 * j\n"
                                "                    y_2_arg = y_params_2 * j\n";                    
                        if(bw != 0){
                            switch(bw_param){
                                case 2:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L or (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L or (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                    
                                    break;
                                }
                                case 1:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L and (x_2_arg + y_2_arg) >= U:\n"
                                "                        if (x_1_arg + y_1_arg) == L and (x_2_arg + y_2_arg) == U:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                               
                                    break;
                                }
                                case 0:{
                                    code += 
                                "                    if (x_1_arg + y_1_arg) <= L:\n"
                                "                        if (x_1_arg + y_1_arg) == L:\n"
                                "                            matrices[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_left[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                            matrices_top[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(-10000)\n"
                                "                    else:\n"
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                              
                                    break;
                                }
                            }
                        }
                        else{
                            code += 
                                "                        mx_1_ind_1 = j + inds_mx1[0]\n"
                                "                        mx_1_ind_2 = j + inds_mx1[2]\n"
                                "                        mx_1_ind_3 = j + inds_mx1[5]\n"
                                "                        mx_2_ind_1 = j + inds_mx2[0]\n"
                                "                        mx_2_ind_2 = j + inds_mx2[2]\n"
                                "                        mx_2_ind_3 = j + inds_mx2[5]\n"
                                "                        mx_3_ind_1 = j + inds_mx3[0]\n"
                                "                        mx_3_ind_2 = j + inds_mx3[2]\n"
                                "                        mx_3_ind_3 = j + inds_mx3[5]\n";                                                     

                        }
                        if(min_check == "1"){
                            code += 
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = max(diagonal_value, top_value, left_value)\n";
                        }
                        else{
                            code += 
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1] = mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec if mx1_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[3]) * (SIZE + 1) + mx_1_ind_2] + mx1_arg2_vec > mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec else mx1_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[6]) * (SIZE + 1) + mx_1_ind_3] + mx1_arg3_vec\n"
                                "                        diagonal_value = f32(mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx1[1]) * (SIZE + 1) + mx_1_ind_1])\n"                                
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1] = mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec if mx2_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[3]) * (SIZE + 1) + mx_2_ind_2] + mx2_arg2_vec > mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec else mx2_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[6]) * (SIZE + 1) + mx_2_ind_3] + mx2_arg3_vec\n"
                                "                        top_value = f32(mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx2[1]) * (SIZE + 1) + mx_2_ind_1])\n"
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1] = mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec if mx3_arg2[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[3]) * (SIZE + 1) + mx_3_ind_2] + mx3_arg2_vec > mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec else mx3_arg3[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[6]) * (SIZE + 1) + mx_3_ind_3] + mx3_arg3_vec\n"
                                "                        left_value = f32(mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + (i + inds_mx3[1]) * (SIZE + 1) + mx_3_ind_1])\n"   
                                "                        target_value = min(diagonal_value, top_value, left_value)\n";
                        }
                        code += 
                                "                        mx1_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(diagonal_value)\n"                           
                                "                        mx3_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(left_value)\n"
                                "                        mx2_arg1[t * (SIZE + 1) * (SIZE + 1) + i * (SIZE + 1) + j] = f32(top_value)\n"
                                "                        if target_value > max_value:\n"
                                "                            max_value = target_value\n";                             
                        break;
                    }
                        

                }
                code +=                                 
                                "            if f32(z_value) != f32(-1):\n"
                                "                if max_value - target_value <= f32(z_value):\n"
                                "                    scores[t] = target_value\n"
                                "                else:\n"
                                "                    scores[t] = f32(-32768)\n"
                                "            else:\n"
                                "                scores[t] = target_value\n"                                
                                "    CUDA_XBLOCK_SIZE = 256\n"                                     
                                "    num_blocks = QUANTITY // CUDA_XBLOCK_SIZE\n"                                                                                          
                                "    kernel(scores, matrices, matrices_left, matrices_top, sequences, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names, inds_mx1, mx2_names, inds_mx2, mx3_names, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY)\n"                                
                                "def sumOne(s_x, s_y, byPass, params, args1, args2, args3, bw_params, inds_mx1, inds_mx2, inds_mx3, checker_1, checker_2, checker_3, y_params_1, y_params_2, x_params_1, x_params_2, inds_ptr, inds_H_ptr, inds_V_ptr, first_list, second_list, third_list, mx1_names_final, mx2_names_final, mx3_names_final, hyper_params):\n"
                                "    SIZE = len(s_x[0])\n"
                                "    CUDA_XBLOCK_SIZE = 256\n"                                                
                                "    z_value = f32(byPass)\n"
                                "    major_outer_start = hyper_params[0]\n"
                                "    major_outer_step = hyper_params[1]\n"
                                "    major_outer_inst = hyper_params[3]\n"
                                "    major_inner_start = hyper_params[4]\n"
                                "    major_inner_step = hyper_params[5]\n"
                                "    major_inner_inst = hyper_params[7]\n"
                                "    major_params_1 = s_x[major_outer_start: len(s_x) + major_outer_inst: major_outer_step]\n"
                                "    major_params_2 = s_y[major_inner_start: len(s_y) + major_inner_inst: major_inner_step]\n"
                                "    if hyper_params[2] == -2:\n"
                                "        major_params_1 = s_y[major_outer_start: len(s_y) + major_outer_inst: major_outer_step]\n"
                                "    if hyper_params[6] == -1:\n"
                                "        major_params_2 = s_x[major_inner_start: len(s_x) + major_inner_inst: major_inner_step]\n"
                                "    var = len(major_params_1)\n"
                                "    QUANTITY = var * var\n"
                                "    seqx_pair_init = ['' for seq in range(QUANTITY)]\n"
                                "    seqy_pair_init = ['' for seq in range(QUANTITY)]\n"                                
                                "    for j in range(len(major_params_1)):\n"
                                "        for _ in range(var):\n"
                                "            seqx_pair_init[(j * var) + _ ] = str(major_params_1[j])\n"
                                "            seqy_pair_init[(j * var) + _ ] = str(major_params_2[_])\n"
                                "    if len(seqx_pair_init) != len(seqy_pair_init):\n"
                                "        print('Error: Number of target sequences does not match number of query sequences, They have to be equal for GPU')\n"
                                "        print('Target Size:', len(seqx_pair_init), ', Query Size:', len(seqy_pair_init))\n"
                                "    paired_sequences = [(target.strip(), query.strip()) for target, query in zip(seqx_pair_init, seqy_pair_init)]\n"
                                "    sequences_bytes = ''.join(pair[0] + pair[1] for pair in paired_sequences)\n"                             
                                "    outer_start = params[0]\n"
                                "    outer_step = params[1]\n"
                                "    outer_stop = 0\n"  
                                "    if params[2] == -1:\n"
                                "        outer_stop += len(s_x[0])\n"
                                "    elif params[2] == -2:\n"
                                "        outer_stop += len(s_y[0])\n"
                                "    outer_stop += params[3]\n"
                                "    inner_start = params[4]\n"
                                "    inner_step = params[5]\n"
                                "    inner_stop = 0\n"
                                "    if params[6] == -1:\n"
                                "        inner_stop += len(s_x[0])\n"
                                "    elif params[6] == -2:\n"
                                "        inner_stop += len(s_y[0])\n"
                                "    inner_stop += params[7]\n"
                                "    lens = [len(s_x[0]), len(s_y[0])]\n"                                                                                
                                "    scores = [f32(0)] * QUANTITY\n"
                                "    max_score = [f32(0)] * QUANTITY\n"                     
                                "    matrices_1d = " + lst1_eq + "\n"
                                "    matrices = [f32(item) for sublist in matrices_1d for item in sublist] * QUANTITY\n"
                                "    H_1d = " + lst2_eq + "\n"
                                "    H = [f32(item) for sublist in H_1d for item in sublist] * QUANTITY\n"
                                "    V_1d = " + lst3_eq + "\n"   
                                "    V = [f32(item) for sublist in V_1d for item in sublist] * QUANTITY\n"                                                             
                                "    align(scores, matrices, V, H, sequences_bytes, params, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, first_list, second_list, third_list, mx1_names_final, inds_mx1, mx2_names_final, inds_mx2, mx3_names_final, inds_mx3, z_value, max_score, bw_params, x_params_1, x_params_2, y_params_1, y_params_2, checker_1, checker_2, checker_3, QUANTITY)\n"
                                "    for score in scores:\n"
                                "       print(" + byPass_mat + ")\n";              
                std::ofstream MyFile("code.py");
                MyFile << code << "\n";
                MyFile.close();
                M->parseCode(code);
                auto *sumOne = M->getOrRealizeFunc("sumOne", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                auto *sumOneCall = util::call(sumOne, {args_1, args_2, byPass_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, bw_ptr, mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});                        
                v->replaceAll(sumOneCall);                       
            }                                           

        
        }

    }
    void FuncReplacement::handle(CallInstr *v) { transform(v); }

    } // namespace vectron


