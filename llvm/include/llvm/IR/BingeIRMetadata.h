#ifndef BINBENCH_LLVM_BINGEIRMETADATA_H
#define BINBENCH_LLVM_BINGEIRMETADATA_H

#include "llvm/IR/DebugInfo.h"
#include <map>
#include <set>
#include <string>
#include <unordered_map>

namespace llvm {

class BingeIRMetadata{
public:
  static std::map<std::string, std::map<Value*, std::set<Value*>>> BingeIRInfo;
  static std::map<std::string, std::map<Value*, std::string>> BingeIRSrcInfo;

  struct StmtInfo {
    std::string collectorKey; // collectorType@FileName@FunctionName
    std::string stmtStr;  // corresponding statement string
  };

  static MDNode* GenBingeMd(Function *F);
  static std::string decodeFileName(const std::string &key);
  static std::string decodeFuncName(const std::string &key);
};

} // end namespace llvm

#endif // BINBENCH_LLVM_BINGEIRMETADATA_H
