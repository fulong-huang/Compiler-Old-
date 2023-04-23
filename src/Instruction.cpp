#include "Instruction.h"

int currInstNum;
struct INST *InstHead, *InstTail;

struct INST *JoinBlock;

std::string stringIndent = "";

struct Instruction *WhileCondition, *WhileDo;
struct LinkedInst* LinkedInstHead[LICOUNT];

struct LinkedInst* LinkedInstruction[LICOUNT];

bool eliminating;

// ***** Functions Start: ***** //
void InitializeInstruction(){
    WhileCondition = NULL;
    WhileDo = NULL;
}

void DestroyInstruction(){
    // std::cout << "START" << std::endl;
    if(InstHead != nullptr){
    std::cout << "********** Destroying Instruction Head **********" << std::endl;
        switch(InstHead->TYPE){
            case INT:
                delete (InstInt*)InstHead;
                break;
            case SINGLE:
                delete (Instruction*)InstHead;
                break;
            case BLOCK:
                delete (InstBlock*)InstHead;
                break;
        }
    std::cout << "********** Instruction Head Destroyed **********" << std::endl;
    }
    
    // std::cout << "START 1" << std::endl;
    // if(InstTail != NULL){
    //     switch(InstTail->TYPE){
    //         case INT:
    //             delete (InstInt*)InstTail;
    //             break;
    //         case SINGLE:
    //             delete (Instruction*)InstTail;
    //             break;
    //         case BLOCK:
    //             delete (InstBlock*)InstTail;
    //             break;
    //     }
    // }
    // if(JoinBlock != NULL){
    //     switch(JoinBlock->TYPE){
    //         case INT:
    //             delete (InstInt*)JoinBlock;
    //             break;
    //         case SINGLE:
    //             delete (Instruction*)JoinBlock;
    //             break;
    //         case BLOCK:
    //             delete (InstBlock*)JoinBlock;
    //             break;
    //     }
    // }
    // if(WhileCondition != NULL) delete WhileCondition;
    // if(WhileDo != NULL) delete WhileDo;
    
    std::cout << "********* Destroying LinkedInst Head **********" << std::endl;
    for(int i = 0; i < LICOUNT; i++){
        if(LinkedInstHead[i] != nullptr) delete LinkedInstHead[i];
    }
    std::cout << "********* LinkedInst Head Destoryed **********" << std::endl;
}
struct INST* getJoinBlock(){ return JoinBlock; }
void setJoinBlock(INST* newJoinBlock){JoinBlock = newJoinBlock;}

void setWhileCondiction(Instruction* newCondition){
    WhileCondition = newCondition;
}
void setWhileDo(Instruction* newDo){
    WhileDo = newDo;
}
struct Instruction* getWHileCondiction(){
    return WhileCondition;
}
struct Instruction* getWhileDo(){
    return WhileDo;
} 
bool getEliminating(){return eliminating;}
void setEliminating(bool e){eliminating = e;}
struct INST* getInstHead(){return InstHead;}
void setInstHead(INST* newInstHead){InstHead = newInstHead;}


struct LinkedInst* getLI(int idx){
    return LinkedInstruction[idx];
}
void setLI(int idx, LinkedInst* newLI){
    LinkedInstruction[idx] = newLI;
}


int getCurrInstNum(){
    return currInstNum;
}
int incCurrInstNum(){
    return currInstNum++;
}

INST* getInstTail(){
    return InstTail;
}
void setInstTail(INST* newTail){
    InstTail = newTail;
}

void InitInstruction(){
    currInstNum = 1;
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
    delete LinkedInstruction[n]->inst;
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
        LL->inst->b->inst->TYPE == inst->b->inst->TYPE //&&
        // LL->inst->a->name.compare(inst->a->name) == 0 &&
        // LL->inst->b->name.compare(inst->b->name) == 0
        )
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
                            LL->inst->b->name+"("+std::to_string(ib)+")" ;
            LL->inst->op = COMMENT;
            LL->inst->InstNum = inst->InstNum;
            eliminating = true;
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


struct Instruction* addPhiInst(struct Instruction* inst, std::shared_ptr<Opr> a, std::shared_ptr<Opr> b){
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

void updateInst(std::shared_ptr<Opr> oldOp, Instruction* newInst){ // use when while loop create an update
    // 1. constant value were referenced or assigned
    // 2. value have different instruction number.
    updateIndivInst((INST*) WhileCondition, oldOp, newInst);
    updateIndivInst((INST*) WhileDo, oldOp, newInst);
}

void updateBlockInst(struct INST* instruction, std::shared_ptr<Opr> oldOp, Instruction* newInst){
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
        put(" ------- WARNING ------- UNKNOWN BLOCK FOUND IN [updateBlockInst] in [Instruction.cpp]  ------- WARNING ------- " + blockInst->name );
    }
}

