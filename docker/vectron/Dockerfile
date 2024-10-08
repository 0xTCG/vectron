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
    python3-pip \
    ninja-build

RUN apt-get install -y cuda-libraries-12-4

# Clean up
RUN rm -rf /var/lib/apt/lists/*

# Install Python packages
RUN pip3 install jupyter jupyterlab

# Build LLVM
RUN git clone --depth 1 -b codon https://github.com/exaloop/llvm-project && \
    mkdir -p llvm-project/llvm/build && \
    cd llvm-project/llvm/build && \
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release \
             -DLLVM_INCLUDE_TESTS=OFF \
             -DLLVM_ENABLE_RTTI=ON \
             -DLLVM_ENABLE_ZLIB=OFF \
             -DLLVM_ENABLE_TERMINFO=OFF \
             -DLLVM_TARGETS_TO_BUILD="host;NVPTX" && \
    cmake --build . && cmake --install .

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

# Build Codon
# Remove the -DCODON_GPU=ON flag if the device the container is being built on does not have an NVIDIA GPU and/or
# does not support cuda. Also please note that if there are no NVIDIA GPUs present on the system, Vectron's GPU
# benchmarks as well as other GPU implementations (like ADEPT) will not be able to run on the resulting container.

RUN git clone https://github.com/exaloop/codon.git ${VECTRON_CODON_PATH} && \
    cd $VECTRON_CODON_PATH && \
    cmake -S . -B build_release -G Ninja \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_C_COMPILER=clang \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DLLVM_DIR=/llvm-project/llvm/build/lib/cmake/llvm \
         -DCMAKE_INSTALL_PREFIX=${VECTRON_CODON_PATH}/install_release \
         -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON && \
    cmake --build build_release && \
    cmake --install build_release

RUN cd $VECTRON_CODON_PATH && \
    cmake -S . -B build_release_gpu -G Ninja \
         -DCMAKE_BUILD_TYPE=Release \
         -DCODON_GPU=ON \
         -DCMAKE_C_COMPILER=clang \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DLLVM_DIR=/llvm-project/llvm/build/lib/cmake/llvm \
         -DCMAKE_INSTALL_PREFIX=${VECTRON_CODON_PATH}/install_release_gpu \
         -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON && \
    LD_LIBRARY_PATH=/usr/local/cuda-12.4/compat:${LD_LIBRARY_PATH} cmake --build build_release_gpu && \
    cmake --install build_release_gpu

# Update PATH environment variable
ENV PATH=/codon/install_release/bin/:$PATH

RUN mkdir -p /codon/install_release/lib/codon/plugins/seq

# Build Seq
RUN git clone --depth 1 https://github.com/exaloop/seq ${VECTRON_SEQ_PATH} && \
    cd $VECTRON_SEQ_PATH && \
    cmake -S . -B build -G Ninja  \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_C_COMPILER=clang \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DLLVM_DIR=/llvm-project/llvm/build/lib/cmake/llvm \
         -DCODON_PATH=/codon/install_release \
         -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
         -DCMAKE_INSTALL_PREFIX=/codon/install_release/lib/codon/plugins/seq && \
    cmake --build build && \
    cmake --install build

RUN cd $VECTRON_SEQ_PATH && \
    cmake -S . -B build_gpu -G Ninja  \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_C_COMPILER=clang \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DLLVM_DIR=/llvm-project/llvm/build/lib/cmake/llvm \
         -DCODON_PATH=/codon/install_release_gpu \
         -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
         -DCMAKE_INSTALL_PREFIX=/codon/install_release/lib/codon/plugins/seq && \
    LD_LIBRARY_PATH=/usr/local/cuda-12.4/compat:${LD_LIBRARY_PATH} cmake --build build_gpu && \
    cmake --install build_gpu

# Build Vectron
RUN git clone https://github.com/0xTCG/vectron.git ${VECTRON_PATH} && \
    cd $VECTRON_PATH && \
    cmake -S . -B build -G Ninja \
         -DCODON_PATH=/codon/install_release \
         -DLLVM_PATH=/llvm-project/llvm/build \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DCMAKE_C_COMPILER=clang && \
    cmake --build build && \
    cmake --install build
RUN cd $VECTRON_PATH && \
    cmake -S . -B build_gpu -G Ninja \
        -DCODON_PATH=/codon/install_release_gpu \
        -DLLVM_PATH=/llvm-project/llvm/build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_C_COMPILER=clang && \
    cmake --build build_gpu && \
    cmake --install build_gpu

RUN mkdir /vectron/docker/experiments_docker/data/cpp_medium && \
    mkdir /vectron/docker/experiments_docker/data/cpp_large && \
    mkdir /vectron/docker/experiments_docker/data/cuda_small && \
    mkdir /vectron/docker/experiments_docker/data/cuda_medium

# Run experiments
RUN cd ${VECTRON_PATH}/docker/experiments_docker && \
    python3 seq_modifier.py 16 && \
    python3 seq_modifier.py 32 && \
    python3 seq_modifier.py 64 && \
    python3 seq_modifier.py 512 && \
    python3 seq_modifier.py 2048

# Expose the port for Jupyter
EXPOSE 8888

#Install tabulate
RUN pip3 install tabulate

COPY simd.codon /codon/install_release/lib/codon/stdlib/experimental/

# Copy the local Jupyter notebook file into the container
COPY benchmarks.ipynb /root/

COPY smith_waterman_inverted.codon /root/

COPY smith_waterman_inverted.cpp /root/

COPY utils.h /root/

# Default command to run JupyterLab and generate a token
CMD ["bash", "-c", "cd /root && jupyter lab --ip=0.0.0.0 --no-browser --allow-root"]
