
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "utils.h"
#include <fstream>
#include <iomanip> 

constexpr int SIZE = 512;
int QUANTITY = 0;
constexpr int BATCH_SIZE = 262144;
 
using dp_mat = std::vector<std::vector<int16_t>>;

void align(std::vector<int16_t> &scores, std::vector<dp_mat> &matrices, 
           const std::vector<std::pair<std::string, std::string>> &sequences, int start, int end) {
    auto const gap_o = 1;
    auto const mismatch = 4;
    auto const match = 2;
    auto const gap_e_r = 2;
    auto const gap_e_c = 3;
    auto const ambig = 3;

    for (int t = start; t < end; ++t) {
        matrices[t - start][0][0] = 0;
        for (int16_t i = 1; i < SIZE + 1; ++i) {
            matrices[t - start][0][i] = gap_o + gap_e_c * i;
            matrices[t - start][i][0] = gap_o + gap_e_r * i;
        }
        int16_t target_value;
        int16_t max_value = 0;
        for (int16_t i = 1; i < SIZE + 1; ++i) {
            for (int16_t j = 1; j < SIZE + 1; ++j) {

                int16_t value = 0;
                if (sequences[t].first[i - 1] == 'N' || sequences[t].second[j - 1] == 'N') {
                    value += ambig;
                } else {
                    value += (sequences[t].first[i - 1] == sequences[t].second[j - 1] ? match : mismatch);
                }

                int16_t diagonal_value = matrices[t - start][i - 1][j - 1] + value;
                int16_t top_value = matrices[t - start][i - 1][j] + value;
                int16_t left_value = matrices[t - start][i][j - 1] + value;
                int16_t temp = top_value - ((top_value - left_value) & ((top_value - left_value) >> (sizeof(int16_t) * 8 - 1)));
                target_value = diagonal_value - ((diagonal_value - temp) & ((diagonal_value - temp) >> (sizeof(int16_t) * 8 - 1)));
                matrices[t - start][i][j] = target_value;
            }
        }
        scores[t] = target_value;
    }
}

void sw_cpu(const std::vector<std::pair<std::string, std::string>> &sequences, int QUANTITY) {
    std::vector<int16_t> scores(QUANTITY);

    auto const start_time = std::chrono::steady_clock::now();

    for (int start = 0; start < QUANTITY; start += BATCH_SIZE) {
        int end = std::min(start + BATCH_SIZE, QUANTITY);
        std::vector<dp_mat> matrices(end - start, dp_mat(SIZE + 1, std::vector<int16_t>(SIZE + 1)));
        align(scores, matrices, sequences, start, end);
    }

    for (auto e : scores) {
        std::cout << e << "\n";
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

    sw_cpu(paired_sequences, QUANTITY);

    return 0;
}