void updateIndivInst(INST* instruction, std::shared_ptr<Opr> oldOp, Instruction* newInst){
    std::shared_ptr<Opr*> op;
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

std::shared_ptr<Opr> updateOp(std::shared_ptr<Opr> thisOp, std::shared_ptr<Opr> targetOp, Instruction* newInst){
    if(thisOp != NULL &&
        thisOp->inst == targetOp->inst &&
        thisOp->name.compare(targetOp->name) == 0)
    {
        if(thisOp->name[0] == '#'){
            // std::cout << "NAME CHANGE " << std::endl;
            // sleep(3);
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
    std::string g;
    if(insthead->a->name[0] == 'I'){
        // Condition block
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+instBlock->name+":n [label=\"fall-through(cond)\"];\n");
        setPrevLabel(instBlock->name);
        // addLabel(stringIndent + instBlock->name);
        PrintInst((INST*) instBlock->head);

        n1 = (InstBlock*) instBlock->next;
        n2 = (InstBlock*) instBlock->next2;

        g = getGraph();
        // Then block
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"fall-through\"];\n");
        addToGraphConnection(getPrevLabel()+":s -> "+n2->name+":n [label=\"branch\"];\n");
        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        // connect to join block before prevLabel overided by next PrintInst
        //  PrintInst may encounter another block and change prevLabel
        n1 = (InstBlock*) n1->next;
        // then block to join block
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"branch\"];\n");

        // Else block
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n2->name+"[shape=record, label=\"<b>"+n2->name+"| {");
        setPrevLabel(n2->name);
        // addLabel(stringIndent + n2->name);
        PrintInst(n2->head);

        // join block part 2
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"fall-through\"];\n");
        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        // returned to upper level
        stringIndent = stringIndent.substr(0, stringIndent.size()-1);
        n1 = (InstBlock*) n1->next;
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {"); 
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"fall-thorough(out)\"];\n");
        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }

    }
    else if(insthead->a->name[0] == 'J'){ // Join while
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+instBlock->name+":n [label=\"fall-thorough\"];\n");
        setPrevLabel(instBlock->name);
        std::string joinName = instBlock->name;
        // addLabel(stringIndent + instBlock->name);
        // print join block
        PrintInst((INST*) instBlock->head);

        // print while block
        n1 = (InstBlock*) instBlock->next;
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"fall-thorough\"];\n");
        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);

        std::string whileName = n1->name;
        n2 = (InstBlock*) n1->next2;
        n1 = (InstBlock*) n1->next;
        // print do block
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {");
        // while connect to do
        addToGraphConnection(whileName+":s -> "+n1->name+":n [label=\"fall-thorough\",color=green];\n");
        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);
        // do connect to while
        //  Must be after printinst for the latest block to connect back to while
        addToGraphConnection(getPrevLabel()+":s -> "+joinName+":n [label=\"branch\",color=red];\n");

        // print end block 
        stringIndent = stringIndent.substr(0, stringIndent.size()-1);
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n2->name+"[shape=record, label=\"<b>"+n2->name+"| {");
        // while to end
        addToGraphConnection(whileName+":s -> "+n2->name+":n [label=\"branch\",color=blue];\n");
        setPrevLabel(n2->name);
        // addLabel(stringIndent + n2->name);
        PrintInst(n2->head);

        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
    }
    else if(insthead->a->name[0] == 'M'){ // Main block, only ran once
        stringIndent = "";
        setPrevLabel("BB0");
        // addLabel(stringIndent+ instBlock->name);
        PrintInst((INST*) instBlock->head);

    }
    else if(insthead->a->name[0] == 'F'){ // Function block, almost same as main
        stringIndent = "";
        setPrevLabel(instBlock->name);
        addToGraph(instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {");
        // addLabel(stringIndent+ instBlock->name);
        PrintInst((INST*) instBlock->head);

    }
    else if(insthead->a->name[0] == 'C'){ // Call function
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph( "}\"];\n");
        }
        addToGraph( instBlock->name+"[shape=record, label=\"<b>"+instBlock->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+instBlock->name+":n [label=\"fall-thorough\"];\n");
        setPrevLabel(instBlock->name);
        // addLabel(stringIndent + instBlock->name);
        PrintInst((INST*) instBlock->head);

        n1 = (InstBlock*) instBlock->next2;
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
        addToGraph(n1->name+"[shape=record, label=\"<b>"+n1->name+"| {");
        addToGraphConnection(getPrevLabel()+":s -> "+n1->name+":n [label=\"return\"];\n");

        n2 = (InstBlock*) instBlock->next;
        addToGraphConnection(getPrevLabel()+":sw -> "+n2->name+":e [label=\"Call Function\", color=orange, weight=0];\n");

        setPrevLabel(n1->name);
        // addLabel(stringIndent + n1->name);
        PrintInst(n1->head);
        g = getGraph();
        if(g[g.size()-1] != '\n'){
            addToGraph("}\"];\n");
        }
    }
    else{
        // addLabel("============== UNKNOWN LABEL NAME ==============" + instBlock->name);
        addToGraph("|\\< ===== UNKNOWN LABLE NAME ===== \\>");
        addToGraph("}\"];\n");
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
            // addLabel(stringIndent + inst->a->name);
            currInst = currInst->next;
            continue;
        }
        if(inst->op == COMMENT){
            // put(stringIndent + "  <" + inst->a->name + ">");
        addToGraph("|\\<"+inst->a->name + "\\>");
            currInst = currInst->next;
            continue;
        }
        if(inst->op == NLL){
            // put(stringIndent + std::to_string(inst->InstNum) + " <" + inst->a->name + "> ");
        addToGraph( std::to_string(inst->InstNum) + ": \\<"+inst->a->name + "\\>");
            currInst = currInst->next;
            continue;
        }
        if(inst->op == FUNC){
            // put(stringIndent + std::to_string(inst->InstNum) + " " + inst->a->name);
        addToGraph( "|"+std::to_string(inst->InstNum) + ": "+inst->a->name );
            if(inst->b != NULL){
                n = inst->b->inst->TYPE == INT? ((InstInt*)inst->b->inst)->num : inst->b->inst->InstNum;
                addToGraph( " " + inst->b->name + "(" + std::to_string(n) + ")");
            }
            currInst = currInst->next;
            continue;

        }
        std::string cmd = std::to_string(inst->InstNum) + "\t" + 
                        opText[inst->op] + " ";
        addToGraph( "|"+std::to_string(inst->InstNum)+": "+opText[inst->op] + " ");
        if(inst->a->name[0] != '-' && inst->a->name[0] != '#'){
            cmd += inst->a->name;
            addToGraph( " " + inst->a->name);
        }
        n = inst->a->inst->TYPE == INT? ((InstInt*)inst->a->inst)->num : inst->a->inst->InstNum;
        if(inst->a->name.compare("#") == 0){
            cmd += std::to_string(n) + " ";
            addToGraph( std::to_string(n) + " ");
        }
        else if(n != -1
|| inst->op == PHI
        ){
            cmd += "(" + std::to_string(n) + ") ";
            addToGraph( "("+std::to_string(n) + ")");
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
                    addToGraph( " " + inst->b->name + "("+std::to_string(n)+")");
                    if(n == -1){
                        addToGraph( "| \\<WARNING: Variable " + inst->b->name + 
                            " not initialized in all path" + "\\>");
                        // put(("| <WARNING: Variable " + inst->b->name + 
                        //     " not initialized in all path" + ">"));
                    }
                }
                else if(inst->op == MOVE || inst->b->name[0] == '-'){
                    cmd += " (" + std::to_string(n) + ")";
                    addToGraph( " (" + std::to_string(n) + ")");
                }
                else if(n == -1){
                    cmd += " (" + inst->b->name + ")";
                    addToGraph( " ("+inst->b->name + ")");
                }
                else{
                    cmd += " " + inst->b->name + "(" + 
                        std::to_string(n) + ")";
                    addToGraph( " " + inst->b->name + "("+std::to_string(n)+")");
                }
            }
            else{
                // if(n != 0 && n != 1){
                    cmd += " #" + std::to_string(n) + " ";
                    addToGraph( " #" + std::to_string(n));
                // }
                // else{
                //     cmd += " (" + std::to_string(n) + ")";
                //     graph += " (" + std::to_string(n) + ")";
                // }
            }
        }
        // put(stringIndent + cmd);

        currInst = currInst->next;
    }
}



