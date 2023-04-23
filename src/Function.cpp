#include "Function.h"

// function block head is type ---NLL---
//  function return instruction on b
Instruction* funcHeadInst;

// Name of function : list of int for instruction numbers
//      During function declare, instNum += num_of_params,
//      thus reserved space to move instruction into those number
std::unordered_map<std::string, std::pair<InstBlock*, std::vector<Instruction*> > > FunctionList;



// ********** Functions starts here: *************//
Instruction* getFuncHeadInst(){ return funcHeadInst; }
void setFuncHeadInst(Instruction* newFuncHeadInst){ funcHeadInst = newFuncHeadInst;}

std::unordered_map<std::string, std::pair<InstBlock*, std::vector<Instruction*> > > getFunList(){
    return FunctionList;
};

std::pair<INST*, int> declareFunction(std::string funcName, std::string blockName, std::vector<std::string> params){
    InstBlock* funcBlock =  newInstBlock("Function " + funcName, incCurrInstNum());
    FunctionList[funcName].first = funcBlock;
    funcBlock->name = blockName;

    int returnNum = getCurrInstNum();

    Instruction* inst;

        inst = newInstruction();
        inst->InstNum = incCurrInstNum();
        FunctionList[funcName].second.push_back(inst);

    for(std::string s : params){
        inst = newInstruction();
        inst->InstNum = incCurrInstNum();
        insertVT(s, inst);
        FunctionList[funcName].second.push_back(inst);
    }
    return std::pair<INST*, int>(funcBlock->head, returnNum);
}

std::pair<InstBlock*, std::vector<Instruction*> > getFunctionParam(std::string funcName){
    return FunctionList[funcName];
}



void InitFunction(){
    funcHeadInst = NULL;
    FunctionList.clear();
}
void DestroyFunction(){
    for(auto kv : FunctionList){
        // std::cout << kv.second.first->name << std::endl;
        // std::cin.ignore();
        if(kv.second.first != nullptr){
            delete (InstBlock*) kv.second.first;
        }
        for(int i = 0; i < kv.second.second.size(); i++){
            delete kv.second.second[i];
        }
    }
    // if(funcHeadInst != NULL)
    //     delete funcHeadInst;
}

