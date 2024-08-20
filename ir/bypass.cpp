#include "bypass.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <regex>
#include <unordered_map>


namespace vectron {
std::unordered_map<std::string, std::map<std::string, std::string>> globalAttributes;
using namespace codon::ir;


void byPass::transform(ReturnInstr *v) {
    auto *pf = getParentFunc();
    auto pf_name = pf->getUnmangledName();
    auto att_calc = util::hasAttribute(pf, "std.vectron.dispatcher.vectron_kernel");
    if(!att_calc)
        return;
    std::vector<codon::ir::Value *> func = v->getUsedValues();
    auto *func_log = cast<CallInstr>(func[0]);
    auto func_name = util::getFunc(func_log->getCallee())->getUnmangledName();
    auto *by_pass_func = util::getFunc(func_log->getCallee());
    auto att_bypass = util::hasAttribute(by_pass_func, "std.vectron.dispatcher.vectron_bypass");
    if(!att_bypass){
        std::ifstream python_script(v->getSrcInfo().file);
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
                if (trimmed_line.find("@vectron_kernel") != std::string::npos) {
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
            std::cerr << "Error: No return statement found in function decorated with @vectron\n";
            return;
        }
        //int bp;
        //pf->setAttribute(bp, -1);
        std::map<std::string, std::string> attributes{{"Value", "-1"}, {"Expression", extracted_lines[0]}};
        //pf->setAttribute(std::make_unique<codon::ir::KeyValueAttribute>(attributes));
        globalAttributes["bypass"] = attributes;
    }
    else{
        auto *z_value = func_log->back();
        std::ostringstream oss;
        oss << *z_value;
        std::string z_value_str = oss.str();
        std::map<std::string, std::string> attributes{{"Value", z_value_str}, {"Expression", "score"}};
        globalAttributes["bypass"] = attributes;
    }

}
void byPass::handle(ReturnInstr *v) { transform(v); }

} // namespace vectron
