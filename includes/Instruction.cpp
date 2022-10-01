#include "Instruction.h"


struct InstBlock* newInstBlock(std::string blockName){
    InstBlock* inst = new struct InstBlock();
    inst->TYPE = BLOCK;
    inst->next = NULL;
    inst->next2 = NULL;
    inst->head = (INST*) newInstruction();
    ((Instruction*)inst->head)->InstNum = currInstNum++;
    ((Instruction*)inst->head)->a = newOp(blockName, newInstInt(-1));
    inst->name = blockName;
    return inst;
}

struct Instruction* newInstruction(){
    Instruction* inst = new struct Instruction();
    inst->TYPE = SINGLE;
    inst->next = NULL;
    inst->InstNum = -1;
    inst->op = NLL;
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
    instruction->b = b;
    instruction->InstNum = 102;
    inst->next = (INST*) instruction;
    return instruction;
}

void addLabelInst(std::string labl){
    struct Instruction* inst = newInstruction();
    inst->op = LABEL;
    inst->InstNum = 100;
    inst->a = newOp(labl, newInstInt(-1));
    addInst((INST*) inst);
}

void addCommentInst(std::string comt){
    struct Instruction* inst = newInstruction();
    inst->op = COMMENT;
    inst->InstNum = 101;
    inst->a = newOp(comt, newInstInt(-1));
    addInst((INST*) inst);
}

void InitInstruction(){
    currInstNum = 1;
    InMain = true;

    InstBlock* instBlock = newInstBlock("MAIN");
    ((Instruction*) instBlock->head)->op = COMMENT;
    InstHead = (INST*) instBlock;
    InstTail = ((InstBlock*) InstHead)->head;
}

void updateInst(Opr* oldOp, Instruction* newInst){ // use when while loop create an update
    // 1. constant value were referenced or assigned
    // 2. value have different instruction number.
    updateIndivInst((INST*) WhileCondition, oldOp, newInst);
    updateIndivInst((INST*) WhileDo, oldOp, newInst);
}

void updateBlockInst(struct INST* instruction, Opr* oldOp, Instruction* newInst){
    struct Instruction* inst;
    struct InstBlock* blockInst;
    struct InstBlock* saveBlock;

    blockInst = (InstBlock*) instruction;
    if(blockInst->name[0] == 'I'){
        // cond if
        updateIndivInst(blockInst->head, oldOp, newInst);

        // then (cond->next)
        updateIndivInst(((InstBlock*) blockInst->next)->head, oldOp, newInst);

        // else (cond->next2)
        updateIndivInst(((InstBlock*) blockInst->next2)->head, oldOp, newInst);

        // join (then/else->next)
        updateIndivInst(((InstBlock*) blockInst->next2->next)->head, oldOp, newInst);

        // end block
        updateIndivInst(((InstBlock*) blockInst->next2->next->next)->head, oldOp, newInst);

    }
    else if(blockInst->name[0] == 'J'){
        // join block
        updateIndivInst(blockInst->head, oldOp, newInst);

        // cond
        updateIndivInst(((InstBlock*) blockInst->next)->head, oldOp, newInst);

        // do block
        updateIndivInst(((InstBlock*) blockInst->next->next)->head, oldOp, newInst);

        // end block
        blockInst = (InstBlock*) blockInst->next;
        updateIndivInst(((InstBlock*) blockInst->next2)->head, oldOp, newInst);
    }
}

void updateIndivInst(INST* instruction, Opr* oldOp, Instruction* newInst){
    struct Opr* op;
    struct Instruction* inst;
    while(instruction != NULL){
        if(instruction->TYPE == BLOCK){
            updateBlockInst(instruction, oldOp, newInst);
            break;
        }
        inst = (Instruction*) instruction;
        updateOp(inst->a, oldOp, newInst);
        updateOp(inst->b, oldOp, newInst);
        instruction = instruction->next;
    }
}

void updateOp(Opr* thisOp, Opr* targetOp, Instruction* newInst){
    if(thisOp != NULL &&
        thisOp->inst == targetOp->inst &&
        thisOp->name.compare(targetOp->name) == 0)
    {
        thisOp->inst = newInst;
        if(thisOp->name[0] == '#'){
            thisOp->name = thisOp->name.substr(1);
        }
    }
}

