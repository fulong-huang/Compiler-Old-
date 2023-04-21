#pragma once
#include <iostream>
#include "Structures.h"
#include "Instruction.h"
#include "ValueTable.h"



// Declare new function; 
// Function needs:
//      new block 
//      new value table 
//      list of param instruction
//      
// Need to keep track of returned instruction number
//      Save it to block->head->(op b)
std::unordered_map<std::string, std::pair<InstBlock*, std::vector<int> > > getFunList();
std::pair<INST*, int> declareFunction(std::string funcName, std::string blockName, std::vector<std::string> params);
std::pair<InstBlock*, std::vector<int> > getFunctionParam(std::string funcName);

void InitFunction();
void DestroyFunction();
Instruction* getFuncHeadInst();
void setFuncHeadInst(Instruction* newFuncHeadInst);


