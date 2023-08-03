from experimental.simd import *
from bio import *
import time
import os

def zdrop(a, b, c):
     if b - a <= c:
          return a
     else:
          return -32768

def listCR(len_1, len_2, first, row_base, row_mul, col_base, col_mul, rest):
     return [[first if (_ == 0 and __ == 0) else (row_base + _ * row_mul) if (__ == 0 and _ > 0) else (col_base + __ * col_mul) if (_ == 0 and __ > 0) else rest for _ in range(len_1)] for __ in range(len_2)]

def match_func(x, a, b, am, y):
     if str(x) == "N" or str(y) == "N":
          return am
     elif x == y:
          return a
     else:
          return b

def max_store(lst, ind_row, ind_col, val_1, val_2):
     lst[ind_row][ind_col] = max(val_1, val_2)
     return lst[ind_row][ind_col]

def max_val(a):
     mx = 0
     for i in range(len(a)):
          for j in range(len(a[i])):
               if mx < a[i][j]:
                    mx = a[i][j]
     return mx


def orig(h, m):
     q = listCR(len(h) + 1, len(m) + 1, 0, -4, -2, -4, -2, 0)
     l = listCR(len(h) + 1, len(m) + 1, -10000, -4, -2, -10000, 0, -10000)
     s = listCR(len(h) + 1, len(m) + 1, -10000, -10000, 0, -4, -2, -10000)
     for i in range(1, len(m) + 1):
          for j in range(1, len(h) + 1):       
               q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1]), max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6))                              
     return zdrop(q[-1][-1], max_val(q), 800)

def prep(x, y):
     score = [[0 for _ in range(len(y))] for __ in range(len(x))]
     for i in range(len(x)):
          for j in range(len(y)):
               score[i][j] = orig(x[i], y[j])
     return score

@staticmethod
@inline
def match_vec(checker, x_vec, y_vec, a, b, am, params_const, n):
     if checker == 0:
          return Vec(i16(params_const), i16, 16)
     else:
          return Vec.cmp_vec(x_vec, y_vec, a, b, am, n)