void PrintInstBlock(struct InstBlock* instBlock){
    struct InstBlock *n1, *n2;
    stringIndent += "\t";
    Instruction* insthead = (Instruction*) instBlock->head;
    if(insthead->a->name[0] == 'I'){
        // Condition block
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {";
        graphConnection += prevLabel+":s -> "+instBlock->name+":n [label=\"fall-through(cond)\"];\n";
        prevLabel = instBlock->name;
        addLabel(stringIndent + instBlock->name);
        PrintInst((INST*) instBlock->head);

        n1 = (InstBlock*) instBlock->next;
        n2 = (InstBlock*) instBlock->next2;

        // Then block
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n1->name+"[shape=record, label=\"<b>"+n1->name+"| {";
        graphConnection += prevLabel+":s -> "+n1->name+":n [label=\"fall-through\"];\n";
        graphConnection += prevLabel+":s -> "+n2->name+":n [label=\"branch\"];\n";
        prevLabel = n1->name;
        addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        // connect to join block before prevLabel overided by next PrintInst
        //  PrintInst may encounter another block and change prevLabel
        n1 = (InstBlock*) n1->next;
        // then block to join block
        graphConnection += prevLabel+":s -> "+n1->name+":n [label=\"branch\"];\n";

        // Else block
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n2->name+"[shape=record, label=\"<b>"+n2->name+"| {";
        prevLabel = n2->name;
        addLabel(stringIndent + n2->name);
        PrintInst(n2->head);

        // join block part 2
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n1->name+"[shape=record, label=\"<b>"+n1->name+"| {";
        graphConnection += prevLabel+":s -> "+n1->name+":n [label=\"fall-through\"];\n";
        prevLabel = n1->name;
        addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        // returned to upper level
        stringIndent = stringIndent.substr(0, stringIndent.size()-1);
        n1 = (InstBlock*) n1->next;
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n1->name+"[shape=record, label=\"<b>"+n1->name+"| {"; 
        graphConnection += prevLabel+":s -> "+n1->name+":n [label=\"fall-thorough(out)\"];\n";
        prevLabel = n1->name;
        addLabel(stringIndent + n1->name);
        PrintInst(n1->head);
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }

    }
    else if(insthead->a->name[0] == 'J'){ // Join while
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {";
        graphConnection += prevLabel+":s -> "+instBlock->name+":n [label=\"fall-thorough\"];\n";
        prevLabel = instBlock->name;
        std::string joinName = instBlock->name;
        addLabel(stringIndent + instBlock->name);
        // print join block
        PrintInst((INST*) instBlock->head);

        // print while block
        n1 = (InstBlock*) instBlock->next;
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n1->name+"[shape=record, label=\"<b>"+n1->name+"| {";
        graphConnection += prevLabel+":s -> "+n1->name+":n [label=\"fall-thorough\"];\n";
        prevLabel = n1->name;
        addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        std::string whileName = n1->name;
        n2 = (InstBlock*) n1->next2;
        n1 = (InstBlock*) n1->next;
        // print do block
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n1->name+"[shape=record, label=\"<b>"+n1->name+"| {";
        // while connect to do
        graphConnection += whileName+":s -> "+n1->name+":n [label=\"fall-thorough\",color=green];\n";
        prevLabel = n1->name;
        addLabel(stringIndent + n1->name);
        PrintInst(n1->head);
        // do connect to while
        //  Must be after printinst for the latest block to connect back to while
        graphConnection += prevLabel+":s -> "+joinName+":n [label=\"branch\",color=red];\n";

        // print end block 
        stringIndent = stringIndent.substr(0, stringIndent.size()-1);
        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
        graph += n2->name+"[shape=record, label=\"<b>"+n2->name+"| {";
        // while to end
        graphConnection += whileName+":s -> "+n2->name+":n [label=\"branch\",color=blue];\n";
        prevLabel = n2->name;
        addLabel(stringIndent + n2->name);
        PrintInst(n2->head);

        if(graph[graph.size()-1] != '\n'){
            graph += "}\"];\n";
        }
    }
    else if(insthead->a->name[0] == 'M'){ // Main block, only ran once
        stringIndent = "";
        addLabel(stringIndent+ instBlock->name);
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
            return;
        }
        inst = (Instruction*) currInst;
        if(inst->InstNum == -1){
            currInst = currInst->next;
            continue;
        }
        if(inst->op == LABEL){
            addLabel(stringIndent + inst->a->name);
            currInst = currInst->next;
            continue;
        }
        if(inst->op == COMMENT){
            put(stringIndent + "  <" + inst->a->name + ">");
        graph += "|\\<"+inst->a->name + "\\>";
            currInst = currInst->next;
            continue;
        }
        if(inst->op == NLL){
            put(stringIndent + std::to_string(inst->InstNum) + " <" + inst->a->name + "> ");
        graph += std::to_string(inst->InstNum) + ": \\<"+inst->a->name + "\\>";
            currInst = currInst->next;
            continue;
        }
        std::string cmd = std::to_string(inst->InstNum) + "\t" + 
                        opText[inst->op] + " ";
        graph += "|"+std::to_string(inst->InstNum)+": "+opText[inst->op] + " ";
        if(inst->a->name != "-"){
            cmd += inst->a->name;
            graph += " " + inst->a->name;
        }
        n = inst->a->inst->TYPE == INT? ((InstInt*)inst->a->inst)->num : inst->a->inst->InstNum;
        if(n != -1){
            cmd += "(" + std::to_string(n) + ") ";
            graph += "("+std::to_string(n) + ")";
        }
        
        if(inst->b != NULL){
            // n = inst->b->inst->InstNum;
            n = inst->b->inst->TYPE == INT? ((InstInt*)inst->b->inst)->num : inst->b->inst->InstNum;
            if(inst->b->name != "#"){
                if(n == -1){
                    cmd += " (" + inst->b->name + ")";
                    graph += " ("+inst->b->name + ")";
                }
                else{
                    cmd += " " + inst->b->name + "(" + 
                        std::to_string(n) + ")";
                    graph += " " + inst->b->name + "("+std::to_string(n)+")";
                }
            }
            else{
                if(n != 0 && n != 1){
                    cmd += " #" + std::to_string(n) + " ";
                    graph += " " + std::to_string(n);
                }
                else{
                    cmd += " (" + std::to_string(n) + ")";
                    graph += " (" + std::to_string(n) + ")";
                }
            }
        }
        put(stringIndent + cmd);

        currInst = currInst->next;
    }
}



