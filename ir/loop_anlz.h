#pragma once

#include <algorithm>
#include <iterator>
#include <utility>

#include "codon/cir/cir.h"
#include "codon/cir/flow.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/analyze/dataflow/capture.h"
#include "codon/cir/analyze/dataflow/reaching.h"
#include "codon/cir/util/irtools.h"
#include "codon/cir/util/side_effect.h"


namespace vectron {

class LoopAnalyzer : public codon::ir::transform::OperatorPass {
  const std::string KEY = "vectron-loop-analysis";
  std::string getKey() const override { return KEY; }

  void handle(codon::ir::ImperativeForFlow *) override;
  
  void transform(codon::ir::ImperativeForFlow *);
};

} // namespace sequre