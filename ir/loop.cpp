#include "loop.h"
#include "codon/cir/cir.h"
#include "codon/cir/transform/pass.h"
#include "codon/cir/util/cloning.h"
#include "codon/cir/util/irtools.h"
#include "codon/parser/ast.h"
#include "codon/parser/match.h"
#include "codon/parser/visitors/format/format.h"
#include "codon/parser/visitors/scoping/scoping.h"
#include "codon/parser/visitors/translate/translate.h"
#include "codon/parser/visitors/typecheck/ctx.h"
#include "codon/parser/visitors/typecheck/typecheck.h"
#include "codon/parser/visitors/visitor.h"

namespace vectron {

using namespace codon;
using namespace codon::ir;
using namespace codon::matcher;

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

  auto *rangeType = M->getOrRealizeType(
      codon::ast::getMangledClass("std.internal.types.range", "range"), {});
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
      "std.vectron.lib");
  assert(dpmatFunc);

  auto *dpmatCall = util::call(dpmatFunc, {outerRange, innerRange});
  assert(dpmatCall);

  x->setRhs(dpmatCall);
  x->getLhs()->setType(util::getReturnType(dpmatFunc));
  updatedVars.push_back(x->getLhs());
}

// Search expression tree for a identifier
class ComprehensionSearchVisitor : public ast::CallbackASTVisitor<void, ast::Stmt *> {
  using fn_t = std::function<ast::Stmt *(ast::AssignStmt *, int)>;

  fn_t transformation;
  bool stop;
  int depth;
  ast::Stmt *modified;

  static int comprehensionCount;

public:
  ComprehensionSearchVisitor(const fn_t &transformation, int depth = 0)
      : transformation(transformation), stop(false), depth(depth), modified(nullptr) {}
  void transform(ast::Expr *expr) override {
    if (stop || !expr)
      return;
    ComprehensionSearchVisitor v(transformation, depth + 1);
    if (expr)
      expr->accept(v);
    stop = v.stop;
  }
  ast::Stmt *transform(ast::Stmt *stmt) override {
    if (stop || !stmt)
      return nullptr;
    ComprehensionSearchVisitor v(transformation,
                                 depth + (!ast::cast<ast::SuiteStmt>(stmt)));
    if (stmt)
      stmt->accept(v);
    stop = v.stop;
    return v.modified;
  }
  void visit(ast::SuiteStmt *stmt) override {
    for (auto &s : *stmt) {
      if (auto ns = transform(s)) {
        s = ns;
      }
    }
  }
  void visit(ast::AssignStmt *stmt) override {
    using namespace codon::ast;
    if (match(stmt, M<AssignStmt>(M<IdExpr>(),
                                  M<GeneratorExpr>(GeneratorExpr::ListGenerator, M_),
                                  M_, M_))) {
      modified = transformation(stmt, ComprehensionSearchVisitor::comprehensionCount++);
    }
  }
  void visit(ast::ForStmt *stmt) override {
    if (depth == 2)
      stop = true;
    else
      CallbackASTVisitor<void, ast::Stmt *>::visit(stmt);
  }
};

// Definition of the static member outside the class
int ComprehensionSearchVisitor::comprehensionCount = 0;

