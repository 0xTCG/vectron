#include "list_init.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace vectron {
extern std::unordered_map<std::string, std::map<std::string, std::string>>
    globalAttributes;
using namespace codon::ir;

std::string mergeVector(const std::vector<std::string> &vec) {
  std::string result;
  for (const auto &str : vec) {
    result += str + "\n";
  }
  return result;
}

std::vector<std::string> split_first(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    size_t pos = s.find(delimiter);
    if (pos != std::string::npos) {
        tokens.push_back(s.substr(0, pos));
        tokens.push_back(s.substr(pos + 1));
    } else {
        tokens.push_back(s);
    }
    return tokens;
}

// Trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

void ListInitializer::transform(CallInstr *v) {

  auto *M = v->getModule();
  auto *orig = util::getFunc(v->getCallee());
  auto att_att = util::hasAttribute(orig, "std.vectron.dispatcher.vectron_kernel");
  if (!att_att)
    return;

  // Open the Python script file
  std::ifstream python_script(v->getSrcInfo().file);
  if (!python_script) {
    std::cerr << "Error: Unable to open the Python script\n";
    return;
  }

  std::string line;
  bool in_function = false;
  bool in_vectron_calc = false;
  bool rest = false;
  std::vector<std::string> extracted_lines;
  std::vector<std::string> rest_lines;

  // Iterate through Python script
  while (std::getline(python_script, line)) {
    if (!line.empty()) {
      size_t first_non_space_index = line.find_first_not_of(" \t");

      // Extract the substring starting from the first non-space character
      std::string trimmed_line = line.substr(first_non_space_index);
      // Check for function decorator
      if (trimmed_line.find("@vectron_kernel") != std::string::npos) {
        in_function = true;
        in_vectron_calc = true;
        continue;
      }

      // Check if inside the function and not yet encountered a for loop
      if (in_function && in_vectron_calc) {
        if (trimmed_line.find("for ") != std::string::npos &&
            trimmed_line.find(":") != std::string::npos) {
          // Found for loop, stop extracting lines
          rest = true;
        }
        if (trimmed_line.find("@vectron") != std::string::npos ||
            trimmed_line.find("return") != std::string::npos) {
          break;
        }
        if (trimmed_line.find("def ") == std::string::npos && trimmed_line[0] != '#') {
          if (rest) {
            rest_lines.push_back(line);
          } else {

            extracted_lines.push_back(trimmed_line);
          }
        }
      }
    }
  }

  // Close the Python script file
  python_script.close();
  if (extracted_lines.size() < 1 || extracted_lines.size() > 3) {
    std::cerr << "Error: Extracted lines count should be between 1 to 3 "
              << extracted_lines.size() << std::endl;
    return;
  }
  int for_loop_count = 0;
  std::vector<std::string> processed_rest_lines;
  int index = 0;
  for (int i = rest_lines.size() - 1; i >= 0; i--) {
    if (rest_lines[i].find("for ") != std::string::npos &&
        rest_lines[i].find(":") != std::string::npos) {
      for_loop_count++;
      if (for_loop_count >= 2) {
        index = i;
        break;
      }
    } else {
      for_loop_count = 0;
    }
  }
  for (int i = 0; i < index; i++) {
    processed_rest_lines.push_back(rest_lines[i]);
  }
  std::vector<std::string> replacement = {"matrices_nv", "V_nv", "H_nv"};
  // Create and write to output files
  std::regex len_regex("len\\([^\\)]*\\)");
  for (int i = 0; i < extracted_lines.size(); ++i) {
    std::string outfile_name = "lst_";
    outfile_name += std::to_string(i + 1);
    std::vector<std::string> parts = split_first(extracted_lines[i], '=');
    std::ostringstream oss;
    oss << parts[0][0];
    std::string parts_name = oss.str();
    if (parts.size() != 2) {
      std::cerr << "Error: Unable to parse line\n";
      continue;
    }
    for (int j = 0; j < processed_rest_lines.size(); j++) {
      size_t pos = 0;
      while ((pos = processed_rest_lines[j].find(parts_name, pos)) !=
             std::string::npos) {
        processed_rest_lines[j].replace(pos, parts_name.length(), replacement[i]);
        // Move `pos` past the replaced part
        pos += replacement[i].length();
      }
    }
    parts[1] = std::regex_replace(parts[1], len_regex, "SIZE");

    trim(parts[0]);
    trim(parts[1]);
    std::map<std::string, std::string> outfile_attributes{{"part_0", parts[0]},
                                                          {"part_1", parts[1]}};
    globalAttributes[outfile_name] = outfile_attributes;
  }
  for (int j = 0; j < processed_rest_lines.size(); j++) {
    processed_rest_lines[j] =
        std::regex_replace(processed_rest_lines[j], len_regex, "SIZE");
  }
  std::string merged_lines = mergeVector(processed_rest_lines);
  std::map<std::string, std::string> merged_atts{{"rest", merged_lines}};
  globalAttributes["merged"] = merged_atts;
}

void ListInitializer::handle(CallInstr *v) { transform(v); }

} // namespace vectron

