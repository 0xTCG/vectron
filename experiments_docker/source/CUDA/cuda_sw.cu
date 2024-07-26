#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <utility>
#include <chrono>
#include <cuda_runtime.h>
#include <iomanip> 
#include "utils.h"


constexpr int SIZE = 512; 
int QUANTITY = 0;
constexpr int CUDA_XBLOCK_SIZE = 256; 

using dp_mat = float[SIZE + 1][SIZE + 1];

__global__
void align(float *scores, dp_mat *matrices, dp_mat *matrices_left, dp_mat *matrices_top, char *sequences, size_t size) {
    const int t = threadIdx.x + blockIdx.x * blockDim.x;

    if (t >= QUANTITY)
        return;

    const float gap_o = -4.0f;
    const float mismatch = -4.0f;
    const float match = 2.0f;
    const float gap_e = -2.0f;
    const float ambig = -3.0f;

    matrices[t][0][0] = 0.0f;
    matrices_left[t][0][0] = -10000.0f;
    matrices_top[t][0][0] = -10000.0f;
    for (int i = 1; i < size + 1; ++i) {
        matrices[t][0][i] = gap_o + gap_e * i;
        matrices[t][i][0] = gap_o + gap_e * i;
        matrices_left[t][0][i] = gap_o + gap_e * i;
        matrices_left[t][i][0] = -10000.0f;
        matrices_top[t][0][i] = -10000.0f;
        matrices_top[t][i][0] = gap_o + gap_e * i;
    }

    float target_value;
    float max_value = 0.0f;
    for (int i = 1; i < size + 1; ++i) {
        for (int j = 1; j < size + 1; ++j) {
            if (j - i <= -105 || j - i >= 105) {
                matrices[t][i][j] = -10000.0f;
                matrices_left[t][i][j] = -10000.0f;
                matrices_top[t][i][j] = -10000.0f;   
            }
            else {
                float diagonal_value = matrices[t][i - 1][j - 1];
                if (sequences[t * size * 2 + i - 1] == 'N' || sequences[t * size * 2 + j - 1 + size] == 'N') {
                    diagonal_value += ambig;
                } else {
                    diagonal_value += (sequences[t * size * 2 + i - 1] == sequences[t * size * 2 + j - 1 + size] ? match : mismatch);
                }            
                float top_value =  (matrices[t][i - 1][j] + gap_o + gap_e > matrices_top[t][i - 1][j] + gap_e ? matrices[t][i - 1][j] + gap_o + gap_e : matrices_top[t][i - 1][j] + gap_e);
                float left_value = (matrices[t][i][j - 1] + gap_o + gap_e > matrices_left[t][i][j - 1] + gap_e ? matrices[t][i][j - 1] + gap_o + gap_e : matrices_left[t][i][j - 1] + gap_e);
                float temp = top_value - ((top_value - left_value) * (top_value - left_value < 0));
                target_value = diagonal_value - ((diagonal_value - temp) * (diagonal_value - temp < 0));
                matrices[t][i][j] = target_value;
                matrices_left[t][i][j] = left_value;
                matrices_top[t][i][j] = top_value;        
                if(target_value > max_value)
                    max_value = target_value;
            } 
        }
    }
    scores[t] = target_value;
}

void sw_cuda_alpern(std::vector<std::pair<std::string, std::string>> const sequences) {
    int const num_blocks = QUANTITY / CUDA_XBLOCK_SIZE;

    std::vector<float> scores(QUANTITY);

    dp_mat *dev_matrices;
    dp_mat *dev_matrices_top;
    dp_mat *dev_matrices_left;
    char *dev_input;
    float *dev_output;
    int64_t matrices_size = QUANTITY * sizeof(dp_mat);
    int64_t output_size = QUANTITY * sizeof(float);
    int64_t input_size = QUANTITY * SIZE * 2;
    auto const start_time = std::chrono::steady_clock::now();
    cudaMalloc((void **)&dev_output, output_size);
    cudaMalloc((void **)&dev_matrices, matrices_size);
    cudaMalloc((void **)&dev_matrices_top, matrices_size);
    cudaMalloc((void **)&dev_matrices_left, matrices_size);
    cudaMalloc((void **)&dev_input, input_size);

    const char *sequences_bytes = to_byte_arr(sequences);

    cudaMalloc((void **)&dev_input, input_size);
    cudaError_t mallocError = cudaGetLastError();
    if (mallocError != cudaSuccess) {
        std::cerr << "CUDA malloc error: " << cudaGetErrorString(mallocError) << std::endl;
        return; 
    }
    cudaMemcpy(dev_input, sequences_bytes, input_size, cudaMemcpyHostToDevice);
    auto const start_time_kernel = std::chrono::steady_clock::now();
    align<<<num_blocks, CUDA_XBLOCK_SIZE>>>(dev_output, dev_matrices, dev_matrices_left, dev_matrices_top, dev_input, SIZE);
    auto const end_time_kernel = std::chrono::steady_clock::now();    
    cudaMemcpy(scores.data(), dev_output, output_size, cudaMemcpyDeviceToHost);
    
    cudaFree(dev_input);
    cudaFree(dev_matrices);
    cudaFree(dev_matrices_left);
    cudaFree(dev_matrices_top);
    cudaFree(dev_output);
    //for (auto e : scores) {
    //    std::cout << e << "\n";
    //}
    // Simulating the score printing CPU load
    volatile float dummy;
    for (auto e : scores) {
        dummy = e;
    }
    auto const end_time = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2) 
          << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1e6 
          << std::endl;
}

std::vector<std::string> read_sequences_from_file(const std::string &filename) {
    std::ifstream file(filename);
    std::vector<std::string> sequences;
    std::string line;
    while (std::getline(file, line)) {
        sequences.push_back(line);
    }
    return sequences;
}

std::vector<std::pair<std::string, std::string>> pair_sequences(const std::vector<std::string> &targets,
                                                               const std::vector<std::string> &queries) {
    std::vector<std::pair<std::string, std::string>> pairs;
    for (size_t i = 0; i < targets.size(); ++i) {
        pairs.emplace_back(targets[i], queries[i]);
    }
    return pairs;
}

int main(int argc, char* argv[]) {
    std::string target_file = argv[argc - 1];
    std::string query_file = argv[argc - 2];

    std::vector<std::string> target_sequences = read_sequences_from_file(target_file);
    std::vector<std::string> query_sequences = read_sequences_from_file(query_file);
    QUANTITY = target_sequences.size();
    if (target_sequences.size() != query_sequences.size()) {
        std::cerr << "Error: Number of target sequences does not match number of query sequences." << std::endl;
        std::cout << "Target Size: " << target_sequences.size() << ", Query Size: " << query_sequences.size() << "\n";
        return 1;
    }

    std::vector<std::pair<std::string, std::string>> paired_sequences = pair_sequences(target_sequences, query_sequences);

    sw_cuda_alpern(paired_sequences);

    return 0;
}