void LoopVec::handle(AssignInstr *w) {
  try {
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
    auto vectronAttr = codon::ast::getMangledFunc("std.vectron.__init__", "vectron");
    if (!bool(vectronFunc) || !util::hasAttribute(vectronFunc, vectronAttr))
      return;

    // @inumanag: begin change
    auto cache = v->getModule()->getCache();

    auto attr = vectronFunc->getAttribute<KeyValueAttribute>()->get(vectronAttr);
    auto dtype = cache->typeCtx->forceFind("int")->getType()->getClass();
    auto mode = 1;
    auto lane_size = 8;
    auto threads = 1;
    if (!attr.empty()) {
      auto f = cache->typeCtx->forceFind(attr)->getType();
      auto tv = ast::TypecheckVisitor(cache->typeCtx);
      lane_size = tv.extractFuncGeneric(f, 0)->getIntStatic()->value;
      dtype = tv.extractFuncGeneric(f, 1)->getClass();
      threads = tv.extractFuncGeneric(f, 2)->getIntStatic()->value;
      mode = tv.extractFuncGeneric(f, 3)->getIntStatic()->value;
    }
    LOG("[vectron] fn: {}, lane: {}, dtype: {}, threads: {}, mode: {}",
        vectronFunc->getName(), lane_size, dtype->debugString(0), threads, mode);

    seqassertn(dtype && in(cache->getClass(dtype)->realizations,
                          dtype->getClass()->realizedName()),
              "{} not realized", dtype ? dtype->debugString(0) : "<null>");
    auto vectronParams = std::vector<types::Generic>{
        types::Generic{lane_size},
        types::Generic{cache->getClass(dtype)
                           ->realizations[dtype->getClass()->realizedName()]
                           ->ir},
        types::Generic{threads},
    };

    // 1. Clone the function, change its name (FN -> FN.vectron)
    auto fnAst = cast<ast::FunctionStmt>(
        clean_clone(vectronFunc->getType()->getAstType()->getFunc()->ast));
    auto origFnName = fnAst->getName();
    auto fnName = fmt::format("{}.vectron", cache->rev(origFnName));
    fnAst->setName(fnName);
    fnAst->addParam(ast::Param{
        ".VECTRON_LANE", cache->N<ast::IndexExpr>(cache->N<ast::IdExpr>("Literal"),
                                                  cache->N<ast::IdExpr>("int"))});
    fnAst->addParam(ast::Param{".VECTRON_TYPE", cache->N<ast::IdExpr>("type")});
    fnAst->addParam(ast::Param{
        ".VECTRON_THREADS", cache->N<ast::IndexExpr>(cache->N<ast::IdExpr>("Literal"),
                                                     cache->N<ast::IdExpr>("int"))});

    // 2. Modify the function (wrap the comprehension into:
    //    def FN(): return COMPREHENSION; M = WRAPPER(FN)
    ComprehensionSearchVisitor([&](ast::AssignStmt *stmt,
                                   int comprehensionCount) -> ast::Stmt * {
      using namespace codon::ast;
      auto tmpFnName = cache->getTemporaryVar("vectron");
      return cache->N<SuiteStmt>(
          cache->N<FunctionStmt>(tmpFnName, nullptr, std::vector<Param>{},
                                 cache->N<ReturnStmt>(clone(stmt->getRhs()))),
          cache->N<AssignStmt>(
              clone(stmt->getLhs()),
              cache->N<CallExpr>(
                  cache->N<IdExpr>(getMangledFunc("std.vectron.lib", "dpmat_wrap")),
                  std::vector<CallArg>{
                      CallArg{"", cache->N<IdExpr>(tmpFnName)},
                      CallArg{"", cache->N<IntExpr>(comprehensionCount)},
                      CallArg{"LANE_SIZE", cache->N<IdExpr>(".VECTRON_LANE")},
                      CallArg{"T", cache->N<IdExpr>(".VECTRON_TYPE")},
                      CallArg{"THREADS", cache->N<IdExpr>(".VECTRON_THREADS")},
                  }),
              clone(stmt->getTypeExpr())));
    }).transform(fnAst);

    // 3. Check & typecheck new function
    auto s = cache->N<ast::SuiteStmt>(fnAst);
    if (auto err = ast::ScopingVisitor::apply(cache, s))
      throw exc::ParserException(std::move(err));
    auto transAst = ast::TypecheckVisitor::apply(cache->typeCtx, fnAst);
    ast::TranslateVisitor(cache->codegenCtx).translateStmts(transAst);

    // 4. Realize new function
    auto vecListType = util::getReturnType(
        M->getOrRealizeFunc("_get_vec_list", {}, vectronParams, "std.vectron.lib"));
    assert(vecListType && "_get_vec_list method not found in std.lib");
    std::vector<ir::types::Type *> newFnArgs;
    for (auto it = vectronFunc->arg_begin(); it != vectronFunc->arg_end(); ++it)
      newFnArgs.push_back(vecListType);
    auto newFn = v->getModule()->getOrRealizeFunc(fnName, newFnArgs, vectronParams);

    // @inumanag: end change

    std::vector<Value *> args;
    std::vector<types::Type *> tps;
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

    tps.push_back(newFn->getType());
    auto *alpernFunc =
        M->getOrRealizeFunc("alpern", tps, vectronParams, "std.vectron.lib");
    assert(alpernFunc);

    auto *alpernCall = util::call(alpernFunc, args);
    assert(alpernCall);

    w->setRhs(alpernCall);
  } catch (const exc::ParserException &e) {
    for (auto &trace : e.getErrors())
      for (auto &msg : trace)
        LOG("parser error at {}: {}", msg.getSrcInfo(), msg.getMessage());
    throw;
  }
}

} // namespace vectron
