var = 16

# Read sequences from seqx.txt and seqy.txt
with open("/vectron/experiments_docker/data/FLOAT_small/seqx.txt", "r") as file:
    seqx_data = file.readlines()

with open("/vectron/experiments_docker/data/FLOAT_small/seqy.txt", "r") as file:
    seqy_data = file.readlines()

with open("/vectron/experiments_docker/data/CUDA_small/seqx_256.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("/vectron/experiments_docker/data/CUDA_small/seqy_256.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)

print("Done")
