#include "loop.h"
#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/parser/ast.h"
#include "codon/parser/visitors/format/format.h"
#include "codon/parser/visitors/translate/translate.h"
#include "codon/parser/visitors/typecheck/ctx.h"
#include "codon/parser/visitors/typecheck/typecheck.h"
#include "codon/parser/visitors/visitor.h"

namespace vectron {

using namespace codon;
using namespace codon::ir;

Value *VectronFunctionTransformer::getRange(ForFlow *forFl) {
  auto *M = forFl->getModule();
  auto *iter = forFl->getIter();

  auto *iterCall = cast<CallInstr>(iter);
  if (!iterCall || iterCall->numArgs() != 1)
    return nullptr;

  auto *iterFunc = util::getFunc(iterCall->getCallee());
  if (!iterFunc || iterFunc->getUnmangledName() != Module::ITER_MAGIC_NAME)
    return nullptr;

  auto *rangeVal = iterCall->front();
  if (!rangeVal)
    return nullptr;

  auto *rangeType = M->getOrRealizeType(codon::ast::getMangledClass("std.internal.types.range", "range"), {});
  if (!rangeVal->getType()->is(rangeType))
    return nullptr;

  return util::tupleGet(rangeVal, 1);
}

void VectronFunctionTransformer::handle(AssignInstr *x) {
  auto *M = x->getModule();
  auto *rhs = x->getRhs();
  auto *v = cast<FlowInstr>(rhs);
  if (!v)
    return;

  auto *forFl = cast<ForFlow>(cast<SeriesFlow>(v->getFlow())->back());
  // If not for flow then it is not comprehension.
  if (!forFl)
    return;

  auto *outerRange = getRange(forFl);
  if (outerRange == nullptr)
    return;

  auto *innerFl =
      cast<FlowInstr>(
          cast<CallInstr>(cast<SeriesFlow>(forFl->getBody())->back())->back())
          ->getFlow();
  if (!innerFl)
    return;

  auto *innerForFl = cast<ForFlow>(cast<SeriesFlow>(innerFl)->back());
  // If not inner for flow then it is not a double comprehension.
  if (!innerForFl)
    return;

  auto *innerRange = getRange(innerForFl);
  if (innerRange == nullptr)
    return;

  auto *whatCall = cast<CallInstr>(cast<SeriesFlow>(innerForFl->getBody())->back());
  if (!whatCall)
    return;

  auto *initVal = cast<IntConst>(whatCall->back());
  if (!initVal)
    return;

  auto *dpmatFunc = M->getOrRealizeFunc(
      "dpmat", {M->getIntType(), M->getIntType()},
      {types::Generic(initVal->getVal()), types::Generic(updatedVars.size())},
      "std.lib");
  assert(dpmatFunc);

  auto *dpmatCall = util::call(dpmatFunc, {outerRange, innerRange});
  assert(dpmatCall);

  x->setRhs(dpmatCall);
  x->getLhs()->setType(util::getReturnType(dpmatFunc));
  updatedVars.push_back(x->getLhs());
}

// Search expression tree for a identifier
class ComprehensionSearchVisitor : public ast::CallbackASTVisitor<void, void> {
  bool stop;
  int depth;
  std::string wrapFunc, castType;

public:
  ComprehensionSearchVisitor(std::string wrapFunc, std::string castType, int depth = 0)
      : stop(false), depth(depth), wrapFunc(std::move(wrapFunc)),
        castType(std::move(castType)) {}
  void transform(const ast::ExprPtr &expr) override {
    if (stop || !expr)
      return;
    ComprehensionSearchVisitor v(wrapFunc, castType, depth + 1);
    if (expr)
      expr->accept(v);
    stop = v.stop;
  }
  void transform(const ast::StmtPtr &stmt) override {
    if (stop || !stmt)
      return;
    ComprehensionSearchVisitor v(wrapFunc, castType, depth + (!stmt->getSuite()));
    if (stmt)
      stmt->accept(v);
    stop = v.stop;
  }
  void visit(ast::AssignStmt *stmt) override {
    if (stmt->lhs->getId() && stmt->rhs->getIf()) {
      auto s = stmt->rhs->getIf()->elsexpr->getStmtExpr();
      if (s && !s->stmts.empty() && s->stmts[0]->getSuite()) {
        auto sp = s->stmts[0]->getSuite();
        if (!sp->stmts.empty() && sp->stmts[0]->getAssign()) {
          auto i = sp->stmts[0]->getAssign()->lhs->getId();
          if (i && ast::startswith(i->value, "._gen_")) {
            stmt->rhs = N<ast::CallExpr>(N<ast::IdExpr>(wrapFunc), stmt->rhs);
          }
        }
      }
    }
  }
  void visit(ast::ForStmt *stmt) override {
    if (depth == 2)
      stop = true;
    else
      CallbackASTVisitor<void, void>::visit(stmt);
  }
  // void visit(ast::CallExpr *expr) override {
  //   if (expr->expr->getDot() && expr->expr->getDot()->expr->getId() &&
  //       ast::startswith(expr->expr->getDot()->expr->getId()->value, "._gen_") &&
  //       expr->expr->getDot()->member == "append" && expr->args.size() == 1) {
  //     transform(expr->args.front().value);
  //     expr->args.front().value = N<ast::CallExpr>(
  //         N<ast::IdExpr>(wrapFunc), expr->args.front().value, N<ast::IdExpr>(castType));
  //   } else {
  //     CallbackASTVisitor<void, void>::visit(expr);
  //   }
  // }
};

void LoopVec::handle(AssignInstr *w) {
  auto *v = cast<FlowInstr>(w->getRhs());
  if (!v)
    return;
  
  auto *M = v->getModule();

  // Skip if inner comprehension.
  if (bool(findLast<ForFlow>()))
    return;

  auto *forFl = cast<ForFlow>(cast<SeriesFlow>(v->getFlow())->back());
  // If not for flow then it is not comprehension.
  if (!forFl)
    return;

  auto *innerFl = cast<FlowInstr>(
      cast<CallInstr>(cast<SeriesFlow>(forFl->getBody())->back())->back());
  if (!innerFl || !innerFl->getFlow())
    return;

  auto *innerForFl = cast<ForFlow>(cast<SeriesFlow>(innerFl->getFlow())->back());
  // If not inner for flow then it is not a double comprehension.
  if (!innerForFl)
    return;

  auto *vectronCall = cast<CallInstr>(
      cast<CallInstr>(cast<SeriesFlow>(innerForFl->getBody())->back())->back());
  // If there is no inner call then it cannot be a typical Vectron use-case
  if (!vectronCall)
    return;

  auto *vectronFunc = util::getFunc(vectronCall->getCallee());
  if (!bool(vectronFunc) ||
      !util::hasAttribute(vectronFunc, "std.vectron.attributes.vectron.0:0"))
    return;

  // LOG("VEC TYPE -> {}", util::getReturnType(M->getOrRealizeFunc("_get_vec_type", {}, {}, "std.lib"))->getName());

  // @inumanag: begin change

  auto cache = v->getModule()->getCache();
  auto fnAst = vectronFunc->getType()->getAstType()->getFunc()->ast->clone();
  auto origFnName = fnAst->getFunction()->name;
  fnAst->getFunction()->name += "_vectron";
  auto fnName = fnAst->getFunction()->name;
  auto cv = ComprehensionSearchVisitor(
    "std.lib.dpmat_temp", // I use this as wrapper, feel free to change
    "std.experimental.simd.Vec[Int[16],32]" // init type, get it here; check dpmat_cast
                                            // as well
  );
  cv.transform(fnAst);

  cache->functions[fnName].ast = std::static_pointer_cast<ast::FunctionStmt>(fnAst);
  cache->functions[fnName].isToplevel = cache->functions[origFnName].isToplevel;
  cache->functions[fnName].rootName = fnName;
  cache->reverseIdentifierLookup[fnName] =
    cache->reverseIdentifierLookup[origFnName] + ".vectron";
  fnAst = ast::TypecheckVisitor::apply(cache, fnAst);
  ast::TranslateVisitor(cache->codegenCtx).transform(fnAst);

  LOG("RAW DEBUG -> {}", ast::FormatVisitor(false, cache).transform(fnAst));

  auto vecListType = util::getReturnType(M->getOrRealizeFunc("_get_vec_list", {}, {}, "std.lib"));
  assert(vecListType && "_get_vec_list method not found in std.lib");

  std::vector<ir::types::Type *> newFnArgs;
  for (auto it = vectronFunc->arg_begin(); it != vectronFunc->arg_end(); ++it)
    newFnArgs.push_back(vecListType);
  auto newFn = v->getModule()->getOrRealizeFunc(fnName, newFnArgs);
  LOG("DEBUG -> {}", *newFn);

  // @inumanag: end change

  VectronFunctionTransformer vft;
  // For Ibrahim: I would like to get a clean clone of vectronFunc here
  auto *clone = cast<BodiedFunc>(newFn);
  // clone->getBody()->accept(vft);
  // For Ibrahim: I would like to realize clone types here
  // if (vft.updatedVars.empty())
  //   return;

  std::vector<Value *> args = {M->Nr<VarValue>(clone)};
  std::vector<types::Type *> tps = {clone->getType()};
  for (auto it = vectronCall->begin(); it != vectronCall->end(); ++it) {
    auto *getitemCall = cast<CallInstr>(*it);
    assert(getitemCall &&
           "Arguments in Vectron method in comprehension detected but not accessed. "
           "Did you forget to use __getitem__ (i.e. x[i])?");

    auto *getitem = util::getFunc(getitemCall->getCallee());
    assert(getitem && getitem->getUnmangledName() == Module::GETITEM_MAGIC_NAME &&
           "Arguments in Vectron method in comprehension detected but not accessed. "
           "Did you forget to use __getitem__ (i.e. x[i])?");

    auto *arg = getitemCall->front();
    args.push_back(arg);
    tps.push_back(arg->getType());
  }

  auto *alpernFunc = M->getOrRealizeFunc("alpern", tps, {}, "std.lib");
  assert(alpernFunc);

  auto *alpernCall = util::call(alpernFunc, args);
  assert(alpernCall);

  w->setRhs(alpernCall);

  LOG("DEBUG FINAL -> {}", *w);

}

} // namespace vectron
