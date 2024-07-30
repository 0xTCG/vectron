FROM llvm

# Build Codon
RUN git clone https://github.com/exaloop/codon.git ${VECTRON_CODON_PATH} && \
    cd $VECTRON_CODON_PATH && \
    cmake -S . -B build_release -G Ninja \
         -DCMAKE_BUILD_TYPE=Release \
         -DCODON_GPU=ON \
         -DCMAKE_C_COMPILER=clang \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DLLVM_DIR=/llvm-project/llvm/build/lib/cmake/llvm \
         -DCMAKE_INSTALL_PREFIX=${VECTRON_CODON_PATH}/install_release \
         -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON && \
    LD_LIBRARY_PATH=/usr/local/cuda-12.4/compat:${LD_LIBRARY_PATH} cmake --build build_release && \
    cmake --install build_release

# Update PATH environment variable
ENV PATH=/codon/install_release/bin/:$PATH

ENV CPY_NMR=3

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
    LD_LIBRARY_PATH=/usr/local/cuda-12.4/compat:${LD_LIBRARY_PATH} cmake --build build && \
    cmake --install build

ENV VEC_NMR=1

# Build Vectron
RUN git clone https://Sourenm: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@github.com/0xTCG/vectron.git ${VECTRON_PATH} && \
    cd $VECTRON_PATH && \
    cmake -S . -B build -G Ninja \
         -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_CXX_COMPILER=clang++ \
         -DCMAKE_C_COMPILER=clang && \
    cmake --build build

# Move necessary files
RUN mv ${VECTRON_PATH}/stdlib/experimental/simd.codon /codon/install_release/lib/codon/stdlib/experimental && \
    mv ${VECTRON_PATH}/stdlib/vectron /codon/install_release/lib/codon/stdlib/

RUN mkdir /vectron/experiments_docker/data/cpp_medium && \
    mkdir /vectron/experiments_docker/data/cpp_large && \
    mkdir /vectron/experiments_docker/data/cuda_small && \
    mkdir /vectron/experiments_docker/data/cuda_medium

# Run experiments
RUN cd ${VECTRON_PATH}/experiments_docker && \
    python3 seq_modifier.py 16 && \
    python3 seq_modifier.py 32 && \
    python3 seq_modifier.py 64 && \
    python3 seq_modifier.py 512 && \
    python3 seq_modifier.py 2048

# Expose the port for Jupyter
EXPOSE 8888

#Install tabulate
RUN pip3 install tabulate

# Copy the local Jupyter notebook file into the container
COPY benchmarks.ipynb /root/

# Default command to run JupyterLab and generate a token
CMD ["bash", "-c", "cd /root && jupyter lab --ip=0.0.0.0 --no-browser --allow-root"]
