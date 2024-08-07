#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <seqan/seq_io.h>
#include <seqan/align.h>

using namespace seqan2;

constexpr int SIZE = 512;
int QUANTITY = 0;
constexpr int BATCH_SIZE = 262144;

void sw_cpu(const std::vector<std::pair<std::string, std::string>> &sequences, int QUANTITY) {
    std::vector<int> scores(QUANTITY);

    auto const start_time = std::chrono::steady_clock::now();

    for (int start = 0; start < QUANTITY; start += BATCH_SIZE) {
        int end = std::min(start + BATCH_SIZE, QUANTITY);

        // For each pair of sequences, perform Smith-Waterman alignment
        for (int t = start; t < end; ++t) {
            // Create SeqAn sequences
            Dna5String seq1 = sequences[t].first;
            Dna5String seq2 = sequences[t].second;

            // Create Align object
            Align<Dna5String> align;
            resize(rows(align), 2);
            assignSource(row(align, 0), seq1);
            assignSource(row(align, 1), seq2);

            // Define scoring scheme (match, mismatch, gap extension, gap opening)
            Score<int> scoringScheme(2, -4, -2, -4);

            // Perform local alignment (Smith-Waterman)
            int score = localAlignment(align, scoringScheme);

            scores[t] = score;

            // Optionally print alignment
            std::cout << "Score: " << score << "\n";
            std::cout << align << "\n";
        }
    }
    auto const end_time = std::chrono::steady_clock::now();
    std::cout << std::fixed << std::setprecision(2)
              << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1e6
              << " seconds" << std::endl;
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
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <target_file> <query_file>\n";
        return 1;
    }

    std::string target_file = argv[1];
    std::string query_file = argv[2];

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
