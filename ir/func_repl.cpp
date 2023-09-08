#include "func_repl.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <iostream>
#include <fstream>

namespace vectron {

using namespace codon::ir;


void FuncReplacement::transform(CallInstr *v) {    
    auto *M = v->getModule(); 
    auto *orig = util::getFunc(v->getCallee());
    if (!orig || orig->getUnmangledName() != "prep")
      return;   
    else{
      std::ifstream loop_file("LoopInfo.txt");
      std::basic_string min_check = "";
      //std::string params = "";
      std::string temp_reader = "";
      std::string params_test[34];
      int params[30];
      for(int i = 0; i < 34; i++){
        if(loop_file.eof()){
          break;
        }        
        std::getline(loop_file, temp_reader);
        if(i < 30){
          params[i] = std::stoi(temp_reader);
        }
        //params += temp_reader;
        params_test[i] = temp_reader;
        if(i == 8){
          min_check = temp_reader;
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
      std::string first_param = "";
      std::string second_param = "";
      std::string third_param = "";
      std::string left_param = "";
      if(params_test[11] == "-1"){
        third_param = "0";
        first_param = params_test[30];
        second_param = params_test[31];
        left_param = params_test[32];
      }
      else{
        first_param = params_test[30]; 
        second_param = params_test[31];
        third_param = params_test[32];
        left_param = params_test[33];
      }
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
      int args1_test[6] = {0, 0, 0, 0, 0, 0};
      std::vector<Value*> arg1_arr;    
      int args2_test[6] = {0, 0, 0, 0, 0, 0};
      std::vector<Value*> arg2_arr;    
      int args3_test[6] = {0, 0, 0, 0, 0, 0};
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
      std::ifstream lst_1_file("lst_1.txt");
      if(lst_1_file){
        for(int i = 0; i < 11; i++){
          if(i == 0){
            std::getline(lst_1_file, temp_reader);
            lst1_name = temp_reader;
          }
          else{
            std::getline(lst_1_file, temp_reader);
            lst1[i - 1] = std::stoi(temp_reader);
          }
        }
      }
      
      lst_1_file.close();
      std::ifstream lst_2_file("lst_2.txt");
      if(lst_2_file){
        for(int i = 0; i < 11; i++){
          if(i == 0){
            std::getline(lst_2_file, temp_reader);
            lst2_name = temp_reader;
          }
          else{
            std::getline(lst_2_file, temp_reader);
            lst2[i - 1] = std::stoi(temp_reader);
          }
        }
      }
      lst_2_file.close();
      std::ifstream lst_3_file("lst_3.txt");
      if(lst_3_file){
        for(int i = 0; i < 11; i++){
          if(i == 0){
            std::getline(lst_3_file, temp_reader);
            lst3_name = temp_reader;
          }
          else{
            std::getline(lst_3_file, temp_reader);
            lst3[i - 1] = std::stoi(temp_reader);
          }
        }
      }
      lst_3_file.close();
      for(int i = 0; i < 10; i++){
        lst1_arr.push_back(M->getInt(lst1[i]));
        lst2_arr.push_back(M->getInt(lst2[i]));
        lst3_arr.push_back(M->getInt(lst3[i]));
      }

      Value *lst1_ptr = util::makeTuple(lst1_arr, M);    
      Value *lst2_ptr = util::makeTuple(lst2_arr, M);    
      Value *lst3_ptr = util::makeTuple(lst3_arr, M);     
      auto *lst1_name_ptr = M->getString(lst1_name);
      auto *lst2_name_ptr = M->getString(lst2_name);
      auto *lst3_name_ptr = M->getString(lst3_name);      
      int zdrop = -1;
      std::ifstream zdrop_file("zdrop.txt");
      if(zdrop_file){
        std::getline(zdrop_file, temp_reader);
        zdrop = stoi(temp_reader);
      }
      auto *zdrop_ptr = M->getInt(zdrop);
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
            mx_1_array_str[0] = temp_reader;
          }
          if(i == 3){
            mx_1_array_str[1] = temp_reader;
          }          
          if(i == 7){
            mx_1_array_str[2] = temp_reader;
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
            mx_2_array_str[0] = temp_reader;
          }
          if(i == 3){
            mx_2_array_str[1] = temp_reader;
          }          
          if(i == 7){
            mx_2_array_str[2] = temp_reader;
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
            mx_3_array_str[0] = temp_reader;
          }
          if(i == 3){
            mx_3_array_str[1] = temp_reader;
          }          
          if(i == 7){
            mx_3_array_str[2] = temp_reader;
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
      if(min_check == "1"){
        if(bw != 0){
          switch(bw_param){
            case 2:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_or_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_or_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_or_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_or_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_or_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_or_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_or_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_or_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 1:   
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_and_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_and_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_and_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_and_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_and_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_and_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_and_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_and_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 0:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_1_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_1_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_1_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_1_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_1_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_1_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_1_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_1_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;  
          }
        }
        else{
          switch(mx_final_i){
            case 0:{
              auto *sumOne_0 = M->getOrRealizeFunc("sumOne_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_0);               
              break;
            }
            case 1:{
              auto *sumOne_1 = M->getOrRealizeFunc("sumOne_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_1);               
              break;
            }
            case 10:{
              auto *sumOne_10 = M->getOrRealizeFunc("sumOne_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_10);               
              break;
            }
            case 11:{
              auto *sumOne_11 = M->getOrRealizeFunc("sumOne_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_11); 
              break;
            }
            case 100:{
              auto *sumOne_100 = M->getOrRealizeFunc("sumOne_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_100);               
              break;
            }
            case 101:{
              auto *sumOne_101 = M->getOrRealizeFunc("sumOne_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_101);               
              break;
            }
            case 110:{
              auto *sumOne_110 = M->getOrRealizeFunc("sumOne_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_110);               
              break;
            }
            case 111:{
              auto *sumOne_111 = M->getOrRealizeFunc("sumOne_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_111);               
              break;  
            }            
          } 
        }
      }
      else{
        if(bw != 0){
          switch(bw_param){
            case 2:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_or_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_or_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_or_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_or_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_or_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_or_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_or_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_or_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 1:   
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_and_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_and_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_and_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_and_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_and_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_and_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_and_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_and_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 0:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_1_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_1_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_1_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_1_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_1_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_1_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_1_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_1_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;  
          }
        }
        else{
          switch(mx_final_i){
            case 0:{
              auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_0", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_0);               
              break;
            }
            case 1:{
              auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_1", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_1);               
              break;
            }
            case 10:{
              auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_10", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_10);               
              break;
            }
            case 11:{
              auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_11", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_11); 
              break;
            }
            case 100:{
              auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_100", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_100);               
              break;
            }
            case 101:{
              auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_101", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_101);               
              break;
            }
            case 110:{
              auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_110", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_110);               
              break;
            }
            case 111:{
              auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_111", {args_1->getType(), args_2->getType(), M->getIntType(), typ_ptr30, typ_ptr6, typ_ptr6, typ_ptr6, typ_ptr_int10, typ_ptr_int10, typ_ptr_int10, typ_ptr9, typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr8, typ_ptr8, typ_ptr8, M->getIntType(), M->getIntType(), M->getIntType(), typ_ptr_int3, typ_ptr_int3, typ_ptr_int3, typ_ptr8});
              auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2, zdrop_ptr, params_ptr, args1_ptr, args2_ptr, args3_ptr, lst1_ptr, lst2_ptr, lst3_ptr, bw_ptr,  mx1_ptr, mx2_ptr, mx3_ptr, checker_1_ptr, checker_2_ptr, checker_3_ptr, y_params_1_ptr, y_params_2_ptr, x_params_1_ptr, x_params_2_ptr, inds_ptr, inds_H_ptr, inds_V_ptr, first_list_ptr, second_list_ptr, third_list_ptr, mx1_names_ptr, mx2_names_ptr, mx3_names_ptr, hyper_params_ptr});
              v->replaceAll(sumOneCall_111);               
              break;  
            }            
          } 
        }    
      }
    }
    
    
}


void FuncReplacement::handle(CallInstr *v) { transform(v); }

} // namespace vectron


