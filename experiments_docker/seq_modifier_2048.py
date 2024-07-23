var = 2048

# Read sequences from seqx.txt and seqy.txt
with open("./data/INT_large/seqx.txt", "r") as file:
    seqx_data = file.readlines()

with open("./data/INT_large/seqy.txt", "r") as file:
    seqy_data = file.readlines()

with open("./data/CPP_large/seqx_4194304.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("./data/CPP_large/seqy_4194304.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)

print("Done")
