#include "ValueTable.h"


void declareVar(std::string ident){
    struct Instruction* intInst = newInstInt(-1);
    insertVT(ident, intInst);
}

void insertVT(std::string ident, Instruction* inst){
    int idx = ValueTable.size()-1; // both table must have same depth;
    if(InWhile){
        // During while need to update phi fuction whenever variable is used.
        //  However, it will not be done during while,
        //  it need to update with phi function of "if loop"
        struct Opr* a;
        struct Opr* b = newOp(ident, inst);

        // ============== Update all instructions ================
        // update phi function;
        Instruction* vt = getVT(ident);
        // if it's a instruction, shoudl be ALWAYS TRUE;
        //      Everything inserted should be variable
        // if(vt.first){ 
        Instruction* ist = (Instruction*)((InstBlock*) JoinBlock)->head;
        bool notFound = true;
        while(ist != NULL){
            if(ist->op == PHI && ist->a->name.compare(ident) == 0){
                ist->b->inst = inst;
                notFound = false;
            }
        
            ist = (Instruction*)ist->next;
        }
        if(notFound){
            a = newOp(ident, vt);
            WhileJoin = addPhiInst(WhileJoin, a, b);
            WhileJoin->InstNum = currInstNum;
            // Update Value Table
            // InWhile = false;
            // insertVT(ident, WhileJoin);
            updateInst(a, WhileJoin);
            updateInst(b, WhileJoin);
            // updateInst(a, currInstNum);
            // updateInst(b, currInstNum);
            currInstNum++;
            // InWhile = true;
        }
        // }
        // else{ // if pre exist value was a constant
        //     struct Instruction* instruction = newInstruction();
        //     instruction->op = STORECONST;
        //     instruction->InstNum = currInstNum;
        //     a = newOp("#"+ident, vt.second);
        //     instruction->a = a;
        //     WhileJoin->next = (INST*) instruction;
        //     WhileJoin = instruction;
        //     // Update Value Table
        //     WhileJoin = addPhiInst(WhileJoin, newOp(ident, currInstNum++), b);
        //     // if it were constant, also need to update constant;
        //     updateInst(a, currInstNum);
        //     updateInst(b, currInstNum);
        //     WhileJoin->InstNum = currInstNum;
        //     InWhile = false;
        //     insertVT(ident, currInstNum++);
        //     InWhile = true;

        // }
        // return;
    }
    ValueTable[idx][ident] = inst;
    // if(ConstVal[idx].find(ident) != ConstVal[idx].end()){
    //     ConstVal[idx].erase(ident);
    // }
}

void insertCT(std::string ident, int val){
    if(ConstTable.find(val) != ConstTable.end()){
        insertVT(ident, ConstTable[val]);
    }
    else{
        insertVT(ident, createConst(val));
    }
}
void insertCT(std::string ident, Instruction* inst){
    // createConst(((InstInt*)inst)->num);
    insertVT(ident, inst);
}

// save constant into ConstTable
Instruction* createConst(int val){
    if(ConstTable.find(val) != ConstTable.end()) return ConstTable[val];

    struct Instruction* intInst = newInstInt(currConstNum);
    ConstTable[val] = intInst;
    Instruction* inst = newInstruction();
    inst->InstNum = currConstNum;
    inst->a = newOp("#", newInstInt(val));
    inst->op = CONST;
    inst->next = InstHead;
    InstHead = (INST*) inst;
    currConstNum--;
    return intInst;
}


Instruction* getVT(std::string ident){
    int lastIdx = ValueTable.size()-1;
    // find variable
    for(int i = lastIdx; i >= 0; i--){
        if(ValueTable[i].find(ident) != ValueTable[i].end()){
            return ValueTable[i][ident];
        }
    }
    // for(int i = lastIdx; i >= 0; i--){
    //     if(ConstVal[i].find(ident) != ConstVal[i].end()){
    //         result.first = -1;
    //         result.second = ConstVal[i][ident];
    //         return result;
    //     }
    // }
    // value not exist
    
    // variable not found
    return newInstInt(-2);
}

Instruction* getPrevVT(std::string ident){
    std::pair<bool, Instruction*> result;
    int lastIdx = ValueTable.size()-2;
    for(int i = lastIdx; i >= 0; i--){
        if(ValueTable[i].find(ident) != ValueTable[i].end()){
            return ValueTable[i][ident];
        }
    }
    // for(int i = lastIdx; i >= 0; i--){
    //     if(ConstVal[i].find(ident) != ConstVal[i].end()){
    //         result.first = -1;
    //         result.second = ConstVal[i][ident];
    //         return result;
    //     }
    // }
    // value not exist
    return newInstInt(-2);
}

Instruction* getCT(int instNum){
    for(auto &it:ConstTable){
        if(((InstInt*) it.second)->num == instNum){
            return newInstInt(it.first);
        }
    }
    return newInstInt(-101);
}


// void insertCV(std::string ident, int inst){
//     int idx = ValueTable.size()-1; // both table must have same depth;
//     if(InWhile){
//         struct Instruction* instruction = newInstruction();
//         instruction->op = STORECONST;
//         instruction->InstNum = currInstNum;
//         instruction->a = newOp("#"+ident, inst);
//         WhileJoin->next = (INST*) instruction;
//         WhileJoin = instruction;

//         Opr* nOp = newOp(ident, currInstNum++);

//         std::pair<int, int> vt = getVT(ident);
//         if(vt.first >= 0){
//             WhileJoin = addPhiInst(WhileJoin, newOp(ident, vt.first), nOp);
//             WhileJoin->InstNum = currInstNum;
//             // Update Value Table
//             InWhile = false;
//             insertVT(ident, currInstNum++);
//             InWhile = true;
//         }
//         else{
//             instruction = newInstruction();
//             instruction->op = STORECONST;
//             instruction->InstNum = currInstNum;
//             instruction->a = newOp("#"+ident, vt.second);
//             WhileJoin->next = (INST*) instruction;
//             WhileJoin = instruction;
            
//             // Update Value Table
//             WhileJoin = addPhiInst(WhileJoin, newOp(ident, currInstNum++), nOp);
//             WhileJoin->InstNum = currInstNum;
//             InWhile = false;
//             insertVT(ident, currInstNum++);
//             InWhile = true;

//         }
//         return;
//     }
//     ConstVal[idx][ident] = inst;
//     if(ValueTable[idx].find(ident) != ValueTable[idx].end()){
//         ValueTable[idx].erase(ident);
//     }
// }

void InsertVTLayer(){
    ValueTable.push_back(std::unordered_map<std::string, Instruction*>());
    // ConstVal.push_back(std::unordered_map<std::string, int>());
}
void RemoveVTLayer(){
    // int idx = ValueTable.size()-1;
    // ValueTable.erase(ValueTable.begin()+idx);
    // ConstVal.erase(ConstVal.begin()+idx);
    ValueTable.pop_back();
    // ConstVal.pop_back();
}
void ClearLastLayer(){
    int idx = ValueTable.size()-1;
    ValueTable[idx].clear();
    // ConstVal[idx].clear();
}

void InitVT(){
    ValueTable.clear();
    // ConstVal.clear();
    ConstTable.clear();
    InsertVTLayer();
    InWhile = false;
    WhileCondition = NULL;
    WhileDo = NULL;
    WhileJoin = NULL;
    currConstNum = 1;
    createConst(1);
    createConst(0);
    currConstNum = -3;
    
}
