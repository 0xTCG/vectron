#include "bypass.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <regex>

namespace vectron {

using namespace codon::ir;


void byPass::transform(ReturnInstr *v) {
    if(std::filesystem::exists("LoopInfo.txt")){
        std::remove("LoopInfo.txt");
    }
    if(std::filesystem::exists("Prep_info.txt")){
        std::remove("Prep_info.txt");
    }
    if(std::filesystem::exists("arg_1.txt")){
        std::remove("arg_1.txt");
    }
    if(std::filesystem::exists("arg_2.txt")){
        std::remove("arg_2.txt");
    }
    if(std::filesystem::exists("arg_3.txt")){
        std::remove("arg_3.txt");
    }
    if(std::filesystem::exists("mx_arg1.txt")){
        std::remove("mx_arg1.txt");
    }
    if(std::filesystem::exists("mx_arg2.txt")){
        std::remove("mx_arg2.txt");
    }
    if(std::filesystem::exists("mx_arg3.txt")){
        std::remove("mx_arg3.txt");
    }
    if(std::filesystem::exists("lst_1.txt")){
        std::remove("lst_1.txt");
    }
    if(std::filesystem::exists("lst_2.txt")){
        std::remove("lst_2.txt");
    }
    if(std::filesystem::exists("lst_3.txt")){
        std::remove("lst_3.txt");
    }            
    auto *pf = getParentFunc();
    auto pf_name = pf->getUnmangledName();
    auto att_calc = util::hasAttribute(pf, "vectron_calc");    
    if(!att_calc)
        return;
    std::ofstream MyFile("byPass.txt");
    std::vector<codon::ir::Value *> func = v->getUsedValues();
    auto *func_log = cast<CallInstr>(func[0]);
    auto func_name = util::getFunc(func_log->getCallee())->getUnmangledName();     
    auto *by_pass_func = util::getFunc(func_log->getCallee());
    auto att_bypass = util::hasAttribute(by_pass_func, "vectron_bypass");    
    if(!att_bypass){
        // Open the file containing the path to Python script
        std::ifstream infile("script_name.txt");
        if (!infile) {
            std::cerr << "Error: Unable to open script_name.txt\n";
            return;
        }

        std::string python_script_path;
        std::getline(infile, python_script_path);

        // Open the Python script file
        std::ifstream python_script(python_script_path);
        if (!python_script) {
            std::cerr << "Error: Unable to open the Python script\n";
            return;
        }

        std::string line;
        bool in_function = false;
        bool in_vectron_calc = false;
        std::vector<std::string> extracted_lines;

        // Iterate through Python script
        while (std::getline(python_script, line)) {
            if (!line.empty()) {
                size_t first_non_space_index = line.find_first_not_of(" \t");

                // Extract the substring starting from the first non-space character
                std::string trimmed_line = line.substr(first_non_space_index);        

                // Check for function decorator
                if (trimmed_line.find("@vectron_calc") != std::string::npos) {
                    in_function = true;
                    in_vectron_calc = true;
                    continue;
                }
                // Check if inside the function and not yet encountered a for loop
                if (in_function && in_vectron_calc) {
                    if (trimmed_line.find("return ") != std::string::npos) {
                        // Found return statement, extract whatever is after it
                        std::string extracted_text = trimmed_line.substr(trimmed_line.find("return ") + 7);

                        // Remove anything starting with '#' and after
                        size_t comment_pos = extracted_text.find("#");
                        if (comment_pos != std::string::npos) {
                            extracted_text = extracted_text.substr(0, comment_pos);
                        }
                        std::regex regex(R"(\b\w+(?:\[[^\]]*\])*\[-1\]\[-1\])");
                        extracted_text = std::regex_replace(extracted_text, regex, "scores[i]");

                        // Store the extracted text if it's not empty
                        if (!extracted_text.empty()) {
                            extracted_lines.push_back(extracted_text);
                        }
                        break;
                    }                 
                }

            }
        }

        // Close the Python script file
        python_script.close();
        if (extracted_lines.empty()) {
            std::cerr << "Error: No return statement found in function decorated with @vectron_calc\n";
            return;
        }
        MyFile << -1 << "\n";
        MyFile << extracted_lines[0];
    }
    else{
        auto *z_value = func_log->back();
        MyFile << *z_value << "\n";
        MyFile << "score";
    }
    
}
void byPass::handle(ReturnInstr *v) { transform(v); }

} // namespace vectron