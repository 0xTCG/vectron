#include "env_select.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/matching.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <iomanip>

namespace vectron {

using namespace codon::ir;


void EvnSelector::transform(AssignInstr *v) {          
    auto var_name = v->getLhs()->getName();
    if (var_name == "var_type"){
        std::ofstream MyFile("var_type.txt");
        auto *var_value = v->getRhs();

        MyFile << *var_value;
        MyFile.close();

    }
    else{
        return;
    }



    
}


void EvnSelector::handle(AssignInstr *v) { transform(v); }

} // namespace vectron


