FROM vectron

# Create directories in the container
RUN mkdir -p /others/data

RUN mkdir -p RV

# Copy files from the host to the container
COPY seqx.fasta /others/data/
COPY seqy.fasta /others/data/
COPY /RV/* /RV/

WORKDIR /root

# Install RV
#RUN mkdir rv-llvm-project && \
#    git clone https://github.com/llvm/llvm-project rv-llvm-project && \
#    cd rv-llvm-project && \
#    git clone https://github.com/cdl-saarland/rv && \
#    cd rv && \
#    git submodule update --init && \
#    cd .. && \
#    git checkout llvmorg-16.0.6 && \
#    cmake  -G Ninja -S llvm -B build -DCMAKE_BUILD_TYPE=Release -DLLVM_INCLUDE_TESTS=OFF  -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_EXTERNAL_PROJECTS="rv" -DLLVM_EXTERNAL_RV_SOURCE_DIR=rv -DCMAKE_INSTALL_PREFIX=/llvm-rv && \
#    cmake --build build && \
#    cmake --install build && \
#    gcc -shared -o /llvm-rv/lib/libRV.so -Wl,--whole-archive /llvm-rv/lib/libRV.a -Wl,--no-whole-archive

# Install ADEPT
RUN git clone https://github.com/mgawan/ADEPT/ && \
    cd ADEPT && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make

# Install SSW
RUN mkdir ssw && \
    git clone https://github.com/mengyao/Complete-Striped-Smith-Waterman-Library /ssw && \
    cd /ssw/src && \
    make

# Install Parasail
RUN git clone https://github.com/jeffdaily/parasail && \
    cd parasail && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make


# Install clang 17
RUN apt-get update && apt-get install -y \
    wget \
    software-properties-common \
    gnupg2 \
    && rm -rf /var/lib/apt/lists/*

# Add the LLVM repository key and the repository
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-17 main"

# Update the package list and install Clang 17
RUN apt-get update && apt-get install -y clang-17

# Install SeqAn [done] -> needs clang 17
RUN git clone https://github.com/seqan/seqan && \
    cd seqan && \
    CC=clang-17 CXX=clang++-17 cmake -S . -B build -G Ninja && \
    cmake --build build    

# copy SEQ INTERALIGN
COPY seq_interalign.codon /root/

# Install MPI dependencies
RUN apt-get update && apt-get install -y \
    openmpi-bin \
    libopenmpi-dev \
    && rm -rf /var/lib/apt/lists/*

# Install SWIPE
RUN git clone https://github.com/torognes/swipe/ && \
    cd swipe && \
    mkdir swipe_data && \
    make

COPY /swipe_data/* /swipe/swipe_data/
    
COPY others_benchmark.ipynb /root/