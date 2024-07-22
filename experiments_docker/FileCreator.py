from random import randint

def get_new_seq(l):
    dna = ['A', 'C', 'G', 'T', 'N']
    s = ""
    for i in range(l):
        t = randint(0, 4)
        s += dna[t]
    return s

SEQ_NO_T = 512
SEQ_NO_Q = 512
length = [64, 128, 256, 512]
length_x = [128, 256, 512]
a = 2
b = 4
gapo = 4
gape = 2
f = open("seqx.txt", "w")
seqs_x = ["" for _ in range(SEQ_NO_T)]        
l = SEQ_NO_T
g = 0
count_tmp = 0
#while g < 3:
#    count = randint(0, l)
#    for j in range(count_tmp, count + count_tmp):
#        seqs_x[j] = get_new_seq(length_x[g])
#        f.write(seqs_x[j])
#        f.write("\n")
#    g += 1
#    l -= count
#    count_tmp += count
for j in range(SEQ_NO_T):
    seqs_x[j] = get_new_seq(512)
    f.write(seqs_x[j])
    f.write("\n")    
f.close()
f = open("seqy.txt", "w")
seqs_y = ["" for _ in range(SEQ_NO_Q)]        
l = SEQ_NO_Q
g = 0
count_tmp = 0
#while g < 3:
#    count = randint(0, l)
#    for j in range(count_tmp, count + count_tmp):
#        seqs_y[j] = get_new_seq(length_x[g])
#        f.write(seqs_y[j])
#        f.write("\n")
#    g += 1
#    l -= count
#    count_tmp += count
for j in range(SEQ_NO_Q):
    seqs_y[j] = get_new_seq(512)
    #print(seqs_y[j])
    f.write(seqs_y[j])
    f.write("\n")    
f.close()
f = open("seqx_seq.txt", "w")
f2 = open("seqy_seq.txt", "w")
for i in range(SEQ_NO_T):
    for j in range(SEQ_NO_Q):
        f.write(seqs_x[i])
        f.write("\n")
        f2.write(seqs_y[j])
        f2.write("\n")
f.close()
f2.close()