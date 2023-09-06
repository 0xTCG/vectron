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
      //std::string params_test[34];
      for(int i = 0; i < 34; i++){
        if(loop_file.eof()){
          break;
        }        
        std::getline(loop_file, temp_reader);
        //params += temp_reader;
        //params_test[i] = temp_reader;
        if(i == 8){
          min_check = temp_reader;
        }        
      }      
      /*auto var_make = M->Nr<M->getArrayType(M->getStringType())>(params_test);
      auto *tst_ptr = util::makeTuple(var_make, M);
      auto *params_ptr = M->getString(params);
      loop_file.close();     
      int checker_1 = 1;
      int checker_2 = 1;
      int checker_3 = 1;
      std::string args1;
      std::string args2;
      std::string args3;
     
      auto *bw_ptr = M->getString(bw_params);

      std::string mx_1_array;
      std::string mx_2_array;
      std::string mx_3_array;

      auto *mx_1_ptr = M->getString(mx_1_array);
      auto *mx_2_ptr = M->getString(mx_2_array);
      auto *mx_3_ptr = M->getString(mx_3_array);

      std::ifstream arg_1_file("arg_1.txt");
      if(!arg_1_file){
        checker_1 = 0;
      }
      else{
        for(int i = 0; i < 6; i++){
          std::getline(arg_1_file, temp_reader);
          args1 += temp_reader;
        }
      }
      std::ifstream arg_2_file("arg_2.txt");
      if(!arg_2_file){
        checker_2 = 0;
      }
      else{
        for(int i = 0; i < 6; i++){
          std::getline(arg_2_file, temp_reader);
          args2 += temp_reader;
        }
      }     
      std::ifstream arg_3_file("arg_3.txt");
      if(!arg_3_file){
        checker_3 = 0;
      }
      else{
        for(int i = 0; i < 6; i++){
          std::getline(arg_3_file, temp_reader);
          args3 += temp_reader;
        }
      } 
      auto *arg_1_ptr = M->getString(args1);
      auto *arg_2_ptr = M->getString(args2);
      auto *arg_3_ptr = M->getString(args3);
      std::string lst1;
      std::string lst2;
      std::string lst3;
      std::string lst1_name = "";
      std::string lst2_name = "";
      std::string lst3_name = "";
      std::ifstream lst_1_file("lst_1.txt");
      if(lst_1_file){
        for(int i = 0; i < 10; i++){
          if(i == 0){
            std::getline(lst_1_file, temp_reader);
            lst1_name = temp_reader;
          }
          else{
            std::getline(lst_1_file, temp_reader);
            lst1 += temp_reader;
          }
        }
      }
      std::ifstream lst_2_file("lst_2.txt");
      if(lst_2_file){
        for(int i = 0; i < 10; i++){
          if(i == 0){
            std::getline(lst_2_file, temp_reader);
            lst2_name = temp_reader;
          }
          else{
            std::getline(lst_2_file, temp_reader);
            lst2 += temp_reader;
          }
        }
      }
      std::ifstream lst_3_file("lst_3.txt");
      if(lst_3_file){
        for(int i = 0; i < 10; i++){
          if(i == 0){
            std::getline(lst_3_file, temp_reader);
            lst3_name = temp_reader;
          }
          else{
            std::getline(lst_3_file, temp_reader);
            lst3 += temp_reader;
          }
        }
      } 
      auto *lst_1_ptr = M->getString(lst1);
      auto *lst_2_ptr = M->getString(lst1);
      auto *lst_3_ptr = M->getString(lst1);
      auto *lst1_name_ptr = M->getString(lst1_name);
      auto *lst2_name_ptr = M->getString(lst2_name);
      auto *lst3_name_ptr = M->getString(lst3_name);
      auto *checker_1_ptr = M->getInt(checker_1);
      auto *checker_2_ptr = M->getInt(checker_2);
      auto *checker_3_ptr = M->getInt(checker_3);*/
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
      std::string bw_temp = "";
      int bw = 0;
      int bw_param = 0;
      std::string bw_params;
      for(int i = 0; i < 9; i++){
        std::getline(bw_file, bw_temp);
        bw_params += bw_temp;
        bw += stoi(bw_temp);
        if(i == 0){
          if(bw_temp == "0"){
            bw_param = 0;
          }
          if(bw_temp == "1"){
            bw_param = 1;    
          }
          if(bw_temp == "2"){
            bw_param = 2;
          }
        }
      }
      bw_file.close();       
      std::string mx_1_checker = "1";
      std::string mx_2_checker = "1";
      std::string mx_3_checker = "1";      
      std::ifstream mx_1_file("mx_arg1.txt");
      if(!mx_1_file){
        mx_1_checker = "0";
      }
      //else{
      //  for(int i = 0; i < 10; i++){
      //    std::getline(mx_1_file, temp_reader);
      //    mx_1_array += temp_reader;
      //  }
      //}
      std::ifstream mx_2_file("mx_arg2.txt");
      if(!mx_2_file){
        mx_2_checker = "0";
      }      
      //else{
      //  for(int i = 0; i < 10; i++){
      //    std::getline(mx_2_file, temp_reader);
      //    mx_2_array += temp_reader;
      //  }
      //}      
      std::ifstream mx_3_file("mx_arg3.txt");   
      if(!mx_3_file){
        mx_3_checker = "0";
      }       
      //else{
      //  for(int i = 0; i < 10; i++){
      //    std::getline(mx_3_file, temp_reader);
      //    mx_3_array += temp_reader;
      //  }
      //}

      std::string mx_final = mx_1_checker + mx_2_checker + mx_3_checker;

      int mx_final_i = stoi(mx_final);
      if(min_check == "1"){
        if(bw != 0){
          switch(bw_param){
            case 2:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_or_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_or_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_or_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_or_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_or_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_or_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_or_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_or_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 1:   
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_and_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_and_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_and_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_and_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_and_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_and_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_and_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_and_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 0:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_1_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_1_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_1_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_1_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_1_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_1_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_1_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_1_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
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
              auto *sumOne_0 = M->getOrRealizeFunc("sumOne_0", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
              v->replaceAll(sumOneCall_0);               
              break;
            }
            case 1:{
              auto *sumOne_1 = M->getOrRealizeFunc("sumOne_1", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
              v->replaceAll(sumOneCall_1);               
              break;
            }
            case 10:{
              auto *sumOne_10 = M->getOrRealizeFunc("sumOne_10", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
              v->replaceAll(sumOneCall_10);               
              break;
            }
            case 11:{
              auto *sumOne_11 = M->getOrRealizeFunc("sumOne_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
              auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
              v->replaceAll(sumOneCall_11); 
              break;
            }
            case 100:{
              auto *sumOne_100 = M->getOrRealizeFunc("sumOne_100", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
              v->replaceAll(sumOneCall_100);               
              break;
            }
            case 101:{
              auto *sumOne_101 = M->getOrRealizeFunc("sumOne_101", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
              v->replaceAll(sumOneCall_101);               
              break;
            }
            case 110:{
              auto *sumOne_110 = M->getOrRealizeFunc("sumOne_110", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
              v->replaceAll(sumOneCall_110);               
              break;
            }
            case 111:{
              auto *sumOne_111 = M->getOrRealizeFunc("sumOne_111", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
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
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_or_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_or_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_or_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_or_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_or_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_or_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_or_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_or_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 1:   
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_and_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_and_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_and_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_and_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_and_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_and_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_and_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_and_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
                  v->replaceAll(sumOneCall_111);               
                  break;  
                }            
              } 
              break;
            case 0:
              switch(mx_final_i){
                case 0:{
                  auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_1_0", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
                  v->replaceAll(sumOneCall_0);               
                  break;
                }
                case 1:{
                  auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_1_1", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
                  v->replaceAll(sumOneCall_1);               
                  break;
                }
                case 10:{
                  auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_1_10", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
                  v->replaceAll(sumOneCall_10);               
                  break;
                }
                case 11:{
                  auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_1_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
                  auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
                  v->replaceAll(sumOneCall_11); 
                  break;
                }
                case 100:{
                  auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_1_100", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
                  v->replaceAll(sumOneCall_100);               
                  break;
                }
                case 101:{
                  auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_1_101", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
                  v->replaceAll(sumOneCall_101);               
                  break;
                }
                case 110:{
                  auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_1_110", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
                  v->replaceAll(sumOneCall_110);               
                  break;
                }
                case 111:{
                  auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_1_111", {args_1->getType(), args_2->getType()});    
                  auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
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
              auto *sumOne_0 = M->getOrRealizeFunc("sumOne_min_0", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_0 = util::call(sumOne_0, {args_1, args_2});       
              v->replaceAll(sumOneCall_0);               
              break;
            }
            case 1:{
              auto *sumOne_1 = M->getOrRealizeFunc("sumOne_min_1", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_1 = util::call(sumOne_1, {args_1, args_2});       
              v->replaceAll(sumOneCall_1);               
              break;
            }
            case 10:{
              auto *sumOne_10 = M->getOrRealizeFunc("sumOne_min_10", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_10 = util::call(sumOne_10, {args_1, args_2});       
              v->replaceAll(sumOneCall_10);               
              break;
            }
            case 11:{
              auto *sumOne_11 = M->getOrRealizeFunc("sumOne_min_11", {args_1->getType(), args_2->getType(), M->getIntType()});    
              auto *sumOneCall_11 = util::call(sumOne_11, {args_1, args_2, zdrop_ptr});       
              v->replaceAll(sumOneCall_11); 
              break;
            }
            case 100:{
              auto *sumOne_100 = M->getOrRealizeFunc("sumOne_min_100", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_100 = util::call(sumOne_100, {args_1, args_2});       
              v->replaceAll(sumOneCall_100);               
              break;
            }
            case 101:{
              auto *sumOne_101 = M->getOrRealizeFunc("sumOne_min_101", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_101 = util::call(sumOne_101, {args_1, args_2});       
              v->replaceAll(sumOneCall_101);               
              break;
            }
            case 110:{
              auto *sumOne_110 = M->getOrRealizeFunc("sumOne_min_110", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_110 = util::call(sumOne_110, {args_1, args_2});       
              v->replaceAll(sumOneCall_110);               
              break;
            }
            case 111:{
              auto *sumOne_111 = M->getOrRealizeFunc("sumOne_min_111", {args_1->getType(), args_2->getType()});    
              auto *sumOneCall_111 = util::call(sumOne_111, {args_1, args_2});       
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


