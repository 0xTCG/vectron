# 💠 Vectron

## Introduction
Vectron is a compiler pass that automatically analyzes and vectorizes array or matrix-based dynamic programming (DP) kernels to maximize throughput over a large set of instances.
Based on our experiments, it can improve the performance of DP algorithms up to 18 times.

Vectron is based on [Codon](https://github.com/exaloop/codon), an ahead-of-time Pythonic compilation framework.

## Installation

Before building and using Vectron, please ensure that you have the following prerequisites:

- Codon and its LLVM distribution (ideally built from source; see [here](https://docs.exaloop.io/codon/advanced/build) for details)
     - Codon GPU support (with LLVM NVPTX) is required for GPU support
- Clang++
- (optional) [Seq library](https://github.com/exaloop/seq) for experimental section

Then, build vectron as follows:
```cmake
cmake -S vectron -B vectron/build -G Ninja \
      -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang \
      -DCODON_PATH=<path to Codon>
      -DLLVM_PATH=<path to Codon LLVM>
cmake --build vectron/build
cmake --install vectron/build
```

If stuck, please see our [Dockerfile](docker/vectron/Dockerfile) for the exact build script.

## Running

After building Vectron, use Vectron as follows:
```
codon build -plugin vectron -release example.codon
```

Here is a typical use case:

```python
import time
import sys
from vectron.dispatcher import *

var_type = "i16"

# Use vectron to annotate DP kernel
@vectron_kernel
def levenshtein(Q, T):
  M = [[0] * (len(Q) + 1) for _ in range(len(T) + 1)]
  for i in range(len(T) + 1):
    M[i][0] = -2 + i * -4
  for j in range(len(Q) + 1):
    M[0][j] = -2 + j * -4

  # Kernel
  for i in range(1, len(T)+1):
    for j in range(1, len(Q)+1):
      M[i][j] = max(
        M[i - 1][j] - 2,
        M[i][j - 1] - 2,
        M[i-1][j-1] + (2 if Q[j-1] == T[i-1] else -3)
      )

  # Aggregation
  return M[-1][-1]

# Use vectron_scheduler to annotate function that invokes the kernel on list of pairs
@vectron_scheduler
def invoke(x, y):
    score = [[0 for i in range(len(y))] for j in range(len(x))]
    for i in range(len(x)):
        for j in range(len(y)):
            score[i][j] = levenshtein(x[i], y[j])
    return score
with open(sys.argv[-1], 'r') as file:
    seqs_x = [line.strip() for line in file]

with open(sys.argv[-2], 'r') as file:
    seqs_y = [line.strip() for line in file]

SEQ_NO_T = len(seqs_x)
SEQ_NO_Q = len(seqs_y)

with time.timing("Total: "):
    d = invoke(seqs_x, seqs_y)
```
- ```var_type```: determines the type of auto-vectorization, where i16 will invoke the CPU and f32 will invoke the GPU mode.
- ```@vectron_kernel```: is the decorator for the main kernel of vectron. It has 3 parts:
     - Initialization: Where you can initialize 1 to 3 matrices for the desired DP using for loops or list comprehensions
     - Kernel: Where the main DP operation is done. The main available operations are a min or a max, with 2 or 3 arguments. Each argument can be a matrix element (+ or - a constant), a ternary instruction (a sample of which is provided above), or a call to a function decorated with ```@vectron_max```, which compares two input elements, returns their maximum and at the same time stores this maximum at a given destination.
     - Aggregation: Where the final results are returned. This part can return a matrix element (or a math operation on a matrix element), or a call to a function decorated with ```@vectron_bypass``` which compares the matrix element to a fixed threshold and returns a default number (usually a large negative number) if the result is smaller than the threshold. (```@vectron_bypass``` is mostly used in genomic applications and is called ```zdrop```)
- ```@vectron_scheduler``` which is used to handle and modify how the input sequences are paired and sent to the main kernel. In this function -- which basically loops over the target and then the query sequences -- the user can determine which target sequences gets paired up with which query sequences by modifying the nested loops' start, step and stop values.

After using the above build command for the sample script, one can run the built script by passing the target and query sequences to it as ```sys.arg``` values. Here's a sample command:

```
./example ../data/seqx.txt ../data/seqy.txt
```

## Experiments
The [experiment directory](docker/experiments_docker) contains few popular DP implementations such as banded Smith-Waterman, Needleman-Wunsch, Hamming Distance, Levenshtein Distance, Manhattan Tourist, Minimum Cost Path, and Longest Common Subsequence (in both integer and floating point versions). Alignment scores are calculated using sample string files `seqx.txt` and `seqy.txt`, each containing 64 DNA sequences of length 512. All DP algorithms perform an all-to-all pairing and comparison between them.

## Limitations

The current version has some limitations that will be addressed soon.
Most importantly:

- The number of arguments in the min/max function is limited to three
- List comprehensions in the initialization cannot have newlines
- Vectron must be imported as `from vectron.dispatcher import *` (aliases won't work)

## License

[Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Contact

[Sourena Moghaddesi](https://github.com/SourenM) ([souren at uvic dot ca](mailto:)) or [Ibrahim Numanagić](https://github.com/inumanag) ([inumanag at uvic dot ca](mailto:)).
