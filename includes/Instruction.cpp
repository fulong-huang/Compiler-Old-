
#include "Instruction.h"


struct InstBlock* newInstBlock(std::string blockName){
    InstBlock* inst = new struct InstBlock();
    inst->TYPE = BLOCK;
    inst->next = NULL;
    inst->next2 = NULL;
    inst->head = (INST*) newInstruction();
    inst->name = blockName;
    return inst;
}

struct Instruction* newInstruction(){
    Instruction* inst = new struct Instruction();
    inst->TYPE = SINGLE;
    inst->next = NULL;
    inst->InstNum = -1;
    return inst;
}

void addInst(struct INST* inst){
    InstTail->next = inst;
    InstTail = inst;
}

void InitInstruction(){
    currInstNum = 2;
    InstHead = (INST*) newInstBlock("MAIN");
    InstTail = ((InstBlock*) InstHead)->head;
}

void PrintInstBlock(struct InstBlock* instBlock){
    struct InstBlock *n1, *n2;
    if(instBlock->name[0] == 'I'){
        addLabel(instBlock->name);
        PrintInst((INST*) instBlock->head);
        n1 = (InstBlock*) instBlock->next;
        n2 = (InstBlock*) instBlock->next2;
        addLabel(n1->name);
        PrintInst(n1->head);
        addLabel(n2->name);
        PrintInst(n2->head);

        n1 = (InstBlock*) n1->next;
        addLabel(n1->name);
        PrintInst(n1->head);
    }
    else if(instBlock->name[0] == 'W'){
        addLabel(instBlock->name);
        PrintInst((INST*) instBlock->head);

        n1 = (InstBlock*) instBlock->next;
        n2 = (InstBlock*) instBlock->next2;
        
        addLabel(n1->name);
        PrintInst(n1->head);

        addLabel(n2->name);
        PrintInst(n2->head);
    }
    else if(instBlock->name[0] == 'M'){ // Main block, only ran once
        addLabel(instBlock->name);
        PrintInst((INST*) instBlock->head);
    }
    else{
        addLabel("============== UNKNOWN LABEL NAME ==============" + instBlock->name);
    }
}

void PrintInst(struct INST* currInst){
    struct Instruction* inst;
    while(currInst != NULL){
        if(currInst->TYPE == BLOCK){
            PrintInstBlock((InstBlock*) currInst);
            break;
        }
        inst = (Instruction*) currInst;
        if(inst->InstNum == -1){
            currInst = currInst->next;
            continue;
        }
        if(inst->op == LABEL){
            addLabel(inst->a->name);
            currInst = currInst->next;
            continue;
        }
        std::string cmd = std::to_string(inst->InstNum) + "\t" + 
                        opText[inst->op] + " " + 
                        inst->a->name+std::to_string(inst->a->instNum) + " ";
        
        if(inst->b != NULL){
            if(inst->b->name != ""){
                cmd += "(" + inst->b->name + ")";
            }
            cmd += std::to_string(inst->b->instNum) + " ";
        }
        if(inst->c != NULL){
            if(inst->c->name != ""){
                cmd += "(" + inst->c->name + ")";
            }
            cmd += std::to_string(inst->c->instNum) + " ";
        }
        put(cmd);

        currInst = currInst->next;
    }
}


