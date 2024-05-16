#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"

namespace vectron {

class FuncReplacement : public codon::ir::transform::OperatorPass {
  const std::string KEY = "vectron-funcreplacement";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::CallInstr *) override;
  
  void transform(codon::ir::CallInstr *);
};

} // namespace sequre