#pragma once

#include "codon/dsl/dsl.h"

namespace vectron {

class Vectron : public codon::DSL {
public:
  void addIRPasses(codon::ir::transform::PassManager *pm, bool debug) override;
};

} // namespace vectron