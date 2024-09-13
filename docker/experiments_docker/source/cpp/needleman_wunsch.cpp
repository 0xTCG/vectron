#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "utils.h"
#include <fstream>
#include <iomanip> 

constexpr int SIZE = 512;
int QUANTITY = 0;
 
using dp_mat = std::vector<std::vector<int16_t>>;

void align(std::vector<int16_t> &scores, dp_mat &matrix,
           dp_mat &matrix_left, dp_mat &matrix_top,
           std::vector<std::pair<std::string, std::string>> const &sequences, int QUANTITY) {
    auto const gap_o = -4;
    auto const mismatch = -4;
    auto const match = 2;
    auto const gap_e = -2;
    auto const ambig = -3;

    for (int t = 0; t < QUANTITY; ++t) {
        matrix[0][0] = 0;
        matrix_left[0][0] = -10000;
        matrix_top[0][0] = -10000;
        for (int16_t i = 1; i < SIZE + 1; ++i) {
            matrix[0][i] = gap_o + gap_e * i;
            matrix[i][0] = gap_o + gap_e * i;
            matrix_left[0][i] = gap_o + gap_e * i;
            matrix_left[i][0] = -10000;
            matrix_top[0][i] = -10000;
            matrix_top[i][0] = gap_o + gap_e * i;
        }
        int16_t target_value;
        int16_t max_value = 0;
        for (int16_t i = 1; i < SIZE + 1; ++i) {
            for (int16_t j = 1; j < SIZE + 1; ++j) {

                int16_t diagonal_value = matrix[i - 1][j - 1];
                if (sequences[t].first[i - 1] == 'N' || sequences[t].second[j - 1] == 'N') {
                    diagonal_value += ambig;
                } else {
                    diagonal_value += (sequences[t].first[i - 1] == sequences[t].second[j - 1] ? match : mismatch);
                }
                int16_t top_value = matrix[i - 1][j] - 5;
                int16_t left_value = matrix[i][j - 1] - 5;
                int16_t temp = top_value - ((top_value - left_value) & ((top_value - left_value) >> (sizeof(int16_t) * 8 - 1)));
                target_value = diagonal_value - ((diagonal_value - temp) & ((diagonal_value - temp) >> (sizeof(int16_t) * 8 - 1)));
                matrix[i][j] = target_value;
                matrix_left[i][j] = left_value;
                matrix_top[i][j] = top_value;
                if (target_value > max_value)
                    max_value = target_value;
                
            }
        }
        scores[t] = target_value;//max_value - target_value > 800 ? target_value : -32768;
    }
}

void sw_cpu(std::vector<std::pair<std::string, std::string>> const &sequences, int QUANTITY) {
    std::vector<int16_t> scores(QUANTITY);
    dp_mat matrix(SIZE + 1, std::vector<int16_t>(SIZE + 1));
    dp_mat matrix_left(SIZE + 1, std::vector<int16_t>(SIZE + 1));
    dp_mat matrix_top(SIZE + 1, std::vector<int16_t>(SIZE + 1));

    auto const start_time = std::chrono::steady_clock::now();
    align(scores, matrix, matrix_left, matrix_top, sequences, QUANTITY);
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
