var = 32

# Read sequences from seqx.txt and seqy.txt
with open("/vectron/experiments_docker/data/FLOAT_medium/seqx.txt", "r") as file:
    seqx_data = file.readlines()

with open("/vectron/experiments_docker/data/FLOAT_medium/seqy.txt", "r") as file:
    seqy_data = file.readlines()

with open("/vectron/experiments_docker/data/CUDA_medium/seqx_1024.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("/vectron/experiments_docker/data/CUDA_medium/seqy_1024.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)

print("Done")
