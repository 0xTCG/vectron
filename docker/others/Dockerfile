FROM vectron

# Create directories in the container
RUN mkdir -p /others/data

# Copy files from the host to the container
COPY seqx.fasta /others/data/
COPY seqy.fasta /others/data/
COPY /RV/* /RV/

WORKDIR /root

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

# Add the LLVM APT key and repository for Clang 16
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-16 main"

# Update the package list and install Clang 16
RUN apt-get update && apt-get install -y clang-16

# Install SeqAn [done] -> needs clang 17
RUN git clone https://github.com/seqan/seqan && \
    cd seqan && \
    CC=clang-17 CXX=clang++-17 cmake -S . -B build -G Ninja && \
    cmake --build build    

# Install Parasail
RUN git clone https://github.com/jeffdaily/parasail && \
    cd parasail && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make

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

COPY others_benchmarks.ipynb /root/

COPY libRV.so /root/

COPY smith_waterman_seqan.cpp /root/seqan/

RUN clang++-17 -O3 -msse4.2 -funroll-loops -mfpmath=sse -march=native -I/root/seqan/include /root/seqan/smith_waterman_seqan.cpp -o /root/seqan/smith_waterman_seqan

COPY seqx_adept.fasta /others/data/

COPY seqy_adept.fasta /others/data/

COPY seqx.txt /others/data/

COPY seqy.txt /others/data/
