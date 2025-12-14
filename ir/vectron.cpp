#include "vectron.h"
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

enum VectronMode { SIMD, GPU };

std::string getVectronModuleName(const std::string &what) {
  return "std.vectron.stdlib.vectron." + what;
}

void Vectron::addIRPasses(codon::ir::transform::PassManager *pm, bool debug) {
  pm->registerPass(std::make_unique<LoopVec>(),
                   debug ? "core-pipeline-lowering"
                         : "core-pythonic-dict-arithmetic-opt");
  pm->registerPass(std::make_unique<TernaryVec>(),
                   debug ? "core-pipeline-lowering"
                         : "core-pythonic-dict-arithmetic-opt");
}

VectronFunctionTransformer::VectronFunctionTransformer()
    : util::Operator(), updatedVars() {}

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
      getVectronModuleName("lib"));
  assert(dpmatFunc);

  auto *dpmatCall = util::call(dpmatFunc, {outerRange, innerRange});
  assert(dpmatCall);

  x->setRhs(dpmatCall);
  x->getLhs()->setType(util::getReturnType(dpmatFunc));
  updatedVars.push_back(x->getLhs());
}

// Search expression tree for a identifier
class ComprehensionSearchVisitor : public ast::ReplacingCallbackASTVisitor {
  using Base = ReplacingCallbackASTVisitor;

  ast::Cache *cache;
  ast::ASTNode *modified;

  static int comprehensionCount;

public:
  ComprehensionSearchVisitor(ast::Cache *cache, int depth = 0,
                             bool replaceComprehension = true)
      : cache(cache), modified(nullptr) {}

  ast::Expr *transform(ast::Expr *expr) override {
    if (expr) {
      ComprehensionSearchVisitor v(cache);
      if (expr)
        expr->accept(v);
      if (v.modified)
        expr = cast<ast::Expr>(v.modified);
    }
    return expr;
  }

  ast::Stmt *transform(ast::Stmt *stmt) override {
    if (stmt) {
      ComprehensionSearchVisitor v(cache);
      if (stmt)
        stmt->accept(v);
      if (v.modified)
        stmt = cast<ast::Stmt>(v.modified);
    }
    return stmt;
  }

  void visit(ast::AssignStmt *stmt) override {
    using namespace codon::ast;
    if (match(stmt, M<AssignStmt>(M<IdExpr>(),
                                  M<GeneratorExpr>(GeneratorExpr::ListGenerator, M_),
                                  M_, M_))) {
      auto tmpFnName = cache->getTemporaryVar("vectron");
      modified = cache->N<SuiteStmt>(
          cache->N<FunctionStmt>(tmpFnName, nullptr, std::vector<Param>{},
                                 cache->N<ReturnStmt>(clone(stmt->getRhs()))),
          cache->N<AssignStmt>(
              clone(stmt->getLhs()),
              cache->N<CallExpr>(
                  cache->N<IdExpr>(
                      getMangledFunc(getVectronModuleName("lib"), "dpmat_wrap")),
                  std::vector<CallArg>{
                      CallArg{"", cache->N<IdExpr>(tmpFnName)},
                      CallArg{"", cache->N<IntExpr>(comprehensionCount++)},
                      CallArg{"LANE_SIZE", cache->N<IdExpr>(".VECTRON_LANE")},
                      CallArg{"T", cache->N<IdExpr>(".VECTRON_TYPE")},
                      CallArg{"THREADS", cache->N<IdExpr>(".VECTRON_THREADS")},
                  }),
              clone(stmt->getTypeExpr())));
    } else {
      Base::visit(stmt);
    }
  }
};

class OrSearchVisitor : public ast::ReplacingCallbackASTVisitor {
  using Base = ReplacingCallbackASTVisitor;

  ast::Cache *cache;
  ast::ASTNode *modified;
  bool replace;

  static int comprehensionCount;

public:
  OrSearchVisitor(ast::Cache *cache)
      : cache(cache), modified(nullptr), replace(false) {}

  ast::Expr *transform(ast::Expr *expr) override {
    if (expr) {
      OrSearchVisitor v(cache);
      v.replace = replace;
      if (expr)
        expr->accept(v);
      replace = false; // HACK: only handle first expression in IfStmt to avoid doing
                       // this for if/else nodes
      if (v.modified)
        expr = cast<ast::Expr>(v.modified);
    }
    return expr;
  }

