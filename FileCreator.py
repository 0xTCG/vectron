from random import randint

def get_new_seq(l):
    dna = ['A', 'C', 'G', 'T', 'N']
    s = ""
    for i in range(l):
        t = randint(0, 4)
        s += dna[t]
    return s

SEQ_NO_T = 64
SEQ_NO_Q = 64
length = [64, 128, 256, 512]
length_x = [128, 256, 512]

f = open("seqx.txt", "w")
seqs_x = ["" for _ in range(SEQ_NO_T)]        
l = SEQ_NO_T
for j in range(SEQ_NO_T):
    seqs_x[j] = get_new_seq(512)
    f.write(seqs_x[j])
    f.write("\n")    
f.close()
f = open("seqy.txt", "w")
seqs_y = ["" for _ in range(SEQ_NO_Q)]        
l = SEQ_NO_Q
for j in range(SEQ_NO_Q):
    seqs_y[j] = get_new_seq(512)
    f.write(seqs_y[j])
    f.write("\n")    
f.close()
