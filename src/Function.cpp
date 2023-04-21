#include "Function.h"

// function block head is type ---NLL---
//  function return instruction on b
Instruction* funcHeadInst;

// Name of function : list of int for instruction numbers
//      During function declare, instNum += num_of_params,
//      thus reserved space to move instruction into those number
std::unordered_map<std::string, std::pair<InstBlock*, std::vector<int> > > FunctionList;



// ********** Functions starts here: *************//
Instruction* getFuncHeadInst(){ return funcHeadInst; }
void setFuncHeadInst(Instruction* newFuncHeadInst){ funcHeadInst = newFuncHeadInst;}

std::unordered_map<std::string, std::pair<InstBlock*, std::vector<int> > > getFunList(){
    return FunctionList;
};

std::pair<INST*, int> declareFunction(std::string funcName, std::string blockName, std::vector<std::string> params){
    InstBlock* funcBlock =  newInstBlock("Function " + funcName, incCurrInstNum());
    FunctionList[funcName].first = funcBlock;
    funcBlock->name = blockName;

    int returnNum = getCurrInstNum();

    Instruction* inst;

        inst = newInstruction();
        inst->InstNum = getCurrInstNum();
        FunctionList[funcName].second.push_back(incCurrInstNum());

    for(std::string s : params){
        inst = newInstruction();
        inst->InstNum = getCurrInstNum();
        insertVT(s, inst);
        FunctionList[funcName].second.push_back(incCurrInstNum());
    }
    return std::pair<INST*, int>(funcBlock->head, returnNum);
}

std::pair<InstBlock*, std::vector<int> > getFunctionParam(std::string funcName){
    return FunctionList[funcName];
}



void InitFunction(){
    FunctionList.clear();
}
void DestroyFunction(){
    for(auto kv : FunctionList){
        if(kv.second.first != NULL){
            delete kv.second.first;
        }
    }
}

