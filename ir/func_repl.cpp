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
#include <unordered_map> 

namespace vectron {
    extern std::unordered_map<std::string, std::map<std::string, std::string>> globalAttributes; 
    using namespace codon::ir;


    void FuncReplacement::transform(CallInstr *v) {      
        auto *M = v->getModule(); 
        auto *orig = util::getFunc(v->getCallee());
        auto att_att = util::hasAttribute(orig, "vectron_init");
        if (!att_att)
            return;   
        else{
            int checker_1 = 0;
            int checker_2 = 0;
            int checker_3 = 0;     
            std::string mx_1_checker = "0";
            std::string mx_2_checker = "0";
            std::string mx_3_checker = "0";       
            auto bypass_attr = globalAttributes["bypass"];                                                       
            std::string code = "";
            auto params_attr = globalAttributes["params"];
            std::map<std::string, std::string> arg1_attr;
            std::map<std::string, std::string> arg2_attr;
            std::map<std::string, std::string> arg3_attr;
            std::map<std::string, std::string> mx1_attr;
            std::map<std::string, std::string> mx2_attr;
            std::map<std::string, std::string> mx3_attr;
            auto arg1_finder = globalAttributes.find("arg1");
            if (arg1_finder != globalAttributes.end()) {
                checker_1 = 1;
                arg1_attr = globalAttributes["arg1"];  
            }                
            auto arg2_finder = globalAttributes.find("arg2");
            if (arg2_finder != globalAttributes.end()) {
                checker_2 = 1;
                arg2_attr = globalAttributes["arg2"];  
            }
            auto arg3_finder = globalAttributes.find("arg3");
            if (arg3_finder != globalAttributes.end()) {
                checker_3 = 1;
                arg3_attr = globalAttributes["arg3"];  
            }
            auto mx1_finder = globalAttributes.find("mx1");
            if (mx1_finder != globalAttributes.end()) {
                mx_1_checker = "1";
                mx1_attr = globalAttributes["mx1"];
            }            
            auto mx2_finder = globalAttributes.find("mx2");
            if (mx2_finder != globalAttributes.end()) {
                mx_2_checker = "1";
                mx2_attr = globalAttributes["mx2"];
            }    
            auto mx3_finder = globalAttributes.find("mx3");
            if (mx3_finder != globalAttributes.end()) {
                mx_3_checker = "1";
                mx3_attr = globalAttributes["mx3"];
            }    
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
            auto lst1_finder = globalAttributes.find("lst_1");
            if (lst1_finder != globalAttributes.end()) {
                auto lst_1 = globalAttributes["lst_1"]; 
                lst1_name = lst_1["part_0"];
                lst1_eq = lst_1["part_1"];                 
            }
            auto lst2_finder = globalAttributes.find("lst_2");
            if (lst2_finder != globalAttributes.end()) {
                auto lst_2 = globalAttributes["lst_2"]; 
                lst2_name = lst_2["part_0"];
                lst2_eq = lst_2["part_1"];             
            }
            auto lst3_finder = globalAttributes.find("lst_3");
            if (lst3_finder != globalAttributes.end()) {
                auto lst_3 = globalAttributes["lst_3"]; 
                lst3_name = lst_3["part_0"];
                lst3_eq = lst_3["part_1"]; 
            }                                   
            auto var_value = globalAttributes["var"];                                                       
                                                   
            auto lst_2 = globalAttributes["lst_2"];                                                       
            auto lst_3 = globalAttributes["lst_3"]; 
            auto prep = globalAttributes["prep"];  
            int bw_temp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
            auto bw_finder = globalAttributes.find("bw");
            if (bw_finder != globalAttributes.end()) {            
                auto bw_mat = globalAttributes["bw"];  
                bw_temp[0] = std::stoi(bw_mat["bw_00"]);
                bw_temp[1] = std::stoi(bw_mat["bw_01"]);
                bw_temp[2] = std::stoi(bw_mat["bw_02"]);
                bw_temp[3] = std::stoi(bw_mat["bw_03"]);
                bw_temp[4] = std::stoi(bw_mat["bw_04"]);
                bw_temp[5] = std::stoi(bw_mat["bw_05"]);
                bw_temp[6] = std::stoi(bw_mat["bw_06"]);
                bw_temp[7] = std::stoi(bw_mat["bw_07"]);
                bw_temp[8] = std::stoi(bw_mat["bw_08"]);                
            }
        
            std::vector<Value*> bw_arr;  
            int bw = 0;
            int bw_param = bw_temp[0];
            for(int i = 0; i < 9; i++){
                bw += bw_temp[i];
            }            
            std::string var_type = var_value["Value"];     
            std::basic_string min_check = "";
            //std::string params = "";
            std::string temp_reader = "";
            std::string params_test[34];
            int params[30];
            int array_ind_1[4] = {2, 0, 0, 1};
            int array_ind_2[4] = {2, 0, 0, 1};
            int array_ind_3[4] = {2, 0, 0, 1};
            min_check = params_attr["params_08"];
            params_test[0] = params_attr["params_00"];
            params_test[1] = params_attr["params_01"];
            params_test[2] = params_attr["params_02"];
            params_test[3] = params_attr["params_03"];
            params_test[4] = params_attr["params_04"];
            params_test[5] = params_attr["params_05"];
            params_test[6] = params_attr["params_06"];
            params_test[7] = params_attr["params_07"];
            params_test[8] = params_attr["params_08"];
            params_test[9] = params_attr["params_09"];
            params_test[10] = params_attr["params_10"];
            params_test[11] = params_attr["params_11"];
            params_test[12] = params_attr["params_12"];
            params_test[13] = params_attr["params_13"];
            params_test[14] = params_attr["params_14"];
            params_test[15] = params_attr["params_15"];
            params_test[16] = params_attr["params_16"];
            params_test[17] = params_attr["params_17"];
            params_test[18] = params_attr["params_18"];
            params_test[19] = params_attr["params_19"];
            params_test[20] = params_attr["params_20"];
            params_test[21] = params_attr["params_21"];
            params_test[22] = params_attr["params_22"];
            params_test[23] = params_attr["params_23"];
            params_test[24] = params_attr["params_24"];
            params_test[25] = params_attr["params_25"];
            params_test[26] = params_attr["params_26"];
            params_test[27] = params_attr["params_27"];
            params_test[28] = params_attr["params_28"];
            params_test[29] = params_attr["params_29"];
            params_test[30] = params_attr["params_30"];
            params_test[31] = params_attr["params_31"];
            params_test[32] = params_attr["params_32"];
            params_test[33] = params_attr["params_33"];

            static int64_t min_check_int = std::stoi(min_check);
            auto *min_check_ptr = M->getInt(min_check_int);
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
                if(i != 16 && i != 22 && i != 28){                 
                    params[i] = std::stoi(params_test[i]);
                }
                }        

            }
            int hyper_p[8] = {0, 0, 0, 0, 0, 0, 0, 0};            
            hyper_p[0] = std::stoi(prep["prep_00"]);
            hyper_p[1] = std::stoi(prep["prep_01"]);
            hyper_p[2] = std::stoi(prep["prep_02"]);
            hyper_p[3] = std::stoi(prep["prep_03"]);
            hyper_p[4] = std::stoi(prep["prep_04"]);
            hyper_p[5] = std::stoi(prep["prep_05"]);
            hyper_p[6] = std::stoi(prep["prep_06"]);
            hyper_p[7] = std::stoi(prep["prep_07"]);
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

