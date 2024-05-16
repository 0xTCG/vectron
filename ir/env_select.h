#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"

namespace vectron {

class EvnSelector : public codon::ir::transform::OperatorPass {
  const std::string KEY = "vectron-VarTypeSelector";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::AssignInstr *) override;
  
  void transform(codon::ir::AssignInstr *);
};

} // namespace vectron