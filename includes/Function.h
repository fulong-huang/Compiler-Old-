#include <iostream>

// function block head is type ---NLL---
//  function return instruction on b
Instruction* funcHeadInst;

// Name of function : list of int for instruction numbers
//      During function declare, instNum += num_of_params,
//      thus reserved space to move instruction into those number
std::unordered_map<std::string, std::pair<InstBlock*, std::vector<int> > > FunctionList;



// Declare new function; 
// Function needs:
//      new block 
//      new value table 
//      list of param instruction
//      
// Need to keep track of returned instruction number
//      Save it to block->head->(op b)
INST* declareFunction(std::string funcName, std::string blockName, std::vector<std::string> params);
std::pair<InstBlock*, std::vector<int> > getFunctionParam(std::string funcName);

void InitFunction();



