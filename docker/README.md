## Docker Environment

### Abstract

The Vectron artifact and the exact steps for reproducing the above-mentioned experiments can be found in this folder.

### Artifact check-list (meta-information)

- **Algorithm:** Auto-vectorization of many-to-many dynamic programming (DP) algorithms.
- **Program:** The exact steps to run the experiments are outlined in the accompanying Docker build files and Jupyter notebooks. Five (5) datasets of DNA sequence pairs are provided in the repository.
- **Compilation:** Docker or Docker-like environment (e.g., podman) is needed to run the experiments. All Docker builds are based on CUDA-enabled Ubuntu 22.04 (x86-64) and include all necessary dependencies (LLVM 17 with NVPTX support, Codon compiler, Clang suite of compilers). An additional Docker environment is also included for tools that need Intel's Classic Compiler Suite (icc) and CUDA 11.
- **Binary:** All required binaries will be built during Dockerization. For convenience, we have a pre-built provided `libRV.so` in the repository for the RV vectorization framework.
- **Data set:** All datasets are located within the `docker/experiments_docker/data` directory in the Vectron repository.
- **Run-time environment:** Any Linux distribution (x86-64) with Docker (or equivalent) installed.
- **Hardware:** Our setup included an Intel Xeon-powered hardware with SSE 4.1, SSE 4.2, AVX, AVX2 and AVX-512 support, and a NVIDIA-based GPU capable of running CUDA 11 and 12. Non-GPU experiments do not need NVIDIA GPU.
- **Execution:** First build the Docker images via `docker build` (or equivalent). There are three images to build: the main image (`docker/vectron`; contains Vectron, Codon, C++ and CUDA experiments), the external tools image (`docker/others`; contains comparisons with the other state-of-the-art tools), and lastly the CUDA 11 image (`docker/cuda11`; contains ADEPT and SW# tools). Upon building, the images can be run as `docker run --name=<image name>`.
- **Metrics:** Execution time (might vary depending on the underlying CPU speeds) and speed-up ratios (should stay fairly consistent).
- **Output:** The output of all experiments is the DP score between all the `Query` and `Target` sequence pairs, and the execution time of each experiment.
- **Experiments:** The main and external tools images contain the Jupyter notebooks with the annotated commands and timing scripts. The CUDA 11 image only contains a bash environment and a README that shows how to run the remaining experiments.
- **How much disk space required (approximately)?** Around 15 to 20 GB for Docker build.
- **How much time is needed to prepare workflow (approximately)?** The Docker images will be built in less than 90 minutes on a decent system.
- **How much time is needed to complete experiments (approximately)?** The experiments will take around 150 minutes for everything except `Codon` and `C++` experiments on the large dataset. The large experiments take between 12 hours (C++) and 30 hours (Codon).
- **Publicly available?** Yes.
- **Code licenses (if publicly available)?** Apache 2.0 License.

### Description

- **How delivered?** The artifact evaluation package can be obtained from GitHub ([https://github.com/0xTCG/vectron](https://github.com/0xTCG/vectron)).

#### Hardware dependencies

NVIDIA GPU (with CUDA version 11.0 and above) for GPU experiments, and SSE 4.2 or AVX-enabled Intel CPU (64-bit) for CPU experiments.

#### Software dependencies

A x86-64 Linux distribution or any system that can run Linux-based Docker images.

#### Data sets

The datasets are all available and are non-private. They are either provided in the repository or are created (via pre-processing) during the image-building process.

#### Installation

After cloning the repository follow these steps:

- Build the main Docker container using a sample command: `cd docker/vectron && docker build --tag vectron .`  
  Podman users might need to add `--security-opt label=disable --security-opt seccomp=unconfined` for both the build and run steps.

- The main container can now be run using a run command with `docker run -it -p 8888:8888 vectron`. You might need to add `--device nvidia.com/gpu=all` or a similar flag to enable GPU passthrough. Then you can open the experimental notebook by going to [http://127.0.0.1:8888](http://127.0.0.1:8888) or a link shown upon completing the `docker run` command.

- To build the experiments for other state-of-the-art implementations, repeat the same steps as above (`cd docker/others && docker build --tag vectron-sota . && docker run -it -p 8888:8888 vectron-sota`).

- To build the Aalign and SW# experiments on the host system, build the CUDA 11 image: `cd docker/cuda11 && docker build --tag vectron-cuda11 .`. Run the experiments by entering the image shell (`docker run -it vectron-cuda11 /bin/bash`) and then executing `time /swsharp/bin/swsharpdb -i /others/data/seqx.fasta -j /others/data/seqy.fasta` and `time /aalign/ModularDesign/SmithWatermanAffineGap Mod.out -q /others/data/seqx.fasta -d /others/data/seqy.fasta`.

### Experiment workflow

Jupyter notebooks contain the full descriptions of the experiment workflow for Vectron, Codon, C++, and other state-of-the-art experiments.

### Evaluation and expected result

The expected results (from our runs) are shown in the Jupyter notebooks in Vectron's repository. All experiments can be evaluated by running the cells inside the notebooks again (except for Aalign and SW#).

### Experiment customization

All the Vectron (Codon), C++, and CUDA scripts are available in the `docker/experiments_docker/source` directory. They can be modified to run customized experiments.
