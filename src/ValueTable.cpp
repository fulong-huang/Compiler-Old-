#include "ValueTable.h"

/**
 * Save the instruction number of the value givin name stored.
 * ValueTable[ VarName ] = InstNumber of newest version 
 */

// ValueTable:      <ident, instNumber>
//  var x, y; (inst 25) x = Call func(); (inst 26) y = Call func();
// x and y will be stored in ValueTable, ValueTable = {[x:25], [y:26]}
//  var z; z = x+y;                      ValueTable = {[x:25], [y:26], [z:27]}      1 instruction created
std::vector<std::unordered_map<std::string, Instruction*> > ValueTable;

// ConstVal:        <ident, storedValue>
//  var x, y; x = 5; y = 3;
// x and y will be stored in ConstVal, ConstVal = {[x:5], [y:3]}
//  var z; z = x+y;                    ConstVal = {[x:5], [y:3], [z:8]}             0 instruction created
// std::vector<std::unordered_map<std::string, int> > ConstVal; // store instNum of constant;


std::unordered_map<int, Instruction*> ConstTable;

int currConstNum;

bool InWhile, InIf;
struct Instruction* WhileJoin;

// *********** FUNCITONS ***********//

struct Instruction* getWhileJoin(){return WhileJoin;}
void setWhileJoin(struct Instruction* newWhileJoin){WhileJoin = newWhileJoin;}

void setValueTable(std::vector<std::unordered_map<std::string, Instruction*> > newVT){
    ValueTable = newVT;
}
std::vector<std::unordered_map<std::string, Instruction*> > getValueTable(){
    return ValueTable;
}

bool getInWhile(){ return InWhile; }
bool getInIf(){ return InIf;}
void setInWhile(bool b){InWhile = b;}
void setInIf(bool b){InIf = b;}


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
        struct std::shared_ptr<Opr> a;
        struct std::shared_ptr<Opr> b = newOp(ident, inst);

        // ============== Update all instructions ================
        // update phi function;
        Instruction* vt = getVT(ident);
        // if it's a instruction, shoudl be ALWAYS TRUE;
        //      Everything inserted should be variable
        // if(vt.first){ 
        Instruction* ist = (Instruction*)((InstBlock*) getJoinBlock())->head;
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
            WhileJoin->InstNum = getCurrInstNum();
            // Update Value Table
            // InWhile = false;
            // insertVT(ident, WhileJoin);
            updateInst(a, WhileJoin);
            updateInst(b, WhileJoin);
            // updateInst(a, currInstNum);
            // updateInst(b, currInstNum);
            incCurrInstNum();
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
    if(ValueTable[idx].find(ident) != ValueTable[idx].end() && ValueTable[idx][ident]->TYPE == INT){
        delete (InstInt*) ValueTable[idx][ident];
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
    if(inst->TYPE == INT){
        insertCT(ident, ((InstInt*)inst)->num);
    }
    else{
        insertVT(ident, inst);
    }
}

// save constant into ConstTable
Instruction* createConst(int val){

    if(ConstTable.find(val) != ConstTable.end()){
        return newInstInt(((InstInt*)ConstTable[val])->num);
    }

    struct Instruction* intInst = newInstInt(currConstNum);
    ConstTable[val] = intInst;
    Instruction* inst = newInstruction();
    inst->InstNum = currConstNum;
    inst->a = newOp("#", newInstInt(val));
    inst->op = CONST;
    inst->next = getInstHead();
    setInstHead((INST*) inst);
    currConstNum--;


    return newInstInt(((InstInt*)ConstTable[val])->num);
}


Instruction* getVT(std::string ident){
    int lastIdx = ValueTable.size()-1;
    // find variable
    for(int i = lastIdx; i >= 0; i--){
        if(ValueTable[i].find(ident) != ValueTable[i].end()){
            if(ValueTable[i][ident]->TYPE == INT){
                return newInstInt(((InstInt*)ValueTable[i][ident])->num);
            }
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
            if(ValueTable[i][ident]->TYPE == INT){
                return newInstInt(((InstInt*)ValueTable[i][ident])->num);
            }
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
    int idx = ValueTable.size()-1;
    for(auto kv : ValueTable[idx]){
        if(kv.second->TYPE == INT){
            delete (InstInt*) kv.second;
        }
    }
    // ValueTable.erase(ValueTable.begin()+idx);
    // ConstVal.erase(ConstVal.begin()+idx);
    ValueTable.pop_back();
    // ConstVal.pop_back();
}
void ClearLastLayer(){
    int idx = ValueTable.size()-1;
    for(auto kv : ValueTable[idx]){
        if(kv.second->TYPE == INT){
            delete (InstInt*) kv.second;
        }
    }
    ValueTable[idx].clear();
    // ConstVal[idx].clear();
}

void InitVT(){
    ValueTable.clear();
    // ConstVal.clear();
    ConstTable.clear();
    InsertVTLayer();
    InWhile = false;
    InitializeInstruction();
    WhileJoin = NULL;
    currConstNum = 1;

    Instruction* i;
    i = createConst(1);
    delete (InstInt*) i;
    i = createConst(0);
    delete (InstInt*) i;
    currConstNum = -3;
    
}

void DestroyVT(){
    std::cout << "DESTROY VT 1" << std::endl;
    DeleteCurrVT();
    // for(int i = 0; i < ValueTable.size(); i++){
    //     for(auto kv : ValueTable[i]){
    //         if(kv.second != nullptr){
    //             if(kv.second->TYPE == INT){
    //                 delete (InstInt*) kv.second;
    //             }
    //             // else
    //             //     delete kv.second;
    //         }
    //     }
    // }
    std::cout << "DESTROY VT 2" << std::endl;
    for(auto kv : ConstTable){
        if(kv.second != nullptr){
            if(kv.second->TYPE == INT){ // should aywas be true
                delete (InstInt*) kv.second;
            }
        }
    }
    // if(WhileJoin != NULL) delete WhileJoin;
}

void DeleteCurrVT(){
    for(int i = 0; i < ValueTable.size(); i++){
        for(auto kv : ValueTable[i]){
            if(kv.second != nullptr){
                if(kv.second->TYPE == INT){
                    delete (InstInt*) kv.second;
                }
                // else
                //     delete kv.second;
            }
        }
    }
}


