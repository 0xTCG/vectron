#include "env_select.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>


namespace vectron {

using namespace codon::ir;


void EvnSelector::transform(AssignInstr *v) {          
    auto var_name = v->getLhs()->getName();
    if (var_name == "var_type"){
        std::ofstream MyFile("var_type.txt");
        auto *var_value = v->getRhs();

        MyFile << *var_value;
        MyFile.close();
        std::ifstream varFile("var_type.txt");
        std::string var_value_str;
        std::getline(varFile, var_value_str);
        varFile.close();
        std::ifstream byPassFile("byPass.txt");
        std::string line;
        std::string first_line;
        std::string expression;
        if (byPassFile.is_open()) {
            for (int i = 0; i < 2; ++i) {
                std::getline(byPassFile, line);
                if (i == 0){
                    first_line = line;
                }
                if (i == 1) {
                    expression = line;
                    break;
                }
            }
            byPassFile.close();
        } else {
            std::cerr << "Unable to open byPass.txt\n";
            return;
        }

        // Tokenize the expression by spaces
        std::stringstream ss(expression);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        // Parenthesize non-operand expressions based on var_value
        std::string result;
        for (const auto& token : tokens) {
            if (token == "*" || token == "+" || token == "-" || token == "/" || token == "(" || token == ")" || token == "//") {
                // Operand: Append directly to result
                result += token;
            } else {
                // Non-operand: Encompass in f32(...) or i16(...) based on var_value
                result += (var_value_str == "\"i16\"") ? "i16(" + token + ")" : "f32(" + token + ")";
            }
            // Add a space after each token
            result += " ";
        }

        // Remove the trailing space
        result.pop_back();   
        std::ofstream outFile("byPass.txt");
        outFile << first_line << "\n";
        outFile << result;
        outFile.close();   

    }
    else{
        return;
    }



    
}


void EvnSelector::handle(AssignInstr *v) { transform(v); }

} // namespace vectron


