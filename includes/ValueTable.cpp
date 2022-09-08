#include "ValueTable.h"


void declareVar(std::string ident){
    insertVT(ident, -3);
}


void insertVT(std::string ident, int inst){
    int idx = ValueTable.size()-1; // both table must have same depth;
    if(InWhile){
        // ============== Update all instructions ================
        // 
        // addLabelInst("ADDED HERE========");

        // update phi function;
        std::pair<int, int> vt = getVT(ident);
        if(vt.first >= 0){
            WhileJoin = addPhiInst(WhileJoin, newOp(ident, vt.first), newOp(ident, inst));
            WhileJoin->InstNum = currInstNum;
            // Update Value Table
            InWhile = false;
            insertVT(ident, currInstNum++);
            InWhile = true;
        }
        else{
            struct Instruction* instruction = newInstruction();
            instruction->op = STORECONST;
            instruction->InstNum = currInstNum;
            instruction->a = newOp(ident, vt.second);
            WhileJoin->next = (INST*) instruction;
            WhileJoin = instruction;
            
            // Update Value Table
            WhileJoin = addPhiInst(WhileJoin, newOp(ident, currInstNum++), newOp(ident, inst));
            InWhile = false;
            insertVT(ident, currInstNum++);
            InWhile = true;

        }
        return;
    }
    ValueTable[idx][ident] = inst;
    if(ConstVal[idx].find(ident) != ConstVal[idx].end()){
        ConstVal[idx].erase(ident);
    }
}

std::pair<int, int> getVT(std::string ident){
    std::pair<int, int> result;
    int lastIdx = ValueTable.size()-1;
    for(int i = lastIdx; i >= 0; i--){
        if(ValueTable[i].find(ident) != ValueTable[i].end()){
            result.first = ValueTable[i][ident];
            return result;
        }
    }
    for(int i = lastIdx; i >= 0; i--){
        if(ConstVal[i].find(ident) != ConstVal[i].end()){
            result.first = -1;
            result.second = ConstVal[i][ident];
            return result;
        }
    }
    // value not exist
    result.first = -2;
    return result;
}

std::pair<int, int> getPrevVT(std::string ident){
    std::pair<int, int> result;
    int lastIdx = ValueTable.size()-2;
    for(int i = lastIdx; i >= 0; i--){
        if(ValueTable[i].find(ident) != ValueTable[i].end()){
            result.first = ValueTable[i][ident];
            return result;
        }
    }
    for(int i = lastIdx; i >= 0; i--){
        if(ConstVal[i].find(ident) != ConstVal[i].end()){
            result.first = -1;
            result.second = ConstVal[i][ident];
            return result;
        }
    }
    // value not exist
    result.first = -2;
    return result;
}


void insertCV(std::string ident, int inst){
    int idx = ValueTable.size()-1; // both table must have same depth;
    if(InWhile){
        struct Instruction* instruction = newInstruction();
        instruction->op = STORECONST;
        instruction->InstNum = currInstNum;
        instruction->a = newOp(ident, inst);
        WhileJoin->next = (INST*) instruction;
        WhileJoin = instruction;

        Opr* nOp = newOp(ident, currInstNum++);

        std::pair<int, int> vt = getVT(ident);
        if(vt.first >= 0){
            WhileJoin = addPhiInst(WhileJoin, newOp(ident, vt.first), nOp);
            WhileJoin->InstNum = currInstNum;
            // Update Value Table
            InWhile = false;
            insertVT(ident, currInstNum++);
            InWhile = true;
        }
        else{
            instruction = newInstruction();
            instruction->op = STORECONST;
            instruction->InstNum = currInstNum;
            instruction->a = newOp(ident, vt.second);
            WhileJoin->next = (INST*) instruction;
            WhileJoin = instruction;
            
            // Update Value Table
            WhileJoin = addPhiInst(WhileJoin, newOp(ident, currInstNum++), nOp);
            WhileJoin->InstNum = currInstNum;
            InWhile = false;
            insertVT(ident, currInstNum++);
            InWhile = true;

        }
        return;
    }
    ConstVal[idx][ident] = inst;
    if(ValueTable[idx].find(ident) != ValueTable[idx].end()){
        ValueTable[idx].erase(ident);
    }
}

void InsertVTLayer(){
    ValueTable.push_back(std::unordered_map<std::string, int>());
    ConstVal.push_back(std::unordered_map<std::string, int>());
}
void RemoveVTLayer(){
    // int idx = ValueTable.size()-1;
    // ValueTable.erase(ValueTable.begin()+idx);
    // ConstVal.erase(ConstVal.begin()+idx);
    ValueTable.pop_back();
    ConstVal.pop_back();
}
void ClearLastLayer(){
    int idx = ValueTable.size()-1;
    ValueTable[idx].clear();
    ConstVal[idx].clear();
}

void InitVT(){
    ValueTable.clear();
    ConstVal.clear();
    InsertVTLayer();
    currInstNum = 0;
    InWhile = false;
    WhileCondition = NULL;
    WhileDo = NULL;
    WhileJoin = NULL;
}
