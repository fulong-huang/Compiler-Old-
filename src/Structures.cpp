#include "Structures.h"
struct Instruction* newInstInt(int n){
    InstInt* inst = new struct InstInt();
    inst->TYPE = INT;
    inst->num = n;
    return (Instruction*) inst;
}
std::shared_ptr<Opr> newOp(std::string name, Instruction* inst){
    std::shared_ptr<Opr> result = std::make_shared<Opr>();
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
    result->inst->TYPE = BLOCK; // Indicate I have not been used, thus can be destroyed
    // result->inst = nullptr;
    result->inst->op = COMMENT;
    result->inst->a = newOp("new LinkedInst", newInstInt(-1));
    return result;
}

INST::~INST(){
    // std::cout << " !!!!!!!!! DESTROYING INST !!!!!!!!! " << std::endl;
    // std::cin.ignore();
}

Instruction::~Instruction(){
    std::cout << "Attemp Instruction" << std::endl;
    INST* temp = next;
    next = nullptr;
    if(temp != nullptr){
        switch(temp->TYPE){
            case INT:
                delete (InstInt*)temp;
                break;
            case SINGLE:
                delete (Instruction*)temp;
                break;
            case BLOCK: // Do crazy stuff
                Instruction* instHead = (Instruction*)((InstBlock*)temp)->head;
                std::cout << "_+_+_+_+_+_+_+_+_+_+_+_+_+_" << instHead->a->name << std::endl;
                if(instHead->a->name[0] == 'I'){ // If statement
                    delete (InstBlock*)temp->next->next->next;
                    delete (InstBlock*) temp->next->next;
                    delete (InstBlock*) temp->next;
                    delete (InstBlock*) ((InstBlock*)temp)->next2;
                }
                else if(instHead->a->name[0] == 'C'){
                    // function only split once, next to the function itself, and next 2 to what's next
                    delete (InstBlock*) ((InstBlock*)temp)->next2;
                }
                else if(instHead->a->name[0] == 'J'){
                    std::cout << ((Instruction*)((InstBlock*)(temp->next))->head)->a->name << std::endl;
                    std::cout << ((Instruction*)((InstBlock*)(temp->next->next))->head)->a->name << std::endl;
                    std::cout << ((Instruction*)((InstBlock*)(((InstBlock*)temp->next)->next2))->head)->a->name << std::endl;
                    delete (InstBlock*) temp->next->next;
                    delete (InstBlock*) ((InstBlock*)temp->next)->next2;
                    delete (InstBlock*) temp->next;
                }

                delete (InstBlock*)temp;
                break;
        }
    }
    std::cout << "Finished Instruction" << std::endl;

}

InstBlock::~InstBlock(){
    // std::cout << "Attemp InstBlock: " << name << std::endl;
    // std::cin.ignore();
    INST *temp, *temp1, *temp2;


    temp = head;
    // temp1 = next;
    // temp2 = next2;
    head = nullptr;
    next = nullptr;
    next2 = nullptr;
    if(temp != nullptr){
        switch(temp->TYPE){
            case INT:
                delete (InstInt*)temp;
                break;
            case SINGLE:
                delete (Instruction*)temp;
                break;
            case BLOCK:
                delete (InstBlock*)temp;
                break;
        }
    }
    std::cout << "Finished InstBlock" << std::endl;

}

LinkedInst::~LinkedInst(){
    std::cout << "delete LinkedInst"<< std::endl;
    if(inst != NULL && inst->TYPE == BLOCK){ // Type == BLOCK only mean it is not used, thus should be destroyed
        delete inst;
    }
    if(next != nullptr){
        delete next;
        next = nullptr;
    }
    if(next2 != nullptr){ 
        delete next2;
        next2 = nullptr;
    }
    std::cout << "Finished LinkedInst"<< std::endl;
}

Opr::~Opr(){
    std::cout << "OPR" << std::endl;
    if(inst != NULL && inst->TYPE == INT) delete (InstInt*)inst;
    std::cout << "Finished OPR" << std::endl;
}

InstInt::~InstInt(){
    std::cout << "Calling Inst Int" << std::endl;
}
