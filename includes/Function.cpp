#include "Function.h"

INST* declareFunction(std::string funcName, std::string blockName, std::vector<std::string> params){
    InstBlock* funcBlock =  newInstBlock("Function " + funcName, currInstNum++);
    FunctionList[funcName].first = funcBlock;
    funcBlock->name = blockName;

    Instruction* inst;
    for(std::string s : params){
        inst = newInstruction();
        inst->InstNum = currInstNum;
        insertVT(s, inst);
        FunctionList[funcName].second.push_back(currInstNum++);
    }
    return funcBlock->head;
}

std::pair<InstBlock*, std::vector<int> > getFunctionParam(std::string funcName){
    return FunctionList[funcName];
}



void InitFunction(){
    FunctionList.clear();
}

