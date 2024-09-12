#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include "utils.h"
#include <fstream>
#include <iomanip>

constexpr int SIZE = 512;
int QUANTITY = 0;
 
using dp_mat = std::vector<std::vector<int>>;

void init(std::vector<dp_mat> &matrices, std::vector<dp_mat> &matrices_left, std::vector<dp_mat>             
&matrices_top) {                                                                                             
    auto const gap_o = -4;                                                                                 
    auto const gap_e = -2;                                                                                 
                                                                                                            
    for (int t = 0; t < QUANTITY; ++t) {                                                                     
        matrices[t][0][0] = 0;                                                                             
        matrices_left[t][0][0] = -10000;                                                                   
        matrices_top[t][0][0] = -10000;                                                                    
        for (int i = 1; i < SIZE + 1; ++i) {                                                                 
            matrices[t][0][i] = gap_o + gap_e * i;                                                           
            matrices[t][i][0] = gap_o + gap_e * i;                                                           
            matrices_left[t][0][i] = gap_o + gap_e * i;                                                      
            matrices_left[t][i][0] = -10000;                                                               
            matrices_top[t][0][i] = -10000;                                                                
            matrices_top[t][i][0] = gap_o + gap_e * i;                                                       
        }                                                                                                    
    }                                                                                                        
}                                                                                                            

//#pragma scop
                                                                                                         
void align(std::vector<int> &scores, std::vector<dp_mat> &matrices,                                          
        std::vector<dp_mat> &matrices_left, std::vector<dp_mat> &matrices_top,                            
        std::vector<std::pair<std::string, std::string>> const &sequences, int QUANTITY) {                              
    auto const mismatch = -4;                                                                              
    auto const match = 2;                                                                                  
    auto const ambig = -3;                                                                                 
    auto const gap_o = -4;                                                                                 
    auto const gap_e = -2;                                                                                                                 
    init(matrices, matrices_left, matrices_top);   
    for (int t = 0; t < QUANTITY; ++t) {                                                                     
        int target_value;
        int max_value = 0;
        for (int i = 1; i < SIZE + 1; ++i) {
            for (int j = 1; j < SIZE + 1; ++j) {
                if (j - i <= -105 || j - i >= 105) {
                    matrices[t][i][j] = -10000;
                    matrices_left[t][i][j] = -10000;
                    matrices_top[t][i][j] = -10000;
                } else {
                    int diagonal_value = matrices[t][i - 1][j - 1];
                    if (sequences[t].first[i - 1] == 'N' || sequences[t].second[j - 1] == 'N') {
                        diagonal_value += ambig;
                    } else {
                        diagonal_value += (sequences[t].first[i - 1] == sequences[t].second[j - 1] ? match : mismatch);
                    }
                    int top_value = (matrices[t][i - 1][j] + gap_o + gap_e > matrices_top[t][i - 1][j] + gap_e ? matrices[t][i - 1][j] + gap_o + gap_e : matrices_top[t][i - 1][j] + gap_e);
                    int left_value = (matrices[t][i][j - 1] + gap_o + gap_e > matrices_left[t][i][j - 1] + gap_e ? matrices[t][i][j - 1] + gap_o + gap_e : matrices_left[t][i][j - 1] + gap_e);
                    target_value = std::max(std::max(diagonal_value, top_value), left_value);
                    matrices[t][i][j] = target_value;
                    matrices_left[t][i][j] = left_value;
                    matrices_top[t][i][j] = top_value;
                    if (target_value > max_value)
                        max_value = target_value;
                }
            }
        }
        scores[t] = target_value;
    }
}

//#pragma endscop

void sw_cpu(std::vector<std::pair<std::string, std::string>> const &sequences, int QUANTITY) {
    std::vector<int> scores(QUANTITY);
    std::vector<dp_mat> matrices(QUANTITY, dp_mat(SIZE + 1, std::vector<int>(SIZE + 1)));
    std::vector<dp_mat> matrices_left(QUANTITY, dp_mat(SIZE + 1, std::vector<int>(SIZE + 1)));
    std::vector<dp_mat> matrices_top(QUANTITY, dp_mat(SIZE + 1, std::vector<int>(SIZE + 1)));
    std::cout << "Before Align\n";
    auto const start_time = std::chrono::steady_clock::now();
    align(scores, matrices, matrices_left, matrices_top, sequences, QUANTITY);
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
    std::cout << "Quantity Before: " << QUANTITY << std::endl;
    QUANTITY = target_sequences.size();
    std::cout << "Quantity After: " << QUANTITY << std::endl;
    if (target_sequences.size() != query_sequences.size()) {
        std::cerr << "Error: Number of target sequences does not match number of query sequences." << std::endl;
        std::cout << "Target Size: " << target_sequences.size() << ", Query Size: " << query_sequences.size() << "\n";
        return 1;
    }

    std::vector<std::pair<std::string, std::string>> paired_sequences = pair_sequences(target_sequences, query_sequences);

    sw_cpu(paired_sequences, QUANTITY);

    return 0;
}