            int args1_test[6] = {0, 0, 0, 0, 0, -1};
            std::vector<Value*> arg1_arr;    
            int args2_test[6] = {0, 0, 0, 0, 0, -1};
            std::vector<Value*> arg2_arr;    
            int args3_test[6] = {0, 0, 0, 0, 0, -1};
            std::vector<Value*> arg3_arr;        
            if(checker_1 == 1){
                args1_test[0] = std::stoi(arg1_attr["arg1_00"]);
                args1_test[1] = std::stoi(arg1_attr["arg1_01"]);
                args1_test[2] = std::stoi(arg1_attr["arg1_02"]);
                args1_test[3] = std::stoi(arg1_attr["arg1_03"]);
                args1_test[4] = std::stoi(arg1_attr["arg1_04"]);
                args1_test[5] = std::stoi(arg1_attr["arg1_05"]);
            }
            if(checker_2 == 1){
                args2_test[0] = std::stoi(arg2_attr["arg2_00"]);
                args2_test[1] = std::stoi(arg2_attr["arg2_01"]);
                args2_test[2] = std::stoi(arg2_attr["arg2_02"]);
                args2_test[3] = std::stoi(arg2_attr["arg2_03"]);
                args2_test[4] = std::stoi(arg2_attr["arg2_04"]);
                args2_test[5] = std::stoi(arg2_attr["arg2_05"]);
            }
            if(checker_3 == 1){
                args3_test[0] = std::stoi(arg3_attr["arg3_00"]);
                args3_test[0] = std::stoi(arg3_attr["arg3_00"]);
                args3_test[1] = std::stoi(arg3_attr["arg3_01"]);
                args3_test[2] = std::stoi(arg3_attr["arg3_02"]);
                args3_test[3] = std::stoi(arg3_attr["arg3_03"]);
                args3_test[4] = std::stoi(arg3_attr["arg3_04"]);
                args3_test[5] = std::stoi(arg3_attr["arg3_05"]);
            }

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
            byPass = stoi(bypass_attr["Value"]);
            byPass_mat = bypass_attr["Expression"];