@staticmethod
@inline
def calculate(starget, squery, x, seqs_t, matrices, H, V, check, params, scores, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, checker_1, checker_2, checker_3, H_row, H_row_mul, mat_row, mat_row_mul, V_row, V_row_mul, first_list, second_list, third_list, mx1_checker, mx1_names, inds_mx1, mx2_checker, mx2_names, inds_mx2, mx3_checker, mx3_names, inds_mx3, z_value, max_score):     
     if checker_1 == 1:
          a_1 = Vec(i16(args1[1]), i16, 16)
          b_1 = Vec(i16(args1[3]), i16, 16)
          am_1 = Vec(i16(args1[2]), i16, 16)
          x_ind_1 = args1[4]
          y_ind_1 = args1[5]
     else:
          a_1 = Vec(i16(0), i16, 16)
          b_1 = Vec(i16(0), i16, 16)
          am_1 = Vec(i16(0), i16, 16)
          x_ind_1 = -1
          y_ind_1 = -1          
     if checker_2 == 1:
          a_2 = Vec(i16(args2[1]), i16, 16)
          b_2 = Vec(i16(args2[3]), i16, 16)
          am_2 = Vec(i16(args2[2]), i16, 16)   
          x_ind_2 = args2[4]
          y_ind_2 = args2[5] 
     else:
          a_2 = Vec(i16(0), i16, 16)
          b_2 = Vec(i16(0), i16, 16)
          am_2 = Vec(i16(0), i16, 16)  
          x_ind_2 = -1
          y_ind_2 = -1                  
     if checker_3 == 1:
          a_3 = Vec(i16(args3[1]), i16, 16)
          b_3 = Vec(i16(args3[3]), i16, 16)
          am_3 = Vec(i16(args3[2]), i16, 16)   
          x_ind_3 = args3[4]
          y_ind_3 = args3[5] 
     else:
          a_3 = Vec(i16(0), i16, 16)
          b_3 = Vec(i16(0), i16, 16)
          am_3 = Vec(i16(0), i16, 16)
          x_ind_3 = -1
          y_ind_3 = -1                               
     SEQ_NO_Q = len(squery)
     REG_SIZE = 16   
     c0 = 0
     c1 = 1     
     m_c0_x = matrices[c0].arr.ptr
     m_c1_x = matrices[c1].arr.ptr
     v_c0_x = V[c0].arr.ptr
     v_c1_x = V[c1].arr.ptr
     h_c0_x = H[c0].arr.ptr
     h_c1_x = H[c1].arr.ptr  
     decision = str(mx1_checker) + str(mx2_checker) + str(mx3_checker)
     if mx1_checker != 0:
          mx1_arg2_vec = Vec(i16(inds_mx1[4]), i16, 16)
          mx1_arg3_vec = Vec(i16(inds_mx1[7]), i16, 16)
          if inds_mx1[1] == -1:
               if mx1_names[0] == "M":
                    mx1_arg1_c0 = m_c0_x
                    mx1_arg1_c1 = m_c1_x
               elif mx1_names[0] == "V":
                    mx1_arg1_c0 = v_c0_x
                    mx1_arg1_c1 = v_c1_x
               else:
                    mx1_arg1_c0 = h_c0_x
                    mx1_arg1_c1 = h_c1_x
          else:               
               if mx1_names[0] == "M":
                    mx1_arg1_c0 = m_c1_x
                    mx1_arg1_c1 = m_c0_x
               elif mx1_names[0] == "V":
                    mx1_arg1_c0 = v_c1_x
                    mx1_arg1_c1 = v_c0_x
               else:
                    mx1_arg1_c0 = h_c1_x
                    mx1_arg1_c1 = h_c0_x

          if inds_mx1[3] == -1:
               if mx1_names[1] == "M":
                    mx1_arg2_c0 = m_c0_x
                    mx1_arg2_c1 = m_c1_x
               elif mx1_names[1] == "V":
                    mx1_arg2_c0 = v_c0_x
                    mx1_arg2_c1 = v_c1_x
               else:
                    mx1_arg2_c0 = h_c0_x
                    mx1_arg2_c1 = h_c1_x
          else:               
               if mx1_names[1] == "M":
                    mx1_arg2_c0 = m_c1_x
                    mx1_arg2_c1 = m_c0_x
               elif mx1_names[1] == "V":
                    mx1_arg2_c0 = v_c1_x
                    mx1_arg2_c1 = v_c0_x
               else:
                    mx1_arg2_c0 = h_c1_x
                    mx1_arg2_c1 = h_c0_x                         

          if inds_mx1[6] == -1:
               if mx1_names[2] == "M":
                    mx1_arg3_c0 = m_c0_x
                    mx1_arg3_c1 = m_c1_x
               elif mx1_names[2] == "V":
                    mx1_arg3_c0 = v_c0_x
                    mx1_arg3_c1 = v_c1_x
               else:
                    mx1_arg3_c0 = h_c0_x
                    mx1_arg3_c1 = h_c1_x
          else:               
               if mx1_names[2] == "M":
                    mx1_arg3_c0 = m_c1_x
                    mx1_arg3_c1 = m_c0_x
               elif mx1_names[2] == "V":
                    mx1_arg3_c0 = v_c1_x
                    mx1_arg3_c1 = v_c0_x
               else:
                    mx1_arg3_c0 = h_c1_x
                    mx1_arg3_c1 = h_c0_x              
     else:
          mx1_arg2_vec = Vec(i16(0), i16, 16)
          mx1_arg3_vec = Vec(i16(0), i16, 16)                    
          mx1_arg1_c0 = m_c0_x
          mx1_arg1_c1 = m_c1_x
          mx1_arg2_c0 = m_c0_x
          mx1_arg2_c1 = m_c1_x
          mx1_arg3_c0 = m_c0_x
          mx1_arg3_c1 = m_c1_x                    

     if mx2_checker != 0:
          mx2_arg2_vec = Vec(i16(inds_mx2[4]), i16, 16)
          mx2_arg3_vec = Vec(i16(inds_mx2[7]), i16, 16)                    
          if inds_mx2[1] == -1:
               if mx2_names[0] == "M":
                    mx2_arg1_c0 = m_c0_x
                    mx2_arg1_c1 = m_c1_x
               elif mx2_names[0] == "V":
                    mx2_arg1_c0 = v_c0_x
                    mx2_arg1_c1 = v_c1_x
               else:
                    mx2_arg1_c0 = h_c0_x
                    mx2_arg1_c1 = h_c1_x
          else:               
               if mx2_names[0] == "M":  
                    mx2_arg1_c0 = m_c1_x
                    mx2_arg1_c1 = m_c0_x
               elif mx2_names[0] == "V":
                    mx2_arg1_c0 = v_c1_x
                    mx2_arg1_c1 = v_c0_x
               else:
                    mx2_arg1_c0 = h_c1_x
                    mx2_arg1_c1 = h_c0_x

          if inds_mx2[3] == -1:               
               if mx2_names[1] == "M":
                    mx2_arg2_c0 = m_c0_x
                    mx2_arg2_c1 = m_c1_x
               elif mx2_names[1] == "V":
                    mx2_arg2_c0 = v_c0_x
                    mx2_arg2_c1 = v_c1_x
               else:
                    mx2_arg2_c0 = h_c0_x
                    mx2_arg2_c1 = h_c1_x
          else:               
               if mx2_names[1] == "M":
                    mx2_arg2_c0 = m_c1_x
                    mx2_arg2_c1 = m_c0_x
               elif mx2_names[1] == "V":
                    mx2_arg2_c0 = v_c1_x
                    mx2_arg2_c1 = v_c0_x
               else:
                    mx2_arg2_c0 = h_c1_x
                    mx2_arg2_c1 = h_c0_x                         

          if inds_mx2[6] == -1:
               if mx2_names[2] == "M":
                    mx2_arg3_c0 = m_c0_x
                    mx2_arg3_c1 = m_c1_x
               elif mx2_names[2] == "V":
                    mx2_arg3_c0 = v_c0_x
                    mx2_arg3_c1 = v_c1_x
               else:
                    mx2_arg3_c0 = h_c0_x
                    mx2_arg3_c1 = h_c1_x
          else:               
               if mx2_names[2] == "M":
                    mx2_arg3_c0 = m_c1_x
                    mx2_arg3_c1 = m_c0_x
               elif mx2_names[2] == "V":
                    mx2_arg3_c0 = v_c1_x
                    mx2_arg3_c1 = v_c0_x
               else:
                    mx2_arg3_c0 = h_c1_x
                    mx2_arg3_c1 = h_c0_x  
     else:
          mx2_arg2_vec = Vec(i16(0), i16, 16)
          mx2_arg3_vec = Vec(i16(0), i16, 16)                              
          mx2_arg1_c0 = m_c0_x
          mx2_arg1_c1 = m_c1_x
          mx2_arg2_c0 = m_c0_x
          mx2_arg2_c1 = m_c1_x
          mx2_arg3_c0 = m_c0_x
          mx2_arg3_c1 = m_c1_x   
     if mx3_checker != 0:
          mx3_arg2_vec = Vec(i16(inds_mx3[4]), i16, 16)
          mx3_arg3_vec = Vec(i16(inds_mx3[7]), i16, 16)                    
          if inds_mx3[1] == -1:
               if mx3_names[0] == "M":
                    mx3_arg1_c0 = m_c0_x
                    mx3_arg1_c1 = m_c1_x
               elif mx3_names[0] == "V":
                    mx3_arg1_c0 = v_c0_x
                    mx3_arg1_c1 = v_c1_x
               else:
                    mx3_arg1_c0 = h_c0_x
                    mx3_arg1_c1 = h_c1_x
          else:               
               if mx3_names[0] == "M":  
                    mx3_arg1_c0 = m_c1_x
                    mx3_arg1_c1 = m_c0_x
               elif mx3_names[0] == "V":
                    mx3_arg1_c0 = v_c1_x
                    mx3_arg1_c1 = v_c0_x
               else:
                    mx3_arg1_c0 = h_c1_x
                    mx3_arg1_c1 = h_c0_x

          if inds_mx3[3] == -1:               
               if mx3_names[1] == "M":
                    mx3_arg2_c0 = m_c0_x
                    mx3_arg2_c1 = m_c1_x
               elif mx3_names[1] == "V":
                    mx3_arg2_c0 = v_c0_x
                    mx3_arg2_c1 = v_c1_x
               else:
                    mx3_arg2_c0 = h_c0_x
                    mx3_arg2_c1 = h_c1_x
          else:               
               if mx3_names[1] == "M":
                    mx3_arg2_c0 = m_c1_x
                    mx3_arg2_c1 = m_c0_x
               elif mx3_names[1] == "V":
                    mx3_arg2_c0 = v_c1_x
                    mx3_arg2_c1 = v_c0_x
               else:
                    mx3_arg2_c0 = h_c1_x
                    mx3_arg2_c1 = h_c0_x                         

          if inds_mx3[6] == -1:
               if mx3_names[2] == "M":
                    mx3_arg3_c0 = m_c0_x
                    mx3_arg3_c1 = m_c1_x
               elif mx3_names[2] == "V":
                    mx3_arg3_c0 = v_c0_x
                    mx3_arg3_c1 = v_c1_x
               else:
                    mx3_arg3_c0 = h_c0_x
                    mx3_arg3_c1 = h_c1_x
          else:               
               if mx3_names[2] == "M":
                    mx3_arg3_c0 = m_c1_x
                    mx3_arg3_c1 = m_c0_x
               elif mx3_names[2] == "V":
                    mx3_arg3_c0 = v_c1_x
                    mx3_arg3_c1 = v_c0_x
               else:
                    mx3_arg3_c0 = h_c1_x
                    mx3_arg3_c1 = h_c0_x  
     else:
          mx3_arg2_vec = Vec(i16(0), i16, 16)
          mx3_arg3_vec = Vec(i16(0), i16, 16)                              
          mx3_arg1_c0 = m_c0_x
          mx3_arg1_c1 = m_c1_x
          mx3_arg2_c0 = m_c0_x
          mx3_arg2_c1 = m_c1_x
          mx3_arg3_c0 = m_c0_x
          mx3_arg3_c1 = m_c1_x                                                                                                    
     if params[16] == -1:          
          if first_list == "M":               
               arg_1_c0 = m_c0_x
               arg_1_c1 = m_c1_x
          elif first_list == "V":               
               arg_1_c0 = v_c0_x
               arg_1_c1 = v_c1_x    
          else:
               arg_1_c0 = h_c0_x
               arg_1_c1 = h_c1_x                              
     else:
          if first_list == "M":
               arg_1_c0 = m_c1_x
               arg_1_c1 = m_c0_x
          elif first_list == "V":
               arg_1_c0 = v_c1_x
               arg_1_c1 = v_c0_x    
          else:
               arg_1_c0 = h_c1_x
               arg_1_c1 = h_c0_x
     if params[22] == -1:
          if second_list == "M":
               arg_2_c0 = m_c0_x
               arg_2_c1 = m_c1_x
          elif second_list == "V":
               arg_2_c0 = v_c0_x
               arg_2_c1 = v_c1_x    
          else:
               arg_2_c0 = h_c0_x
               arg_2_c1 = h_c1_x                              
     else:
          if second_list == "M":
               arg_2_c0 = m_c1_x
               arg_2_c1 = m_c0_x
          elif second_list == "V":
               arg_2_c0 = v_c1_x
               arg_2_c1 = v_c0_x    
          else:
               arg_2_c0 = h_c1_x
               arg_2_c1 = h_c0_x
     if params[11] != -1:
          if params[26] == -1:
               if third_list == "M":
                    arg_3_c0 = m_c0_x
                    arg_3_c1 = m_c1_x
               elif third_list == "V":
                    arg_3_c0 = v_c0_x
                    arg_3_c1 = v_c1_x    
               else:
                    arg_3_c0 = h_c0_x
                    arg_3_c1 = h_c1_x                              
          else:
               if third_list == "M":
                    arg_3_c0 = m_c1_x
                    arg_3_c1 = m_c0_x
               elif third_list == "V":
                    arg_3_c0 = v_c1_x
                    arg_3_c1 = v_c0_x    
               else:
                    arg_3_c0 = h_c1_x
                    arg_3_c1 = h_c0_x    
     x_vec = x.arr.ptr
     n = Vec(u8(ord("N")), u8, 16)
     match decision:
          case "000":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   if params[11] != -1:
                                        Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   if params[11] != -1:                                   
                                        Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]) , ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_1_c0, arg_1_c1 = arg_1_c1, arg_1_c0
                    arg_2_c0, arg_2_c1 = arg_2_c1, arg_2_c0
                    if params[11] != -1:
                         arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0                       
          case "001":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_1_c0, arg_1_c1 = arg_1_c1, arg_1_c0
                    arg_2_c0, arg_2_c1 = arg_2_c1, arg_2_c0
                    if params[11] != -1:
                         mx3_arg1_c0, mx3_arg1_c1 = mx3_arg1_c1, mx3_arg1_c0
                         mx3_arg2_c0, mx3_arg2_c1 = mx3_arg2_c1, mx3_arg2_c0   
                         mx3_arg3_c0, mx3_arg3_c1 = mx3_arg3_c1, mx3_arg3_c0                         
          case "010":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   if params[11] != -1:
                                        Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   if params[11] != -1:                                   
                                        Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_1_c0, arg_1_c1 = arg_1_c1, arg_1_c0
                    mx2_arg1_c0, mx2_arg1_c1 = mx2_arg1_c1, mx2_arg1_c0
                    mx2_arg2_c0, mx2_arg2_c1 = mx2_arg2_c1, mx2_arg2_c0
                    mx2_arg3_c0, mx2_arg3_c1 = mx2_arg3_c1, mx2_arg3_c0
                    if params[11] != -1:
                         arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0                    
          case "011":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:                                   
                                   Vec.min(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:                                   
                                   Vec.max(((arg_1_c0[current_y + params[14]][ind_y] * i16(params[9])) + match_vec(checker_1, x_vec[current_x + x_ind_1], s_1[ind_y], a_1, b_1, am_1, params[17], n)) * i16(params[12]), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_1_c0, arg_1_c1 = arg_1_c1, arg_1_c0
                    mx2_arg1_c0, mx2_arg1_c1 = mx2_arg1_c1, mx2_arg1_c0
                    mx2_arg2_c0, mx2_arg2_c1 = mx2_arg2_c1, mx2_arg2_c0
                    mx2_arg3_c0, mx2_arg3_c1 = mx2_arg3_c1, mx2_arg3_c0
                    if params[11] != -1:
                         mx3_arg1_c0, mx3_arg1_c1 = mx3_arg1_c1, mx3_arg1_c0
                         mx3_arg2_c0, mx3_arg2_c1 = mx3_arg2_c1, mx3_arg2_c0   
                         mx3_arg3_c0, mx3_arg3_c1 = mx3_arg3_c1, mx3_arg3_c0                 
          case "100":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   if params[11] != -1:
                                        Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   if params[11] != -1:                                   
                                        Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)) * i16(params[24]), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_2_c0, arg_2_c1 = arg_2_c1, arg_2_c0
                    mx1_arg1_c0, mx1_arg1_c1 = mx1_arg1_c1, mx1_arg1_c0
                    mx1_arg2_c0, mx1_arg2_c1 = mx1_arg2_c1, mx1_arg2_c0
                    mx1_arg3_c0, mx1_arg3_c1 = mx1_arg3_c1, mx1_arg3_c0
                    if params[11] != -1:
                         arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0                         
          case "101":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), ((arg_2_c0[current_y + params[20]][ind_y] * i16(params[10])) + match_vec(checker_2, x_vec[current_x + x_ind_2], s_2[ind_y], a_2, b_2, am_2, params[23], n)) * i16(params[18]), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    arg_2_c0, arg_2_c1 = arg_2_c1, arg_2_c0
                    mx1_arg1_c0, mx1_arg1_c1 = mx1_arg1_c1, mx1_arg1_c0
                    mx1_arg2_c0, mx1_arg2_c1 = mx1_arg2_c1, mx1_arg2_c0
                    mx1_arg3_c0, mx1_arg3_c1 = mx1_arg3_c1, mx1_arg3_c0
                    if params[11] != -1:
                         mx3_arg1_c0, mx3_arg1_c1 = mx3_arg1_c1, mx3_arg1_c0
                         mx3_arg2_c0, mx3_arg2_c1 = mx3_arg2_c1, mx3_arg2_c0   
                         mx3_arg3_c0, mx3_arg3_c1 = mx3_arg3_c1, mx3_arg3_c0                         
          case "110":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   if params[11] != -1:
                                        Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   if params[11] != -1:                                   
                                        Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), ((arg_3_c0[current_y + params[26]][ind_y] * i16(params[11])) + match_vec(checker_3, x_vec[current_x + x_ind_3], s_3[ind_y], a_3, b_3, am_3, params[29], n)), max_score, ind_y, m_c1_x[current_y])
                                   else:
                                        Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    mx1_arg1_c0, mx1_arg1_c1 = mx1_arg1_c1, mx1_arg1_c0
                    mx1_arg2_c0, mx1_arg2_c1 = mx1_arg2_c1, mx1_arg2_c0
                    mx1_arg3_c0, mx1_arg3_c1 = mx1_arg3_c1, mx1_arg3_c0
                    mx2_arg1_c0, mx2_arg1_c1 = mx2_arg1_c1, mx2_arg1_c0
                    mx2_arg2_c0, mx2_arg2_c1 = mx2_arg2_c1, mx2_arg2_c0
                    mx2_arg3_c0, mx2_arg3_c1 = mx2_arg3_c1, mx2_arg3_c0
                    if params[11] != -1:
                         arg_3_c0, arg_3_c1 = arg_3_c1, arg_3_c0                      
          case "111":
               for current_x in range(outer_start, outer_stop, outer_step):
                    for current_y in range(inner_start, inner_stop, inner_step):
                         s_1 = seqs_t[1][current_y + y_ind_1] 
                         s_2 = seqs_t[1][current_y + y_ind_2] 
                         s_3 = seqs_t[1][current_y + y_ind_3]               
                         for ind_y in range(check):
                              if params[8] == 0:
                                   Vec.min((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                              else:
                                   Vec.max((Vec.max_store_vec(mx1_arg1_c0[current_y + inds_mx1[0]], ind_y, mx1_arg2_c0[current_y + inds_mx1[2]], mx1_arg2_vec, mx1_arg3_c0[current_y + inds_mx1[5]], mx1_arg3_vec)), (Vec.max_store_vec(mx2_arg1_c0[current_y + inds_mx2[0]], ind_y, mx2_arg2_c0[current_y + inds_mx2[2]], mx2_arg2_vec, mx2_arg3_c0[current_y + inds_mx2[5]], mx2_arg3_vec)), (Vec.max_store_vec(mx3_arg1_c0[current_y + inds_mx3[0]], ind_y, mx3_arg2_c0[current_y + inds_mx3[2]], mx3_arg2_vec, mx3_arg3_c0[current_y + inds_mx3[5]], mx3_arg3_vec)), max_score, ind_y, m_c1_x[current_y])
                    for i in range(check):
                         matrices[c0][0][i] = matrices[c1][0][i] + mat_row_mul
                         H[c0][0][i] = H[c1][0][i] + H_row_mul
                         V[c0][0][i] = V[c1][0][i] + V_row_mul
                    c1, c0 = c0, c1               
                    v_c0_x, v_c1_x = v_c1_x, v_c0_x
                    h_c0_x, h_c1_x = h_c1_x, h_c0_x
                    m_c1_x, m_c0_x = m_c0_x, m_c1_x
                    mx1_arg1_c0, mx1_arg1_c1 = mx1_arg1_c1, mx1_arg1_c0
                    mx1_arg2_c0, mx1_arg2_c1 = mx1_arg2_c1, mx1_arg2_c0
                    mx1_arg3_c0, mx1_arg3_c1 = mx1_arg3_c1, mx1_arg3_c0
                    mx2_arg1_c0, mx2_arg1_c1 = mx2_arg1_c1, mx2_arg1_c0
                    mx2_arg2_c0, mx2_arg2_c1 = mx2_arg2_c1, mx2_arg2_c0
                    mx2_arg3_c0, mx2_arg3_c1 = mx2_arg3_c1, mx2_arg3_c0
                    if params[11] != -1:
                         mx3_arg1_c0, mx3_arg1_c1 = mx3_arg1_c1, mx3_arg1_c0
                         mx3_arg2_c0, mx3_arg2_c1 = mx3_arg2_c1, mx3_arg2_c0   
                         mx3_arg3_c0, mx3_arg3_c1 = mx3_arg3_c1, mx3_arg3_c0                         
     if z_value != i16(-1):
          for i in range(SEQ_NO_Q):
               ind = i // REG_SIZE
               remain = i - (ind * REG_SIZE)
               Vec.assign_am(matrices[c0][-1], ind, remain, scores, i, max_score, z_value)

     else:     
          for i in range(SEQ_NO_Q):
               ind = i // REG_SIZE
               remain = i - (ind * REG_SIZE)   
               Vec.assign(matrices[c0][-1], ind, remain, scores, i)
     return scores               


@staticmethod
@inline
def y_gen(seqs):
    num = 0
    for row in range(num + 1, len(seqs)):
        if len(seqs[row][0]) != len(seqs[num][0]):
            yield seqs[num: row]
            num = row
    yield seqs[num:] 

def sumOne(s_x, s_y):
     z_value = i16(-1)
     REG_SIZE = 16
     checker_1 = 0
     checker_2 = 0
     checker_3 = 0
     args1 = [0, 0, 0, 0, 0, 0]
     args2 = [0, 0, 0, 0, 0, 0]
     args3 = [0, 0, 0, 0, 0, 0]
     try:
          with open("arg_1.txt") as f:
               args1 = [int(_) for _ in f] 
          checker_1 = 1
     except:
          checker_1 = 0
     try:
          with open("arg_2.txt") as f:
               args2 = [int(_) for _ in f]      
          checker_2 = 1
     except:
          checker_2 = 0
     try:
          with open("arg_3.txt") as f:               
               args3 = [int(_) for _ in f]  
          checker_3 = 1                     
     except:
          checker_3 = 0
     try:
          with open("zdrop.txt") as f:               
               temp_int = [int(_) for _ in f]
          z_value = i16(temp_int[0])
     except:
          z_value = i16(-1)
     temp_loop = [""]
     with open("LoopInfo.txt") as f:
          temp_loop = [str(_) for _ in f]
     count_3 = int(temp_loop[11])
     if count_3 != -1:
          params = [int(temp_loop[_]) for _ in range(len(temp_loop[:-4]))]
          third_param = temp_loop[-2][:-1]  
          second_param = temp_loop[-3][:-1]
          first_param = temp_loop[-4][:-1]               
     else:
          params = [int(temp_loop[_]) for _ in range(len(temp_loop[:-3]))]
          third_param = "0"          
          second_param = temp_loop[-2][:-1]
          first_param = temp_loop[-3][:-1]                         
     left_param = temp_loop[-1][:-1]
     outer_start = params[0]
     outer_step = params[1]
     outer_stop = 0
     if params[2] == -1:
          outer_stop += len(s_x[0])
     elif params[2] == -2:
          outer_stop += len(s_y[0])
     outer_stop += params[3]
     inner_start = params[4]
     inner_step = params[5]
     inner_stop = 0
     if params[6] == -1:
          inner_stop += len(s_x[0])
     elif params[6] == -2:
          inner_stop += len(s_y[0])
     inner_stop += params[7]  
     inds_mx1 = [0, 0, 0, 0, 0, 0, 0, 0]
     mx1_names = [""]
     mx1_checker = 0
     inds_mx2 = [0, 0, 0, 0, 0, 0, 0, 0]
     mx2_names = [""]
     mx2_checker = 0
     inds_mx2 = [0, 0, 0, 0, 0, 0, 0, 0]
     mx2_names = [""]
     mx2_checker = 0     
     try:
          temp_mx1 = [""]
          with open("mx_arg1.txt") as f:
               temp_mx1 = [str(_) for _ in f]        
          inds_mx1 = [int(temp_mx1[1]), int(temp_mx1[2]), int(temp_mx1[4]), int(temp_mx1[5]), int(temp_mx1[6]), int(temp_mx1[8]), int(temp_mx1[9]), int(temp_mx1[10])]
          mx1_names = [temp_mx1[0][:-1], temp_mx1[3][:-1], temp_mx1[7][:-1]]
          mx1_checker = 1
     except:
          mx1_checker = 0
     try:
          temp_mx2 = [""]
          with open("mx_arg2.txt") as f:
               temp_mx2 = [str(_) for _ in f]                  
          inds_mx2 = [int(temp_mx2[1]), int(temp_mx2[2]), int(temp_mx2[4]), int(temp_mx2[5]), int(temp_mx2[6]), int(temp_mx2[8]), int(temp_mx2[9]), int(temp_mx2[10])]
          mx2_names = [temp_mx2[0][:-1], temp_mx2[3][:-1], temp_mx2[7][:-1]]
          mx2_checker = 1
     except:
          mx2_checker = 0
     try:
          temp_mx3 = [""]
          with open("mx_arg3.txt") as f:
               temp_mx3 = [str(_) for _ in f]                     
          inds_mx3 = [int(temp_mx3[1]), int(temp_mx3[2]), int(temp_mx3[4]), int(temp_mx3[5]), int(temp_mx3[6]), int(temp_mx3[8]), int(temp_mx3[9]), int(temp_mx3[10])]
          mx3_names = [temp_mx3[0][:-1], temp_mx3[3][:-1], temp_mx3[7][:-1]]
          mx3_checker = 1
     except:
          mx3_checker = 0                    
     try:
          temp_lst1 = [""]
          with open("lst_1.txt") as f:
               temp_lst1 = [str(_) for _ in f]             
          inds_1 = [int(temp_lst1[_ + 1]) for _ in range(len(temp_lst1) - 1)]
          lst_1_name = temp_lst1[0][:-1]
     except:
          inds_1 = [0 for _ in range(10)]
          lst_1_name = ""
     try:
          temp_lst2 = [""]
          with open("lst_2.txt") as f:
               temp_lst2 = [str(_) for _ in f]                   
          inds_2 = [int(temp_lst2[_ + 1]) for _ in range(len(temp_lst2) - 1)]
          lst_2_name = temp_lst2[0][:-1]
     except:
          inds_2 = [0 for _ in range(10)]
          lst_2_name = ""
     try:
          temp_lst3 = [""]
          with open("lst_3.txt") as f:
               temp_lst3 = [str(_) for _ in f]        
          inds_3 = [int(temp_lst3[_ + 1]) for _ in range(len(temp_lst3) - 1)]
          lst_3_name = temp_lst3[0][:-1]
     except:
          inds_3 = [0 for _ in range(10)]
          lst_3_name = ""
     lens = [len(s_x[0]), len(s_y[0])]
     inds = [0]
     inds_H = [0]
     inds_V = [0]
     if left_param == lst_1_name:
          inds = [lens[inds_1[0] -1] + inds_1[1], lens[inds_1[2] -1] + inds_1[3], inds_1[4], inds_1[5], inds_1[6], inds_1[7], inds_1[8], inds_1[9]]
          inds_H = [lens[inds_2[0] -1] + inds_2[1], lens[inds_2[2] -1] + inds_2[3], inds_2[4], inds_2[5], inds_2[6], inds_2[7], inds_2[8], inds_2[9]]         
          inds_V = [lens[inds_3[0] -1] + inds_3[1], lens[inds_3[2] -1] + inds_3[3], inds_3[4], inds_3[5], inds_3[6], inds_3[7], inds_3[8], inds_3[9]]                
          first_list = "M" if first_param == lst_1_name else "H" if first_param == lst_2_name else "V" if first_param == lst_3_name else "0"
          second_list = "M" if second_param == lst_1_name else "H" if second_param == lst_2_name else "V" if second_param == lst_3_name else "0"
          third_list = "M" if third_param == lst_1_name else "H" if third_param == lst_2_name else "V" if third_param == lst_3_name else "0"
          if mx1_checker != 0:
               mx1_names[0] = "M" if mx1_names[0] == lst_1_name else "H" if mx1_names[0] == lst_2_name else "V" if mx1_names[0] == lst_3_name else "0"
               mx1_names[1] = "M" if mx1_names[1] == lst_1_name else "H" if mx1_names[1] == lst_2_name else "V" if mx1_names[1] == lst_3_name else "0"
               mx1_names[2] = "M" if mx1_names[2] == lst_1_name else "H" if mx1_names[2] == lst_2_name else "V" if mx1_names[2] == lst_3_name else "0"
          if mx2_checker != 0:
               mx2_names[0] = "M" if mx2_names[0] == lst_1_name else "H" if mx2_names[0] == lst_2_name else "V" if mx2_names[0] == lst_3_name else "0"
               mx2_names[1] = "M" if mx2_names[1] == lst_1_name else "H" if mx2_names[1] == lst_2_name else "V" if mx2_names[1] == lst_3_name else "0"
               mx2_names[2] = "M" if mx2_names[2] == lst_1_name else "H" if mx2_names[2] == lst_2_name else "V" if mx2_names[2] == lst_3_name else "0"               
          if mx3_checker != 0:
               mx3_names[0] = "M" if mx3_names[0] == lst_1_name else "H" if mx3_names[0] == lst_2_name else "V" if mx3_names[0] == lst_3_name else "0"
               mx3_names[1] = "M" if mx3_names[1] == lst_1_name else "H" if mx3_names[1] == lst_2_name else "V" if mx3_names[1] == lst_3_name else "0"
               mx3_names[2] = "M" if mx3_names[2] == lst_1_name else "H" if mx3_names[2] == lst_2_name else "V" if mx3_names[2] == lst_3_name else "0"               
     elif left_param == lst_2_name:     
          inds = [lens[inds_2[0] -1] + inds_2[1], lens[inds_2[2] -1] + inds_2[3], inds_2[4], inds_2[5], inds_2[6], inds_2[7], inds_2[8], inds_2[9]]
          inds_H = [lens[inds_1[0] -1] + inds_1[1], lens[inds_1[2] -1] + inds_1[3], inds_1[4], inds_1[5], inds_1[6], inds_1[7], inds_1[8], inds_1[9]]
          inds_V = [lens[inds_3[0] -1] + inds_3[1], lens[inds_3[2] -1] + inds_3[3], inds_3[4], inds_3[5], inds_3[6], inds_3[7], inds_3[8], inds_3[9]]
          first_list = "H" if first_param == lst_1_name else "M" if first_param == lst_2_name else "V" if first_param == lst_3_name else "0"
          second_list = "H" if second_param == lst_1_name else "M" if second_param == lst_2_name else "V" if second_param == lst_3_name else "0"
          third_list = "H" if third_param == lst_1_name else "M" if third_param == lst_2_name else "V" if third_param == lst_3_name else "0"
          if mx1_checker != 0:
               mx1_names[0] = "H" if mx1_names[0] == lst_1_name else "M" if mx1_names[0] == lst_2_name else "V" if mx1_names[0] == lst_3_name else "0"
               mx1_names[1] = "H" if mx1_names[1] == lst_1_name else "M" if mx1_names[1] == lst_2_name else "V" if mx1_names[1] == lst_3_name else "0"
               mx1_names[2] = "H" if mx1_names[2] == lst_1_name else "M" if mx1_names[2] == lst_2_name else "V" if mx1_names[2] == lst_3_name else "0"
          if mx2_checker != 0:
               mx2_names[0] = "H" if mx2_names[0] == lst_1_name else "M" if mx2_names[0] == lst_2_name else "V" if mx2_names[0] == lst_3_name else "0"
               mx2_names[1] = "H" if mx2_names[1] == lst_1_name else "M" if mx2_names[1] == lst_2_name else "V" if mx2_names[1] == lst_3_name else "0"
               mx2_names[2] = "H" if mx2_names[2] == lst_1_name else "M" if mx2_names[2] == lst_2_name else "V" if mx2_names[2] == lst_3_name else "0"               
          if mx3_checker != 0:
               mx3_names[0] = "H" if mx3_names[0] == lst_1_name else "M" if mx3_names[0] == lst_2_name else "V" if mx3_names[0] == lst_3_name else "0"
               mx3_names[1] = "H" if mx3_names[1] == lst_1_name else "M" if mx3_names[1] == lst_2_name else "V" if mx3_names[1] == lst_3_name else "0"
               mx3_names[2] = "H" if mx3_names[2] == lst_1_name else "M" if mx3_names[2] == lst_2_name else "V" if mx3_names[2] == lst_3_name else "0" 

     elif left_param == lst_3_name:      
          inds = [lens[inds_3[0] -1] + inds_3[1], lens[inds_3[2] -1] + inds_3[3], inds_3[4], inds_3[5], inds_3[6], inds_3[7], inds_3[8], inds_3[9]]
          inds_H = [lens[inds_2[0] -1] + inds_2[1], lens[inds_2[2] -1] + inds_2[3], inds_2[4], inds_2[5], inds_2[6], inds_2[7], inds_2[8], inds_2[9]]
          inds_V = [lens[inds_1[0] -1] + inds_1[1], lens[inds_1[2] -1] + inds_1[3], inds_1[4], inds_1[5], inds_1[6], inds_1[7], inds_1[8], inds_1[9]]
          first_list = "V" if first_param == lst_1_name else "H" if first_param == lst_2_name else "M" if first_param == lst_3_name else "0"
          second_list = "V" if second_param == lst_1_name else "H" if second_param == lst_2_name else "M" if second_param == lst_3_name else "0"
          third_list = "V" if third_param == lst_1_name else "H" if third_param == lst_2_name else "M" if third_param == lst_3_name else "0"
          if mx1_checker != 0:
               mx1_names[0] = "V" if mx1_names[0] == lst_1_name else "H" if mx1_names[0] == lst_2_name else "M" if mx1_names[0] == lst_3_name else "0"
               mx1_names[1] = "V" if mx1_names[1] == lst_1_name else "H" if mx1_names[1] == lst_2_name else "M" if mx1_names[1] == lst_3_name else "0"
               mx1_names[2] = "V" if mx1_names[2] == lst_1_name else "H" if mx1_names[2] == lst_2_name else "M" if mx1_names[2] == lst_3_name else "0"
          if mx2_checker != 0:
               mx2_names[0] = "V" if mx2_names[0] == lst_1_name else "H" if mx2_names[0] == lst_2_name else "M" if mx2_names[0] == lst_3_name else "0"
               mx2_names[1] = "V" if mx2_names[1] == lst_1_name else "H" if mx2_names[1] == lst_2_name else "M" if mx2_names[1] == lst_3_name else "0"
               mx2_names[2] = "V" if mx2_names[2] == lst_1_name else "H" if mx2_names[2] == lst_2_name else "M" if mx2_names[2] == lst_3_name else "0"               
          if mx3_checker != 0:
               mx3_names[0] = "V" if mx3_names[0] == lst_1_name else "H" if mx3_names[0] == lst_2_name else "M" if mx3_names[0] == lst_3_name else "0"
               mx3_names[1] = "V" if mx3_names[1] == lst_1_name else "H" if mx3_names[1] == lst_2_name else "M" if mx3_names[1] == lst_3_name else "0"
               mx3_names[2] = "V" if mx3_names[2] == lst_1_name else "H" if mx3_names[2] == lst_2_name else "M" if mx3_names[2] == lst_3_name else "0"           
     mat_00 = Vec(i16(inds[2]), i16, 16)
     mat_row = Vec(i16(inds[3]), i16, 16)
     mat_row_mul = Vec(i16(inds[4]), i16, 16)
     mat_col = Vec(i16(inds[5]), i16, 16)
     mat_col_mul = Vec(i16(inds[6]), i16, 16)
     mat_rest = Vec(i16(inds[7]), i16, 16)
     H_00 = Vec(i16(inds_H[2]), i16, 16)
     H_row = Vec(i16(inds_H[3]), i16, 16)
     H_row_mul = Vec(i16(inds_H[4]), i16, 16)
     H_col = Vec(i16(inds_H[5]), i16, 16)
     H_col_mul = Vec(i16(inds_H[6]), i16, 16)
     H_rest = Vec(i16(inds_H[7]), i16, 16)    
     V_00 = Vec(i16(inds_V[2]), i16, 16)
     V_row = Vec(i16(inds_V[3]), i16, 16)
     V_row_mul = Vec(i16(inds_V[4]), i16, 16)
     V_col = Vec(i16(inds_V[5]), i16, 16)
     V_col_mul = Vec(i16(inds_V[6]), i16, 16)
     V_rest = Vec(i16(inds_V[7]), i16, 16)  
     with open("Prep_info.txt") as f:
          hyper_params = [int(_) for _ in f]
     major_outer_start = hyper_params[0]
     major_outer_step = hyper_params[1]
     major_outer_inst = hyper_params[3]
     major_inner_start = hyper_params[4]
     major_inner_step = hyper_params[5]
     major_inner_inst = hyper_params[7]   
     major_params_1 = s_x[major_outer_start: len(s_x) + major_outer_inst: major_outer_step]
     major_params_2 = s_y[major_inner_start: len(s_y) + major_inner_inst: major_inner_step]      
     if hyper_params[2] == -2:
          major_params_1 = s_y[major_outer_start: len(s_y) + major_outer_inst: major_outer_step]
     if hyper_params[6] == -1:
          major_params_2 = s_x[major_inner_start: len(s_x) + major_inner_inst: major_inner_step]
     SEQ_NO_Q = len(major_params_2)
     for x in major_params_1:
          score = [i16(0) for _ in range(len(major_params_2))]                
          x_vec = [Vec(u8(ord(str(x[_]))), u8, 16) for _ in range(len(x))]
          for row in y_gen(major_params_2):
               MAX_LENGTH_Q = len(row[0])
               check = ((len(row)) // REG_SIZE)
               if len(row) % 16 != 0:
                    check += 1    
               max_score = [Vec(i16(0), i16, 16) for _ in range(check)]                             
               matrices = [[[mat_00 if ___ == 0 and __ == 0 else mat_col + mat_col_mul * i16(__) if ___ == 0 and __ > 0 else mat_row_mul + mat_row if ___ > 0 and __ == 0 else mat_rest  for _ in range(check)] for __ in range(inds[1])] for ___ in range(2)]          
               H = [[[H_00 if ___ == 0 and __ == 0 else H_col + H_col_mul * i16(__) if ___ == 0 and __ > 0 else H_row_mul + H_row if ___ > 0 and __ == 0 else H_rest  for _ in range(check)] for __ in range(inds_H[1])] for ___ in range(2)]
               V = [[[V_00 if ___ == 0 and __ == 0 else V_col + V_col_mul * i16(__) if ___ == 0 and __ > 0 else V_row_mul + V_row if ___ > 0 and __ == 0 else V_rest  for _ in range(check)] for __ in range(inds_V[1])] for ___ in range(2)]
               t = [[u8(ord(str(row[_][__]))) if _ < SEQ_NO_Q else u8(0) for _ in range(SEQ_NO_Q + REG_SIZE)] for __ in range(MAX_LENGTH_Q)]
               seqs_t = tuple(list([[Vec(t[__][(_ * REG_SIZE): (_ + 1) * REG_SIZE], u8, 16) for _ in range(check)] for __ in range(MAX_LENGTH_Q)]))         
               score = calculate(x, row, x_vec, seqs_t, matrices, H, V, check, params, score, outer_start, outer_stop, outer_step, inner_start, inner_stop, inner_step, args1, args2, args3, checker_1, checker_2, checker_3, H_row, H_row_mul, mat_row, mat_row_mul, V_row, V_row_mul, first_list, second_list, third_list, mx1_checker, mx1_names, inds_mx1, mx2_checker, mx2_names, inds_mx2, mx3_checker, mx3_names, inds_mx3, z_value, max_score)
          for i in range(len(score)):
               print(int(score[i]))
     return score



SEQ_NO_T = 64
SEQ_NO_Q = 64
seqs_x = [s"" for _ in range(SEQ_NO_T)]
f = open("seqx.txt", "r")
count = 0
for i in f:
    seqs_x[count] = seq(i[:-1])
    count += 1
f.close()
f = open("seqy.txt", "r")
count = 0
seqs_y = [s"" for _ in range(SEQ_NO_Q)]    
for j in f:
    seqs_y[count] = seq(j[:-1])
    count += 1
f.close()


print("BEGUN")
with time.timing("Total: "):
     d = prep(seqs_x, seqs_y)
print("DONE")