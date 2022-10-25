#include "Instruction.h"

void InitInstruction(){
    currInstNum = 1;
    InMain = true;

    InstBlock* instBlock = newInstBlock("MAIN", currInstNum++);
    ((Instruction*) instBlock->head)->op = COMMENT;
    InstHead = (INST*) instBlock;
    InstTail = ((InstBlock*) InstHead)->head;

    for(int i = 0; i < LICOUNT; i++){
        LinkedInstruction[i] = LinkedInstHead[i] = newLinkedInst();
    }
}


void addInst(struct INST* inst){
    InstTail->next = inst;
    InstTail = inst;
    // if(inst->TYPE == SINGLE){
    //     Instruction* i = (Instruction*) inst;
    //     put(opText[i->op]);
    // }
}

void appendLL(Instruction* inst, LIidx n){
    addInst((INST*) inst);
    LinkedInstruction[n]->next = newLinkedInst();
    LinkedInstruction[n] = LinkedInstruction[n]->next;
    LinkedInstruction[n]->inst = inst;
}

void elim(LinkedInst* LL, Instruction* inst){
    if(LL == NULL) return;
    if(inst->op == COMMENT) return;
    if(LL->inst->op == COMMENT){
        elim(LL->next, inst);
        elim(LL->next2, inst);
        return;
    }
    
    if(LL->inst->a->inst->TYPE == inst->a->inst->TYPE &&
        LL->inst->b->inst->TYPE == inst->b->inst->TYPE &&
        LL->inst->a->name.compare(inst->a->name) == 0 &&
        LL->inst->b->name.compare(inst->b->name) == 0)
    {        
        int lla, llb, ia, ib;
        if(LL->inst->a->inst->TYPE == INT){
            lla = ((InstInt*)LL->inst->a->inst)->num;
            ia = ((InstInt*)inst->a->inst)->num;
        }
        else{
            lla = LL->inst->a->inst->InstNum;
            ia = inst->a->inst->InstNum;
        }

        if(LL->inst->b->inst->TYPE == INT){
            llb = ((InstInt*)LL->inst->b->inst)->num;
            ib = ((InstInt*)inst->b->inst)->num;
        }
        else{
            llb = LL->inst->b->inst->InstNum;
            ib = inst->b->inst->InstNum;
        }
        if(ia == lla && ib == llb){
            LL->inst->a->name = "CSE: "+ opText[inst->op] + " " +
                            LL->inst->a->name+"("+std::to_string(ia)+") "+
                            LL->inst->b->name+"("+std::to_string(ib)+")";
            LL->inst->op = COMMENT;
            LL->inst->InstNum = inst->InstNum;
        }
        // ((InstInt*)LL->inst->a->inst)->num  = inst->InstNum;
    }
    elim(LL->next, inst);
    elim(LL->next2, inst);
}

void ElimSub(LinkedInst* LL){
    if(LL == NULL) return;

    elim(LL->next, LL->inst);
    elim(LL->next2, LL->inst);

    ElimSub(LL->next);
    ElimSub(LL->next2);
}

void CommonSubElim(){
    for(int i = 0; i < LICOUNT; i++){
        ElimSub(LinkedInstHead[i]);
    }
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
    inst = (Instruction*) blockInst->head;
    if(inst->a->name[0] == 'I'){
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
    else if(inst->a->name[0] == 'J'){
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
    else{
        std::cout << " ------- WARNING ------- UNKNOWN BLOCK FOUND IN [updateBlockInst] in [Instruction.cpp]  ------- WARNING ------- "<<blockInst->name << std::endl;
        sleep(3);
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
        inst->a = updateOp(inst->a, oldOp, newInst);
        inst->b = updateOp(inst->b, oldOp, newInst);
        instruction = instruction->next;
    }
}

Opr* updateOp(Opr* thisOp, Opr* targetOp, Instruction* newInst){
    if(thisOp!= NULL)
    std::cout <<thisOp->name<< ", "<<targetOp->name << std::endl;
    sleep(0.2);
    if(thisOp != NULL &&
        thisOp->inst == targetOp->inst &&
        thisOp->name.compare(targetOp->name) == 0)
    {
        if(thisOp->name[0] == '#'){
            std::cout << "NAME CHANGE " << std::endl;
            sleep(3);
            thisOp->name = thisOp->name.substr(1);
        }
        return newOp(thisOp->name, newInst);
    }
    return thisOp;
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
        if(inst->op == FUNC){
            put(stringIndent + std::to_string(inst->InstNum) + " " + inst->a->name);
        graph += "|"+std::to_string(inst->InstNum) + ": "+inst->a->name ;
            currInst = currInst->next;
            continue;

        }
        std::string cmd = std::to_string(inst->InstNum) + "\t" + 
                        opText[inst->op] + " ";
        graph += "|"+std::to_string(inst->InstNum)+": "+opText[inst->op] + " ";
        if(inst->a->name[0] != '-' && inst->a->name[0] != '#'){
            cmd += inst->a->name;
            graph += " " + inst->a->name;
        }
        n = inst->a->inst->TYPE == INT? ((InstInt*)inst->a->inst)->num : inst->a->inst->InstNum;
        if(inst->a->name.compare("#") == 0){
            cmd += std::to_string(n) + " ";
            graph += std::to_string(n) + " ";
        }
        else if(n != -1
|| inst->op == PHI
        ){
            cmd += "(" + std::to_string(n) + ") ";
            graph += "("+std::to_string(n) + ")";
        }
        // else if(inst->op == PHI){
        //     cmd += "(" + std::to_string(0) + ") ";
        //     graph += "("+std::to_string(0) + ")";
        // }
        
        if(inst->b != NULL){
            // n = inst->b->inst->InstNum;
            n = inst->b->inst->TYPE == INT? ((InstInt*)inst->b->inst)->num : inst->b->inst->InstNum;
            if(inst->b->name != "#" && inst->b->name != ""){
                if(inst->op == PHI){
                    cmd += " " + inst->b->name + "(" + 
                        std::to_string(n) + ")";
                    graph += " " + inst->b->name + "("+std::to_string(n)+")";
                    if(n == -1){
                        graph += "| \\<WARNING: Variable " + inst->b->name + 
                            " not initialized in all path" + "\\>";
                        put(("| <WARNING: Variable " + inst->b->name + 
                            " not initialized in all path" + ">"));
                    }
                }
                else if(n == -1){
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
                // if(n != 0 && n != 1){
                    cmd += " #" + std::to_string(n) + " ";
                    graph += " #" + std::to_string(n);
                // }
                // else{
                //     cmd += " (" + std::to_string(n) + ")";
                //     graph += " (" + std::to_string(n) + ")";
                // }
            }
        }
        put(stringIndent + cmd);

        currInst = currInst->next;
    }
}