            auto *byPass_mat_ptr = M->getString(byPass_mat);
            auto *byPass_ptr = M->getInt(byPass);
            auto *args_1 = v->front();
            auto *args_2 = v->back();
            static int64_t bw_param_stat = bw_param;
            static int64_t bw_stat = bw;                         
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

            if(mx_1_checker == "1"){
                mx_1_array_str[0] = mx1_attr["mx1_00"].substr(0, mx1_attr["mx1_00"].find('.'));
                mx_1_array_str[1] = mx1_attr["mx1_03"].substr(0, mx1_attr["mx1_03"].find('.'));
                mx_1_array_str[2] = mx1_attr["mx1_07"].substr(0, mx1_attr["mx1_07"].find('.'));
                mx_1_array[0] = std::stoi(mx1_attr["mx1_01"]);
                mx_1_array[1] = std::stoi(mx1_attr["mx1_02"]);
                mx_1_array[2] = std::stoi(mx1_attr["mx1_04"]);
                mx_1_array[3] = std::stoi(mx1_attr["mx1_05"]);
                mx_1_array[4] = std::stoi(mx1_attr["mx1_06"]);
                mx_1_array[5] = std::stoi(mx1_attr["mx1_08"]);
                mx_1_array[6] = std::stoi(mx1_attr["mx1_09"]);
                mx_1_array[7] = std::stoi(mx1_attr["mx1_10"]);                                                                                     
            }

            if(mx_2_checker == "1"){
                mx_2_array_str[0] = mx2_attr["mx2_00"].substr(0, mx2_attr["mx2_00"].find('.'));
                mx_2_array_str[1] = mx2_attr["mx2_03"].substr(0, mx2_attr["mx2_03"].find('.'));
                mx_2_array_str[2] = mx2_attr["mx2_07"].substr(0, mx2_attr["mx2_07"].find('.'));
                mx_2_array[0] = std::stoi(mx2_attr["mx2_01"]);
                mx_2_array[1] = std::stoi(mx2_attr["mx2_02"]);
                mx_2_array[2] = std::stoi(mx2_attr["mx2_04"]);
                mx_2_array[3] = std::stoi(mx2_attr["mx2_05"]);
                mx_2_array[4] = std::stoi(mx2_attr["mx2_06"]);
                mx_2_array[5] = std::stoi(mx2_attr["mx2_08"]);
                mx_2_array[6] = std::stoi(mx2_attr["mx2_09"]);
                mx_2_array[7] = std::stoi(mx2_attr["mx2_10"]);                                                                                     
            }

