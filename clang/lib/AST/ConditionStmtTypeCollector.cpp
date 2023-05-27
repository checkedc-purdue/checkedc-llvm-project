#include "clang/AST/ConditionStmtTypeCollector.h"
#include "clang/Basic/SourceManager.h"

using namespace clang;

bool ConditionStmtTypeCollector::VisitFunctionDecl(FunctionDecl *FD) {
  currentFD = FD;
  // Continue traversal
  return true;
}


bool ConditionStmtTypeCollector::VisitStmt(Stmt *s) {
  if (currentFD == NULL)
    return true;

  std::string funcName = currentFD->getNameInfo().getName().getAsString();

  // Obtain filename (module name)
  auto &SM = currentFD->getASTContext().getSourceManager();
  std::string fileName = SM.getFilename(currentFD->getBeginLoc()).str();

  std::string conditionCollectorKey = "ConditionCollector@" + fileName + "@" + funcName;

  std::string stmtType;
  if (isa<IfStmt>(s)) {
    stmtType = "If";
  } else if (isa<SwitchStmt>(s)) {
    stmtType = "Switch";
  } else if (isa<GotoStmt>(s)) {
    stmtType = "Goto";
  } else if (isa<ConditionalOperator>(s)) {
    stmtType = "?:";
  }

  if (!stmtType.empty()) {
    Collector.addNodeInfo(conditionCollectorKey, s, stmtType);
  }

  return true;
}
