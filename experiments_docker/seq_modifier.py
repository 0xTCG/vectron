var = 512

# Read sequences from seqx.txt and seqy.txt
with open("seqx.txt", "r") as file:
    seqx_data = file.readlines()

with open("seqy.txt", "r") as file:
    seqy_data = file.readlines()

with open("seqx_262144.txt", "w") as file:
    for j in range(len(seqx_data)):
        for i in range(var):
            file.writelines(seqx_data[j])

with open("seqy_262144.txt", "w") as file:
    for i in range(var):
        file.writelines(seqy_data)

print("Done")
