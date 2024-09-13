# 💠 Vectron

## Introduction
Vectron is a compiler pass that automatically analyzes and vectorizes array or matrix-based dynamic programming (DP) kernels to maximize throughput over a large set of instances.
Based on our experiments, it can improve the performance of DP algorithms up to 18 times.

Vectron is based on [Codon](https://github.com/exaloop/codon), an ahead-of-time Pythonic compilation framework.

## Beneath the Hood

Vectron starts by analyzing all functions with decorators starting with the `@vectron` decorator that contain a single instance DP kernel (this kernel function is decorated with `@vectron_kernel`). For each such kernel, Vectron generates a new procedure that takes a list of instances as input and performs a set of specific operations in parallel on these instances.

Kernel analysis begins by dividing the kernel function into four major blocks.
The first block — **initialization block** — deals with the creation and initialization of a DP matrix. Vectron considers anything before the main set of `for` loops as a part of the initialization block.
The second block — **loop block** — is the core part of a DP method responsible for populating the DP matrix. It consists of a series of nested `for` loops and a DP recurrence. DP recurrence is considered as a **recurrence block**. Vectron assumes that everything within the innermost `for` loop is a recurrence block.
Finally, the **aggregation block** aggregates and returns the final result from the calculated matrix. Vectron treats anything after the iteration block as the aggregation block.

### Recurrence analysis

The main Vectron analysis pass consists of identifying and analyzing the iteration and recurrence blocks within a DP kernel. 
The pass begins by detecting a block of the nested for loops within the function. Once found, Vectron identifies the iteration behavior by extracting `start`, `step`, and `stop` bounds of each loop. 
Typically, `stop` is a constant or the length of an instance (e.g., string) plus or minus a constant, while the `start` and `step` values are constants.
However, Vectron supports more complex custom ranges within the innermost loop. One example of this is the banded Smith-Waterman algorithm that only populates `M` within a diagonal band of a certain, user-defined length. 

Once the loop range values have been identified, the pass moves on to the recurrence within the innermost `for` loop. There, it analyzes the recurrence expression and replaces it with the vectorized equivalent; in the case of SIMD, Vectron uses the `Vec` type and the associated SIMD intrinsics from Codon's SIMD library. 

A recurrence expression is any expression that minimizes or maximizes something via `min` or `max` operator. 
This expression is itself composed of various subexpressions that are typically arguments to the `min`/`max` operators.
Vectron vectorizes these subexpressions that are either (1) constants, (2) arithmetic operations on top of a previously calculated DP matrix value, (3) `max` operators + ‘store’ (assignment) operator (by calling a function decorated with `@vectron_max`), (4) comparator function (which can be a ternary operator or a call to a comparator function decorated with `@vectron_cmp`), or (5) an arithmetic combination of previous expressions. 

Upon analyzing the loop block, Vectron instantiates a separate loop block that iterates over a set of instances and auto-vectorizes the recurrence to perform each step in a data-parallel manner through either SIMD or SIMT (GPU) strategy. 


### Initialization and aggregation

Vectron treats any code before the main loops as the initialization block where the DP matrices are built. The user can build one to three different matrices in this block through conventional methods. Vectron will attempt to vectorize the initialization list comprehensions if possible; if not, it will build matrices as-is. Following the initialization, Vectron will identify the DP matrices and translate them to a semi-3D cache-friendly vectorized structure that will be used by the vectorized kernel. 

The aggregation block typically contains a return statement that can be easily vectorized. In some instances, this block can contain a more complex set of statements or call a special `aggregation` function (decorated with `@vectron_bypass`, which performs additional operations on top of the final matrix score (e.g., `z-drop` score processing implemented by many Smith-Waterman methods). Aggregation functions are also vectorized whenever possible.

### Scheduling

Finally, Vectron integrates previous steps into a scheduler function that processes sequence pairs and dispatches them to the vectorized kernel (decorated with `@vectron_scheduler`). Currently, Vectron's scheduler groups together pairs into blocks that are all to be executed in a data-parallel fashion. Typically, the scheduler groups `n` pairs into `n/v` blocks, where `v` is the maximum parallel throughput (e.g., `v` is 16 for 256-bit AVX2 vectors that contain packed 16-bit integer values). Each block will form a 3D structure that will be handled by the vectorized kernel.

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
