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
      -DLLVM_CUSTOM_PATH=<path to Codon LLVM>
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
from vectron.dispatch import *

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
        M[i-1][j-1] + (2 if q[j-1] == t[i-1] else -3)
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

targets = [...]  # list of strings
queries = [...]
invoke(targets, queries)
```

## Experiments
The [experiment directory](docker/experiments_docker) contains few popular DP implementations such as banded Smith-Waterman, Needleman-Wunsch, Hamming Distance, Levenshtein Distance, Manhattan Tourist, Minimum Cost Path, and Longest Common Subsequence (in both integer and floating point versions). Alignment scores are calculated using sample string files `seqx.txt` and `seqy.txt`, each containing 64 DNA sequences of length 512. All DP algorithms perform an all-to-all pairing and comparison between them.

## Limitations

The current version has some limitations that will be addressed soon.
Most importantly:

- The number of arguments in the min/max function is limited to three
- List comprehensions in the initialization cannot have newlines
- Vectron must be imported as `import vectron` (aliases won't work)

## License

[Apache 2.0](https://www.apache.org/licenses/LICENSE-2.0).

## Contact

[Sourena Moghaddesi](https://github.com/SourenM) ([souren at uvic dot ca](mailto:)) or [Ibrahim Numanagić](https://github.com/inumanag) ([inumanag at uvic dot ca](mailto:)).
