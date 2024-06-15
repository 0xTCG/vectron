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
#include <unordered_map> 


namespace vectron {
extern std::unordered_map<std::string, std::map<std::string, std::string>> globalAttributes; 
using namespace codon::ir;


void EvnSelector::transform(AssignInstr *v) {        
    auto var_name = v->getLhs()->getName();
    if (var_name == "var_type"){

        auto *var_value = v->getRhs();
        std::ostringstream var_oss;                                                                             
        var_oss << *var_value;
        std::string var_str = var_oss.str();
        std::map<std::string, std::string> var_attributes{{"Value", var_str}}; 
        globalAttributes["var"] = var_attributes;               
        auto bypass_attr = globalAttributes["bypass"];     
        std::string first_line = bypass_attr["Value"];
        std::string expression = bypass_attr["Expression"];

        // Tokenize the expression by spaces
        std::stringstream ss(expression);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        // Parenthesize non-operand expressions based on var_value
        std::string result = "";
        for (const auto& token : tokens) {
            if (token == "*" || token == "+" || token == "-" || token == "/" || token == "(" || token == ")" || token == "//" || token == "scores[i]") {
                // Operand: Append directly to result
                result += token;
            } else {
                // Non-operand: Encompass in f32(...) or i16(...) based on var_value
                result += (var_str == "\"i16\"") ? "i16(" + token + ")" : "f32(" + token + ")";
            }
            // Add a space after each token
            result += " ";
        }

        // Remove the trailing space
        result.pop_back();   
        std::map<std::string, std::string> attributes{{"Value", first_line}, {"Expression", result}}; 
        globalAttributes["bypass"] = attributes;    

    }
    else{
        return;
    }



    
}


void EvnSelector::handle(AssignInstr *v) { transform(v); }

} // namespace vectron


