#pragma once

#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/irtools.h"
#include "codon/dsl/dsl.h"
#include "codon/parser/visitors/typecheck/typecheck.h"

namespace vectron {

class Vectron : public codon::DSL {
public:
  void addIRPasses(codon::ir::transform::PassManager *, bool debug) override;
};

struct VectronFunctionTransformer : public codon::ir::util::Operator {
  std::vector<codon::ir::Var *> updatedVars;

  VectronFunctionTransformer();

  void handle(codon::ir::AssignInstr *) override;
  codon::ir::Value *getRange(codon::ir::ForFlow *);
};

class LoopVec : public codon::ir::transform::OperatorPass {
  const std::string KEY = "loop-vec";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::AssignInstr *) override;
};

class TernaryVec : public codon::ir::transform::OperatorPass {
  const std::string KEY = "ternary-vec";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::CallInstr *) override;
  codon::ir::Value *ternToCall(codon::ir::Value *,
                               const std::vector<codon::ir::types::Generic> &);
};

} // namespace vectron
