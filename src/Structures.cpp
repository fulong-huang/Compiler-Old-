#include "Structures.h"
struct Instruction* newInstInt(int n){
    InstInt* inst = new struct InstInt();
    inst->TYPE = INT;
    inst->num = n;
    return (Instruction*) inst;
}
struct Opr* newOp(std::string name, Instruction* inst){
    struct Opr* result = new struct Opr();
    result->name = name;
    result->inst = inst;
    return result;
}

struct Instruction* newInstruction(){
    Instruction* inst = new struct Instruction();
    inst->TYPE = SINGLE;
    inst->next = NULL;
    inst->InstNum = -1;
    inst->op = NLL;
    return inst;
}

struct InstBlock* newInstBlock(std::string blockName, int n){
    InstBlock* inst = new struct InstBlock();
    inst->TYPE = BLOCK;
    inst->next = NULL;
    inst->next2 = NULL;
    inst->head = (INST*) newInstruction();
    ((Instruction*)inst->head)->InstNum = n;
    ((Instruction*)inst->head)->a = newOp(blockName, newInstInt(-1));
    inst->name = blockName;
    return inst;
}


struct LinkedInst* newLinkedInst(){
    LinkedInst* result = new struct LinkedInst();
    result->inst = newInstruction();
    result->inst->op = COMMENT;
    result->inst->a = newOp("new LinkedInst", newInstInt(-1));
    return result;
}

INST::~INST(){
    if(TYPE == INT) { 
        delete (InstInt*)(this);
    }
    else if(TYPE == SINGLE){
        delete (Instruction*)(this);
    }
    else{
        delete (InstBlock*)(this);
    }

    // delete next
    if(next == NULL) return;
    delete next;
}

Instruction::~Instruction(){
    if(TYPE == SINGLE){
        if(a != NULL) delete a;
        if(b != NULL) delete b;
    }
    else if(TYPE == INT) { 
        delete (InstInt*)(this);
    }
    else{
        delete (InstBlock*)(next);
    }

    // delete next
    if(next != NULL) delete next;

}

InstBlock::~InstBlock(){
    if(TYPE == BLOCK) { 
        if(next2 != NULL) delete next2;
        if(head != NULL) delete head;
    }
    else if(TYPE == SINGLE){
        delete (Instruction*)(this);
    }
    else{
        delete (InstInt*)(next);
    }

    // delete next
    if(next != NULL) delete next;

}

LinkedInst::~LinkedInst(){
    if(inst != NULL) delete inst;
    if(next != NULL) delete next;
    if(next2 != NULL) delete next2;
}

Opr::~Opr(){
    if(inst != NULL) delete inst;
}

