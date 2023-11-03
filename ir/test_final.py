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
    q = listCR(len(h) + 1, len(m) + 1, 0, -4, -2, -4, -2, 0)
    s = listCR(len(h) + 1, len(m) + 1, -10000, -4, -2, -10000, 0, -10000) 
    l = listCR(len(h) + 1, len(m) + 1, -10000, -10000, 0, -4, -2, -10000)
    for i in range(1, len(m) + 1):
        for j in range(1, len(h) + 1):       
            if j - i <= -105 or j - i >= 105:
                    if j - i == -105 or j - i == 105:
                        q[i][j] = -10000
                        l[i][j] = -10000
                        s[i][j] = -10000
            else:
                    #q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 6, -2, -1, h[j - 1]), l[i - 1][j - 1] + match_func(m[i - 1], 5, -3, -2, h[j - 1]), s[i - 1][j - 1] + match_func(m[i - 1], 7, -1, -2, h[j - 1])) #000 
                    #q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 5, -4, -3, h[j - 1]), l[i - 1][j - 1] + match_func(m[i - 1], 6, -3, -3, h[j - 1]), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6)) #001   
                    #q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1]), max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), l[i - 1][j - 1] + match_func(m[i - 1], 7, -1, -2, h[j - 1]))   #010 
                    q[i][j] = max(q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1]), max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6))     #011 
                    #q[i][j] = max(max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1]), s[i - 1][j - 1] + match_func(m[i - 1], 7, -1, -2, h[j - 1])) #100                    
                    #q[i][j] = max(max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), q[i - 1][j - 1] + match_func(m[i - 1], 5, -4, -3, h[j - 1]), s[i - 1][j - 1] + match_func(m[i - 1], 7, -3, -4, h[j - 1])) #100 
                    #q[i][j] = max(max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1]), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6)) #101            
                    #q[i][j] = max(max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6), q[i - 1][j - 1] + match_func(m[i - 1], 2, -4, -3, h[j - 1])) #110 
                    #q[i][j] = max(max_store(s, i, j, s[i - 1][j] - 2, q[i - 1][j] - 6), max_store(l, i, j, l[i][j - 1] - 2, q[i][j - 1] - 6), max_store(s, i, j, l[i - 1][j - 1] - 2, q[i - 1][j - 1] - 6)) #111
    print(byPass(q[-1][-1], max_val(q), 800))
    #print(q[-1][-1])
    return byPass(q[-1][-1], max_val(q), 800)

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