
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

struct Instruction* addPhiInst(struct Instruction* inst, struct Opr* a, struct Opr* b){
    struct Instruction* instruction = newInstruction();
    instruction->op = PHI;
    instruction->a = a;
    instruction->InstNum = 102;
    inst->next = (INST*) instruction;
    return instruction;
}

void addLabelInst(std::string labl){
    struct Instruction* inst = newInstruction();
    inst->op = LABEL;
    inst->InstNum = 100;
    inst->a = newOp(labl, -1);
    addInst((INST*) inst);
}

void addCommentInst(std::string comt){
    struct Instruction* inst = newInstruction();
    inst->op = COMMENT;
    inst->InstNum = 101;
    inst->a = newOp(comt, -1);
    addInst((INST*) inst);
}

void InitInstruction(){
    currInstNum = 2;
    InMain = true;
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

        // Join block
        n1 = (InstBlock*) n1->next;
        addLabel(n1->name);
        PrintInst(n1->head);

        // returned to upper level
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

        n2 = (InstBlock*) n2->next;
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
    int n;
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
        if(inst->op == COMMENT){
            put("\t<" + inst->a->name + ">");
            currInst = currInst->next;
            continue;
        }
        std::string cmd = std::to_string(inst->InstNum) + "\t" + 
                        opText[inst->op] + " ";
        if(inst->a->name != "-"){
            cmd += inst->a->name;
        }
        n = inst->a->instNum;
        if(n != -1){
            cmd += "(" + std::to_string(inst->a->instNum) + ") ";
        }
        
        if(inst->b != NULL){
            if(inst->b->name != ""){
                if(inst->b->instNum < 0)
                    cmd += " (" + inst->b->name + ")";
                else
                    cmd += " " + inst->b->name + "(" + 
                        std::to_string(inst->b->instNum) + ")";
            }
            else{
                cmd += " #" + std::to_string(inst->b->instNum) + " ";
            }
        }
        put(cmd);

        currInst = currInst->next;
    }
}