            if(mx_3_checker == "1"){
                mx_3_array_str[0] = mx3_attr["mx3_00"].substr(0, mx3_attr["mx3_00"].find('.'));
                mx_3_array_str[1] = mx3_attr["mx3_03"].substr(0, mx3_attr["mx3_03"].find('.'));
                mx_3_array_str[2] = mx3_attr["mx3_07"].substr(0, mx3_attr["mx3_07"].find('.'));
                mx_3_array[0] = std::stoi(mx3_attr["mx3_01"]);
                mx_3_array[1] = std::stoi(mx3_attr["mx3_02"]);
                mx_3_array[2] = std::stoi(mx3_attr["mx3_04"]);
                mx_3_array[3] = std::stoi(mx3_attr["mx3_05"]);
                mx_3_array[4] = std::stoi(mx3_attr["mx3_06"]);
                mx_3_array[5] = std::stoi(mx3_attr["mx3_08"]);
                mx_3_array[6] = std::stoi(mx3_attr["mx3_09"]);
                mx_3_array[7] = std::stoi(mx3_attr["mx3_10"]);                                                                                     
            }


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
            static int64_t mx_i = mx_final_i;
            std::string name = "";
            if (var_type == "\"i16\"")
                name += "align_cpu";
            else
                name += "align_gpu";
            if(min_check == "1")
                name += "_max";
            else
                name += "_min";
            switch(mx_final_i){
                case 0:
                    name += "_000";
                    break;
                case 1:
                    name += "_001";
                    break;                        
                case 10:
                    name += "_010";
                    break;                        
                case 11:
                    name += "_011";
                    break;                        
                case 100:
                    name += "_100";
                    break;                        
                case 101:
                    name += "_101";
                    break;                        
                case 110:
                    name += "_110";
                    break;                        
                case 111:
                    name += "_111";                                                                                                                                                
                    break;                        
            }
            if(bw != 0){
                switch(bw_param){
                case 2:
                    name += "_or";
                    break;                        
                case 1:
                    name += "_and";
                    break;                        
                case 0:
                    name += "_one";
                    break;                        
                }
            }
            name += ".codon";

            std::array<char, 128> buffer;
            std::string cmd_result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("which codon", "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                cmd_result += buffer.data();
            }
            size_t binPos = cmd_result.find("bin");
            cmd_result = cmd_result.substr(0, binPos) + "lib/codon/stdlib/vectron/";
            std::string func_name = name;
            name = cmd_result + name;
            std::cout << name << std::endl;

            std::ifstream fileIn(name);
            std::vector<std::string> lines;
            std::string line;
            bool found_mat = false;
            bool found_h = false;
            bool found_v = false;

            while (std::getline(fileIn, line)) {
                if (line.find("    matrices_nv = ") == 0 && !found_mat) {
                    lines.push_back("    matrices_nv = " + lst1_eq);
                    found_mat = true;
                } else {
                    if (line.find("    H_nv = ") == 0 && !found_h) {
                        lines.push_back("    H_nv = " + lst2_eq);
                        found_h = true;
                    } 
                    else{   
                        if (line.find("    V_nv = ") == 0 && !found_v) {
                            lines.push_back("    V_nv = " + lst3_eq);
                            found_v = true;
                        } 
                        else{  
                            if (line.find("            scores[i] = ") == 0) {
                                lines.push_back("            scores[i] = " + byPass_mat);
                            }                                 
                            else{                                                                               
        
                                lines.push_back(line);
                            }
                        }
                    }
                }
            }
            fileIn.close();

            std::ofstream fileOut(name);


            for (const auto& l : lines) {
                fileOut << l << "\n";
            }                
            fileOut.close();
            std::remove("/vectron/experiments_docker/source/script_name.txt");
            if (var_type == "\"i16\""){             
                auto *sumOne = M->getOrRealizeFunc("vectron_cpu", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8}, {bw_param_stat, bw_stat, mx_i, min_check_int}, "std.vectron.dispatcher");
                auto *sumOneCall = util::call(sumOne, {args_1, args_2, byPass_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, bw_ptr, mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});                        
                v->replaceAll(sumOneCall);                                                    
            }
            else{    
                auto *sumOne = M->getOrRealizeFunc("vectron_gpu", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8}, {bw_param_stat, bw_stat, mx_i, min_check_int}, "std.vectron.dispatcher");
                auto *sumOneCall = util::call(sumOne, {args_1, args_2, byPass_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, bw_ptr, mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});                        
                v->replaceAll(sumOneCall);                                  
            }                                           

        
        }

    }
    void FuncReplacement::handle(CallInstr *v) { transform(v); }

    } // namespace vectron


