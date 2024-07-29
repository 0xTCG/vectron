FROM nvidia/cuda:12.4.1-devel-ubuntu22.04

ENV DEBIAN_FRONTEND=noninteractive

# Update package lists
RUN apt-get update

# Install essential packages and dependencies for CMake
RUN apt-get install -y \
    cmake \
    protobuf-compiler \
    git \
    clang \
    ninja-build \
    openssl \
    vim \
    time \
    libbz2-dev \
    patch \
    python3 \
    python3-pip

RUN apt-get install -y cuda-libraries-12-4

# Clean up
RUN rm -rf /var/lib/apt/lists/*

# Install Python packages
RUN pip3 install jupyter jupyterlab

# Build LLVM
RUN git clone --depth 1 -b codon https://github.com/exaloop/llvm-project && \
    mkdir -p llvm-project/llvm/build && \
    cd llvm-project/llvm/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DLLVM_INCLUDE_TESTS=OFF \
             -DLLVM_ENABLE_RTTI=ON \
             -DLLVM_ENABLE_ZLIB=OFF \
             -DLLVM_ENABLE_TERMINFO=OFF \
             -DLLVM_TARGETS_TO_BUILD=host && \
    make && \
    make install

# Set environment variables
ENV CODON_PYTHON=/usr/lib64/libpython3.so
ENV VECTRON_LLVM_PATH=/llvm-project/llvm
ENV VECTRON_CODON_PATH=/codon
ENV VECTRON_SEQ_PATH=/codon-seq
ENV VECTRON_NUMPY_PATH=/codon-numpy
ENV VECTRON_PATH=/vectron
ENV CC=clang
ENV CXX=clang++

# Debug prints for CUDA
RUN echo "CUDA Version: $(nvcc --version)" && \
    echo "CUDA Library Path: $(find /usr/local/cuda/lib64 -name 'libcuda.so*')" && \
    echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"