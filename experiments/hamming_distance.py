from experimental.simd import *
from bio import *
import time
import os
import gpu
import sys

var_type = "i16"

@__attribute__
def vectron_calc():
    pass
@__attribute__
def vectron_init():    
    pass
@__attribute__
def vectron_list():    
    pass
@__attribute__
def vectron_bypass():
    pass
@__attribute__
def vectron_max():
    pass
@__attribute__
def vectron_match():
    pass

@vectron_bypass
def by_pass_foo(a, b, c):
    return 0

@vectron_match
def match_func_foo(x, a, b, am, y):
    if str(x) == "N" or str(y) == "N":
        return am
    elif x == y:
        return a
    else:
        return b

@vectron_max
def max_store_foo(lst, ind_row, ind_col, val_1, val_2):
    lst[ind_row][ind_col] = max(val_1, val_2)
    return lst[ind_row][ind_col]

def max_val(a):
    mx = 0
    for i in range(len(a)):
        for j in range(len(a[i])):
            if mx < a[i][j]:
                    mx = a[i][j]
    return mx                

@vectron_calc
def orig_foo(h, m):
    q = [[0 for __ in range(len(m) + 1)] for _ in range(len(h) + 1)]
    for i in range(1, len(m) + 1):
        for j in range(1, len(h) + 1):       
            if j - i <= -1 or j - i >= 1:
                    if j - i == -1 or j - i == 1:
                        q[i][j] = -10000
            else:
                q[i][j] = max(q[i - 1][j - 1] + match_func_foo(m[i - 1], 0, 1, 1, h[j - 1]), 0, 0)
    print(q[-1][-1])
    return q[-1][-1]

@vectron_init
def prep_foo(x, y):
    score = [[0 for _ in range(len(y))] for __ in range(len(x))]
    for i in range(len(x)):
        for j in range(len(y)):
            score[i][j] = orig_foo(x[i], y[j])
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
print("RHI")
with time.timing("Total: "):
    d = prep_foo(seqs_x, seqs_y)
print("DONE")