  ast::Stmt *transform(ast::Stmt *stmt) override {
    if (stmt) {
      OrSearchVisitor v(cache);
      if (stmt)
        stmt->accept(v);
      if (v.modified)
        stmt = cast<ast::Stmt>(v.modified);
    }
    return stmt;
  }

  void visit(ast::IfExpr *expr) override {
    replace = true;
    Base::visit(expr);
  }

  void visit(ast::IfStmt *stmt) override {
    replace = true;
    Base::visit(stmt);
  }

  void visit(ast::BinaryExpr *expr) override {
    using namespace codon::ast;

    if (replace && expr->getOp() == "||") {
      modified = cache->N<CallExpr>(
          cache->N<IdExpr>(getMangledFunc(getVectronModuleName("lib"), "vec_or")),
          expr->getLhs(), expr->getRhs());
    } else if (replace && expr->getOp() == "&&") {
      modified = cache->N<CallExpr>(
          cache->N<IdExpr>(getMangledFunc(getVectronModuleName("lib"), "vec_and")),
          expr->getLhs(), expr->getRhs());
    } else {
      Base::visit(expr);
    }
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
    auto vectronAttr =
        codon::ast::getMangledFunc(getVectronModuleName("__init__"), "vectron");
    if (!bool(vectronFunc) || !util::hasAttribute(vectronFunc, vectronAttr))
      return;

    auto cache = v->getModule()->getCache();

    auto attr = vectronFunc->getAttribute<KeyValueAttribute>()->get(vectronAttr);
    auto dtype = cache->typeCtx->forceFind("int")->getType()->getClass();
    VectronMode mode = SIMD;
    auto lane_size = 8;
    auto threads = 1;
    if (!attr.empty()) {
      auto f = cache->typeCtx->forceFind(attr)->getType();
      auto tv = ast::TypecheckVisitor(cache->typeCtx);
      lane_size = tv.extractFuncGeneric(f, 0)->getIntStatic()->value;
      dtype = tv.extractFuncGeneric(f, 1)->getClass();
      threads = tv.extractFuncGeneric(f, 2)->getIntStatic()->value;
      mode = tv.extractFuncGeneric(f, 3)->getIntStatic()->value == 2 ? GPU : SIMD;
    }
    LOG("[vectron] fn: {}, lane: {}, dtype: {}, threads: {}, mode: {}",
        vectronFunc->getName(), lane_size, dtype->debugString(0), threads, (int)mode);

    seqassertn(dtype && in(cache->getClass(dtype)->realizations,
                           dtype->getClass()->realizedName()),
               "{} not realized", dtype ? dtype->debugString(0) : "<null>");
    std::vector<types::Generic> vectronParams;

    // 1. Clone the function, change its name (FN -> FN.vectron)
    auto fnAst = cast<ast::FunctionStmt>(
        clean_clone(vectronFunc->getType()->getAstType()->getFunc()->ast));

    auto initialization = cache->N<ast::SuiteStmt>();
    auto loops = cache->N<ast::SuiteStmt>();
    for (auto s : *(fnAst->getSuite())) {
      if (ast::cast<ast::ForStmt>(s)) {
        loops->addStmt(s);
      } else if (loops->empty()) {
        initialization->addStmt(s);
      } else {
        loops->addStmt(s);
      }
    }
    std::string argName = "";
    if (mode == GPU) {
      if (initialization->size() == 1) {
        ast::IdExpr *i = nullptr;
        if (match(initialization->front(),
                  matcher::M<ast::AssignStmt>(MVar<ast::IdExpr>(i), M_, M_, M_))) {
          argName = i->getValue();
        }
      }
      if (argName.empty())
        seqassertn(false, "Cannot extract GPU initialization block from function");
    }

    // 2. Modify the function (wrap the comprehension into:
    //    def FN(): return COMPREHENSION; M = WRAPPER(FN)
    if (mode == SIMD) {
      initialization = cast<ast::SuiteStmt>(
          ComprehensionSearchVisitor(cache).transform(initialization));
      loops = cast<ast::SuiteStmt>(OrSearchVisitor(cache).transform(loops));
    }

    auto origFnName = fnAst->getName();
    auto fnName = fmt::format("{}.vectron", cache->rev(origFnName));
    fnAst->setName(fnName);
    if (mode == GPU) {
      fnAst->addParam(ast::Param{argName, nullptr});
      fnAst->addParam(ast::Param{
          ".VECTRON_INIT", cache->N<ast::IndexExpr>(cache->N<ast::IdExpr>("Literal"),
                                                    cache->N<ast::IdExpr>("int"))});
      vectronParams.push_back(types::Generic{int64_t(0)});
    }
    if (mode == SIMD) {
      fnAst->addParam(ast::Param{
          ".VECTRON_LANE", cache->N<ast::IndexExpr>(cache->N<ast::IdExpr>("Literal"),
                                                    cache->N<ast::IdExpr>("int"))});
      vectronParams.push_back(types::Generic{lane_size});
    }
    fnAst->addParam(ast::Param{".VECTRON_TYPE", cache->N<ast::IdExpr>("type")});
    vectronParams.push_back(types::Generic{
        cache->getClass(dtype)->realizations[dtype->getClass()->realizedName()]->ir});
    if (mode == SIMD) {
      fnAst->addParam(ast::Param{
          ".VECTRON_THREADS", cache->N<ast::IndexExpr>(cache->N<ast::IdExpr>("Literal"),
                                                       cache->N<ast::IdExpr>("int"))});
      vectronParams.push_back(types::Generic{threads});
    }
    fnAst->setSuite(cache->N<ast::SuiteStmt>(cache->N<ast::AssignStmt>(
        cache->N<ast::IdExpr>("max"), cache->N<ast::IdExpr>(ast::getMangledFunc(
                                          getVectronModuleName("lib"), "maximum")))));
    if (mode == SIMD) {
      fnAst->getSuite()->addStmt(initialization);
      fnAst->getSuite()->addStmt(loops);
    } else {
      fnAst->getSuite()->addStmt(cache->N<ast::IfStmt>(
          cache->N<ast::BinaryExpr>(cache->N<ast::IdExpr>(".VECTRON_INIT"),
                                    "==", cache->N<ast::IntExpr>(1)),
          cache->N<ast::SuiteStmt>(cache->N<ast::ReturnStmt>(
              ast::cast<ast::AssignStmt>(initialization->front())->getRhs())),
          loops));
    }

    // 3. Check & typecheck new function
    auto s = cache->N<ast::SuiteStmt>(fnAst);
    if (auto err = ast::ScopingVisitor::apply(cache, s))
      throw exc::ParserException(std::move(err));
    auto transAst = ast::TypecheckVisitor::apply(cache->typeCtx, fnAst);
    ast::TranslateVisitor(cache->codegenCtx).translateStmts(transAst);

    // 4. Realize new function
    std::vector<ir::types::Type *> newFnArgs;
    auto vecListType = util::getReturnType(
        M->getOrRealizeFunc("_get_vec_list", {}, vectronParams,
                            getVectronModuleName(mode == SIMD ? "lib" : "libgpu")));
    assert(vecListType && "_get_vec_list method not found");
    for (auto it = vectronFunc->arg_begin(); it != vectronFunc->arg_end(); ++it)
      newFnArgs.push_back(vecListType);
    if (mode == GPU) {
      auto arrListType = util::getReturnType(M->getOrRealizeFunc(
          "_get_array_list", {}, vectronParams, getVectronModuleName("libgpu")));
      assert(arrListType && "_get_array_list method not found in std.lib");
      newFnArgs.push_back(arrListType);
    }
    auto newFn = v->getModule()->getOrRealizeFunc(fnName, newFnArgs, vectronParams);
    seqassertn(newFn, "cannot realize vectron function");

    std::vector<Value *> args;
    std::vector<types::Type *> tps;
    for (auto it = vectronCall->begin(); it != vectronCall->end(); ++it) {
      auto *getitemCall = cast<CallInstr>(*it);
      assert(getitemCall && "Arguments in Vectron method in comprehension "
                            "detected but not accessed. "
                            "Did you forget to use __getitem__ (i.e. x[i])?");

      auto *getitem = util::getFunc(getitemCall->getCallee());
      assert(getitem && getitem->getUnmangledName() == Module::GETITEM_MAGIC_NAME &&
             "Arguments in Vectron method in comprehension "
             "detected but not accessed. "
             "Did you forget to use __getitem__ (i.e. x[i])?");

      auto *arg = getitemCall->front();
      args.push_back(arg);
      tps.push_back(arg->getType());
    }

    if (mode == GPU) {
      // Call fn(VECTRON_INIT=1). GpuAlloc argument is not needed and is replaced with
      // args[0].
      auto initParams = vectronParams;
      initParams[0] = types::Generic{int64_t(1)};
      newFnArgs[newFnArgs.size() - 1] = newFnArgs[0];
      auto initFn = v->getModule()->getOrRealizeFunc(fnName, newFnArgs, initParams);
      seqassertn(initFn, "cannot realize vectron initialization");

      args.push_back(args[0]);
      auto arg = util::call(initFn, args);
      args.pop_back();
      args.push_back(arg);
      tps.push_back(arg->getType());
    }

    tps.push_back(newFn->getType());
    auto *alpernFunc = M->getOrRealizeFunc("kernel", tps, vectronParams,
                                           mode == GPU ? getVectronModuleName("libgpu")
                                                       : getVectronModuleName("lib"));
    seqassertn(alpernFunc, "cannot realize vectron kernel");

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

codon::ir::Value *
TernaryVec::ternToCall(codon::ir::Value *v,
                       const std::vector<codon::ir::types::Generic> &ternaryGenerics) {
  auto ternVal = cast<TernaryInstr>(v);
  assert(ternVal);

  auto values = ternVal->getCond()->getUsedValues();
  if (!values.size())
    return v;

  auto *M = ternVal->getModule();
  auto *cond = values[0];
  auto *trueVal = ternVal->getTrueValue();
  auto *falseVal = ternVal->getFalseValue();

  auto generics = cond->getType()->getGenerics();
  if (generics.size() != 2)
    return v;

  auto *vecType =
      M->getOrRealizeType(codon::ast::getMangledClass("std.simd", "Vec"), generics);

  if (!vecType || !cond->getType()->is(vecType))
    return v;

  auto *ternTrueVal = cast<TernaryInstr>(trueVal);
  auto *ternFalseVal = cast<TernaryInstr>(falseVal);
  if (ternTrueVal)
    trueVal = ternToCall(ternTrueVal, ternaryGenerics);
  if (ternFalseVal)
    falseVal = ternToCall(ternFalseVal, ternaryGenerics);

  auto *ternaryHelper = M->getOrRealizeFunc(
      "ternary", {cond->getType(), trueVal->getType(), falseVal->getType()},
      ternaryGenerics, getVectronModuleName("lib"));
  assert(ternaryHelper);

  auto *ternaryCall = util::call(ternaryHelper, {cond, trueVal, falseVal});
  assert(ternaryCall);

  return ternaryCall;
}

void TernaryVec::handle(CallInstr *v) {
  auto *M = v->getModule();
  if (v->getType()->is(M->getNoneType()))
    return;

  auto *pf = getParentFunc();
  auto vectronAttr =
      codon::ast::getMangledFunc(getVectronModuleName("__init__"), "vectron");
  if (!bool(pf) || !util::hasAttribute(pf, vectronAttr))
    return;
  auto attr = pf->getAttribute<KeyValueAttribute>()->get(vectronAttr);
  auto cache = v->getModule()->getCache();
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
  seqassertn(dtype && in(cache->getClass(dtype)->realizations,
                         dtype->getClass()->realizedName()),
             "{} not realized", dtype ? dtype->debugString(0) : "<null>");
  auto vectronParams = std::vector<types::Generic>{
      types::Generic{lane_size},
      types::Generic{
          cache->getClass(dtype)->realizations[dtype->getClass()->realizedName()]->ir}};

  std::vector<Value *> args;
  std::vector<types::Type *> argsTypes;
  bool ternFound = false;
  for (auto it = v->begin(); it != v->end(); ++it) {
    bool check = (*it)->is<TernaryInstr>();
    ternFound |= check;
    auto arg = check ? ternToCall(*it, vectronParams) : *it;
    args.push_back(arg);
    argsTypes.push_back(arg->getType());
  }

  if (!ternFound)
    return;

  auto *newCallerHelper = M->getOrRealizeMethod(
      v->getType(), util::getFunc(v->getCallee())->getUnmangledName(), argsTypes);
  assert(newCallerHelper);

  auto *newCaller = util::call(newCallerHelper, args);
  assert(newCaller);

  v->replaceAll(newCaller);
}

} // namespace vectron

extern "C" std::unique_ptr<codon::DSL> load() {
  return std::make_unique<vectron::Vectron>();
}
