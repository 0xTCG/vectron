from experimental.simd import *
from bio import *
import time
import os

def byPass(a, b, c):
     if b - a <= c:
          return a
     else:
          return -32768

def listCR(len_1, len_2, first, row_base, row_mul, col_base, col_mul, rest):
     return [[first if (_ == 0 and __ == 0) else (row_base + _ * row_mul) if (__ == 0 and _ > 0) else (col_base + __ * col_mul) if (_ == 0 and __ > 0) else rest for __ in range(len_1)] for _ in range(len_2)]

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
     q = listCR(len(h) + 1, len(m) + 1, 0, 1, 2, 1, 3, 0)
     for i in range(1, len(m) + 1):
          for j in range(1, len(h) + 1):       
                q[i][j] = max(q[i - 1][j] + match_func(m[i - 1], 0, 3, 1, h[j - 1]), q[i][j - 1] + match_func(m[i - 1], 0, 3, 1, h[j - 1]), q[i - 1][j - 1] + match_func(m[i - 1], 0, 3, 1, h[j - 1])) 
     #print(byPass(q[-1][-1], max_val(q), 800))
     print(q[-1][-1])
     return q[-1][-1]#byPass(q[-1][-1], max_val(q), 800)

def prep(x, y):
     score = [[0 for _ in range(len(y))] for __ in range(len(x))]
     for i in range(len(x)):
          for j in range(len(y)):
               score[i][j] = orig(x[i], y[j])
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