import sys

var = int(sys.argv[-1])

if var == 16:
    # Read sequences from seqx.txt and seqy.txt
    with open("/vectron/experiments_docker/data/float_small/seqx.txt", "r") as file:
        seqx_data = file.readlines()

    with open("/vectron/experiments_docker/data/float_small/seqy.txt", "r") as file:
        seqy_data = file.readlines()

    with open("/vectron/experiments_docker/data/cuda_small/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cuda_small/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)

    print("Done")
elif var == 32:
    # Read sequences from seqx.txt and seqy.txt
    with open("/vectron/experiments_docker/data/float_medium/seqx.txt", "r") as file:
        seqx_data = file.readlines()

    with open("/vectron/experiments_docker/data/float_medium/seqy.txt", "r") as file:
        seqy_data = file.readlines()

    with open("/vectron/experiments_docker/data/cuda_medium/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cuda_medium/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)

    print("Done")
elif var == 64:
    # Read sequences from seqx.txt and seqy.txt
    with open("/vectron/experiments_docker/data/int_small/seqx.txt", "r") as file:
        seqx_data = file.readlines()

    with open("/vectron/experiments_docker/data/int_small/seqy.txt", "r") as file:
        seqy_data = file.readlines()

    with open("/vectron/experiments_docker/data/cpp_small/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cpp_small/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)


    with open("/vectron/experiments_docker/data/cuda_large/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cuda_large/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)

    print("Done")
elif var == 512:
    # Read sequences from seqx.txt and seqy.txt
    with open("/vectron/experiments_docker/data/int_medium/seqx.txt", "r") as file:
        seqx_data = file.readlines()

    with open("/vectron/experiments_docker/data/int_medium/seqy.txt", "r") as file:
        seqy_data = file.readlines()

    with open("/vectron/experiments_docker/data/cpp_medium/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cpp_medium/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)

    print("Done")
elif var == 2048:
    # Read sequences from seqx.txt and seqy.txt
    with open("/vectron/experiments_docker/data/int_large/seqx.txt", "r") as file:
        seqx_data = file.readlines()

    with open("/vectron/experiments_docker/data/int_large/seqy.txt", "r") as file:
        seqy_data = file.readlines()

    with open("/vectron/experiments_docker/data/cpp_large/seqx.txt", "w") as file:
        for j in range(len(seqx_data)):
            for i in range(var):
                file.writelines(seqx_data[j])

    with open("/vectron/experiments_docker/data/cpp_large/seqy.txt", "w") as file:
        for i in range(var):
            file.writelines(seqy_data)

    print("Done")        
