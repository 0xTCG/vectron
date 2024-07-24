var = 64

# Read sequences from seqx.txt and seqy.txt
with open("/vectron/experiments_docker/data/INT_small/seqx.txt", "r") as file:
    seqx_data = file.readlines()

with open("/vectron/experiments_docker/data/INT_small/seqy.txt", "r") as file:
    seqy_data = file.readlines()

with open("/vectron/experiments_docker/data/CPP_small/seqx_4096.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("/vectron/experiments_docker/data/CPP_small/seqy_4096.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)


with open("/vectron/experiments_docker/data/CUDA_large/seqx_4096.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("/vectron/experiments_docker/data/CUDA_large/seqy_4096.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)

print("Done")
