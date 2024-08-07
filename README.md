# Vectron

## Introduction
Vectron is a compiler pass that automatically analyzes and vectorizes any array or matrix-based dynamic programming scheme to maximize throughput over a large set of instances.

Vectron is based on Codon, an ahead-of-time Pythonic compilation framework, that benefits from the simplicity of Python's syntax. We compared Vectron against state-of-the-art implementations of various dynamic programming schemes---both optimized for single problem instances and multiple instances---and show that it delivers significant speedups compared to them, achieving an average speedup of up to 17.73x.

## Prerequisites
Before building and using Vectron, ensure you have the following prerequisites:
- clang++ version 17
- Codon's LLVM, which can be built using:
```
git clone --depth 1 -b codon https://github.com/exaloop/llvm-project codon-llvm && \
     cmake -S codon-llvm/llvm -G Ninja \
           -B codon-llvm/build \
           -DCMAKE_BUILD_TYPE=Release \
           -DLLVM_INCLUDE_TESTS=OFF \
           -DLLVM_ENABLE_RTTI=ON \
           -DLLVM_ENABLE_ZLIB=OFF \
           -DLLVM_ENABLE_TERMINFO=OFF \
           -DLLVM_TARGETS_TO_BUILD="host;NVPTX" \
           -DLLVM_BUILD_TOOLS=OFF \
           -DCMAKE_INSTALL_PREFIX=$(pwd)/codon-llvm/install && \
    cmake --build codon-llvm/build && \
    cmake --install codon-llvm/build
```
- SEQ which can be built using:
```
/bin/bash -c "$(curl -fsSL https://seq-lang.org/install.sh)"
```
- Codon itself which can be built using:
```
git clone https://github.com/exaloop/codon && \
    cd codon && \
    cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release -DCODON_GPU=ON -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLLVM_DIR=../codon-llvm/install/lib/cmake/llvm -G Ninja -DCMAKE_INSTALL_PREFIX=$(pwd)/install_release -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON && cmake --build build_release && cmake --install build_release
```

## Copying the Libraries
After Codon is built move the contents of the ```stdlib``` (```stdlib/experimental``` and ```stdlib/vectron```) folder to ```{Codon_INSTALL_PATH}/codon/install_release/lib/codon/stdlib/```

## Building Vectron
Follow these steps to clone and build Vectron:
```
git clone https://github.com/0xTCG/vectron && \
cd vectron
```
Edit `CMakeLists.txt` and change the following lines to point to the correct paths:
```cmake
set(CODON_PATH "<codon_folder_path>/install_release")
set(LLVM_CUSTOM_PATH "<codon_llvm_folder_path>/install")
```
Then build Vectron:
```
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang && \
cmake --build build
```

## Running Tests
After building Vectron, build the `vectron.py`:
```
codon build vectron.codon -release
```
Then run the Vectron providing the required paths:
```
./vectron <seq_folder_path> <vectron_folder_path> <file_path>
```
Afterwards you can run the built file using:
```
./<file_path> <target_seq_path> <query_seq_path>
```

## Experiments
The experiments folder is populated with some of the most famous DP algorithms like banded Smith-Waterman, Needleman-Wunsch, Hamming Distance, Levenshtein Distance, Manhattan Tourist, Minimum Cost Path, and Longest Common Subsequence (in both integer and floating point versions). Alignment scores are calculated using sample string files `seqx.txt` and `seqy.txt`, each containing 64 DNA sequences of length 512. All DP algorithms perform an all-to-all pairing and comparison between them. 
