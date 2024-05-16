#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"

namespace vectron {

class ListInitializer : public codon::ir::transform::OperatorPass {
  const std::string KEY = "vectron-ListInitializer";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::CallInstr *) override;
  
  void transform(codon::ir::CallInstr *);
};

} // namespace vectron