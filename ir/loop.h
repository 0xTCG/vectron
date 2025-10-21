#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/transform/lowering/imperative.h"

namespace vectron {

using namespace codon::ir;

struct VectronFunctionTransformer : public util::Operator {
  std::vector<Var *> updatedVars;

  Value *getRange(ForFlow *forFl);
  void handle(AssignInstr *x) override;
  VectronFunctionTransformer()
      : util::Operator(), updatedVars() {}
};

class LoopVec : public transform::OperatorPass {
  const std::string KEY = "loop-vec";
  std::string getKey() const override { return KEY; }

  void handle(AssignInstr *w) override;
};

} // namespace sequre
