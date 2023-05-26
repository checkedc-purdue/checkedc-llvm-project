#include "llvm/IR/BingeIRMetadata.h"
#include "llvm/IR/Function.h" // Add this at the top of the file
#include <iostream>
#include <sstream> // Add this at the top of the file

using namespace llvm;
std::map<std::string, std::map<Value*, std::set<Value*>>> BingeIRMetadata::BingeIRInfo;
std::map<std::string, std::map<Value*, std::string>> BingeIRMetadata::BingeIRSrcInfo;

std::string BingeIRMetadata::decodeFileName(const std::string &key) {
  std::istringstream iss(key);
  std::string segment;
  std::vector<std::string> seglist;

  while(std::getline(iss, segment, '@')) {
    seglist.push_back(segment);
  }

  if(seglist.size() == 3 && seglist[0] == "ConditionCollector") {
    return seglist[1];
  } else {
    // Handle error
    std::cerr << "Invalid key format\n";
    return "";
  }
}

std::string BingeIRMetadata::decodeFuncName(const std::string &key) {
  std::istringstream iss(key);
  std::string segment;
  std::vector<std::string> seglist;

  while(std::getline(iss, segment, '@')) {
    seglist.push_back(segment);
  }

  if(seglist.size() == 3 && seglist[0] == "ConditionCollector") {
    return seglist[2];
  } else {
    // Handle error
    std::cerr << "Invalid key format\n";
    return "";
  }
}

MDNode* BingeIRMetadata::GenBingeMd(Function *F) {
  std::string fileName = decodeFileName(F->getName().str());
  std::string funcName = decodeFuncName(F->getName().str());

  // Look up the function in the BingeIRSrcInfo map.
  auto iter = BingeIRSrcInfo.find("ConditionCollector@" + fileName + "@" + funcName);
  if (iter == BingeIRSrcInfo.end()) {
    // If the function isn't found in the map, return null.
    return nullptr;
  }

  // Fetch the map of Values to string.
  std::map<Value*, std::string> &valueMap = iter->second;

  for (auto &BB : *F) {  // For each basic block in the function
    for (auto &I : BB) {  // For each instruction in the basic block
      if (valueMap.find(&I) != valueMap.end()) {
        // Get the string representing the location.
        std::string locStr = valueMap[&I];

        // Assuming locStr is in the format "filename:line:col".
        std::istringstream iss(locStr);
        std::string lineStr;
        std::getline(iss, lineStr, ':');
        unsigned line = std::stoi(lineStr);

        // Create a metadata string for this instruction
        MDString *MD = MDString::get(F->getContext(), funcName + ":" + std::to_string(line));
        I.setMetadata("dbg", MDNode::get(F->getContext(), MD));  // Set the debug info
      }
    }
  }

  // Return the last used metadata
  MDString *MD = MDString::get(F->getContext(), funcName);
  return MDNode::get(F->getContext(), MD);
}
