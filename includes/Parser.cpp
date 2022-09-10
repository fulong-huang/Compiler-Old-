#include "Parser.h"


Ops relOp(){
    nextChar();
    Ops op;
    switch(CURR){
        case '=':
            op = EQ;
            skipNext(2); // must be "=="
            break;
        case '!':
            op = NE;
            skipNext(2); // must be "!="
            break;
        case '<':
            next();
            if(CURR == '='){
                op = LE;
                next();
            }
            else{ 
                op = LT;
            }
            break;
        case '>':
            next();
            if(CURR == '='){
                op = GE;
                next();
            }
            else{
                op = GT;
            }
            break;
        default:
            throw std::invalid_argument("relOp are == | != | < | <= | > | >=");
    }
    return op;
}

std::string ident(){
    // std::cout << "IDENT" << std::endl;
    nextChar();
    std::string ident;
    if(!isChar()){
        throw std::invalid_argument("ident need to start with a-z or A-Z");
    }
    while(isChar() || isdigit(CURR)){
        ident.push_back(CURR);
        next();
    }
    if(!isSpace(CURR)){
        throw std::invalid_argument("Ident unknown token found");
    }
    return ident;
}

int number(){
    // std::cout << "NUMBER" << std::endl;
    nextChar();
    if(!isdigit(CURR)){
        throw std::invalid_argument("Number need to start with 0-9");
    }
    // intNum = true;
    int val = CURR - '0';
    // int factor = 10;
    next();
    while(isdigit(CURR)){
        val *= 10;
        val += CURR - '0';
        next();
    }
    if(!isSpace(CURR) && !(CURR == '+' || CURR == '-' ||
                            CURR == '*' || CURR == '/')){
        throw std::invalid_argument("Number unknown token found");
    }
    return val;
}

std::pair<int, int> varRef(std::string ident){ // return <name, inst number> of var
    std::cout << "VARREF" << std::endl;
    return getVT(ident);
    // find inst num using ident();
}

// NEED: 
// ************ Function call *************
std::pair<std::string, int> factor(){ // return result as pair<name, int>
    std::cout << "FACTOR" << std::endl;

    std::pair<std::string, int> result;
    nextChar();
    if(isdigit(CURR)){          // number
        int n = number();
        result.first[0] = '#'; // constant
        result.second = n;
    }
    else if(CURR == '('){       // '(' expression ')'
        next();
        result = expression();       // ------------- change result -------------
        if(CURR != ')'){
            throw std::invalid_argument("Factor expecting token ')' after expression");
        }
    }
    else if(isChar()){
        if(nextIs("call")){     // function Call
            skipNext(4);       // must eat call before funCall();
            int fNum = funcCall();     // ------------- change result -------------
            result.first = "FunCall";
            result.second = fNum;
        }
        else{                   // varRef
            std::string idt = ident();
            std::pair<int, int> val = varRef(idt);
            
            if(val.first == -1){
                if(WhileIf || InWhile){
                    result.first = idt;
                    createInstCV(idt, val.second); 
                    result.second = varRef(idt).first;
                    put(result.first);
                    return result;
                }
                result.first = "#" + idt;
                result.second = val.second;
            }
            else if(val.first == -2){
                result.first = idt;
                result.second = 0;
                addCommentInst("Use of UNKNOWN variable: " + idt);
            }
            else if(val.first == -3){
                result.first = idt;
                result.second = 0;
                addCommentInst("Use of UNINITIALIZED variable: " + idt);
            }
            else{
                result.first = idt;
                result.second = val.first;
            }
        }
    }
    else{                       // END
        throw std::invalid_argument("Factor should ONLY be varRef|number|'('expression')'|funcCall");
    }
    return result;
}

std::pair<std::string, int> term(){
    // std::cout << "TERM" << std::endl;

    std::pair<std::string, int> lhs = factor();
    std::pair<std::string, int> rhs;
    bool constLHS, constRHS, mul;
    nextChar();
    mul = (CURR == '*');
    while( mul || CURR == '/'){
        next();
        rhs = factor();
        constLHS = (lhs.first[0] == '#');
        constRHS = (rhs.first[0] == '#');
        struct Instruction* inst = newInstruction();
        if(mul){ // ================ MULT =====================
            if(constLHS){
                if(constRHS){
                    lhs.second = lhs.second * rhs.second;
                    nextChar();
                    mul = (CURR == '*');
                    continue;
                }
                inst->InstNum = currInstNum;
                inst->op = MULI;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp(lhs.first, lhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;

                mul = (CURR == '*');    
                nextChar();
                continue;
            }
            else if(constRHS){
                inst->InstNum = currInstNum;
                inst->op = MULI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);

                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                
                mul = (CURR == '*');    
                continue;
            }
            else{
                inst->InstNum = currInstNum;
                inst->op = MUL;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);

                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                
                mul = (CURR == '*');    
                continue;
            }
        }
        else{    // ================ DIV =====================
            if(constLHS){
                if(constRHS){
                    lhs.second = lhs.second / rhs.second;
                    nextChar();
                
                    mul = (CURR == '*');    
                    continue;
                }
                // const lhs
                //  (lhs)/(rhs) ==       (1/(rhs))   *   (lhs)

                //   1 / (rhs):
                inst->InstNum = currInstNum++;
                inst->op = DIV;
                inst->a = newOp("-", 1);
                inst->b = newOp(rhs.first, rhs.second);
                addInst((INST*) inst);

                //  result * (lhs)
                inst = newInstruction();
                inst->InstNum = currInstNum;
                inst->op = MULI;
                inst->a = newOp("-", currInstNum-1);
                inst->b = newOp("#", lhs.second);
                addInst((INST*) inst);

                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                
                mul = (CURR == '*');    
                continue;
            }
            else if(constRHS){
                inst->InstNum = currInstNum;
                inst->op = DIVI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);

                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                
                mul = (CURR == '*');    
                continue;
            }
            else{
                inst->InstNum = currInstNum;
                inst->op = MUL;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);

                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                
                mul = (CURR == '*');    
                continue;  // some continue wasn't necessary
            }
        }
    }
    return lhs;
}


std::pair<std::string, int> expression(){
    std::cout << "EXPRESSION" << std::endl;
    nextChar();
    std::pair<std::string, int> lhs = term();
    std::pair<std::string, int> rhs;
    bool constLHS, constRHS, add;
    nextChar();
    add = (CURR == '+');
    while(add || CURR == '-'){
        next();
        rhs = term();
        constLHS = (lhs.first[0] == '#');
        constRHS = (rhs.first[0] == '#');
        struct Instruction* inst = newInstruction();
        if(add){ // ================ ADD =====================
            if(constLHS){
                if(constRHS){
                    lhs.second = lhs.second + rhs.second;
                    nextChar();

                    add = (CURR == '+');
                    continue;
                }
                inst->InstNum = currInstNum;
                inst->op = ADDI;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp("#", lhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
            else if(constRHS){
                inst->InstNum = currInstNum;
                inst->op = ADDI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
            else{
                inst->InstNum = currInstNum;
                inst->op = ADD;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
        }
        else{ // ================ SUB =====================
            if(constLHS){
                if(constRHS){
                    lhs.second = lhs.second - rhs.second;
                    nextChar();
                    
                    add = (CURR == '+');
                    continue;
                }
                // const lhs
                //  (lhs)-(rhs) =    (0 - (rhs)) + lhs

                //   0 - (rhs):
                inst->InstNum = currInstNum++;
                inst->op = NEG;
                inst->a = newOp(rhs.first, rhs.second);
                addInst((INST*) inst);

                //  result + (lhs)
                inst = newInstruction();
                inst->InstNum = currInstNum;
                inst->op = ADDI;
                inst->a = newOp("-", currInstNum-1);
                inst->b = newOp("#", lhs.second);
                addInst((INST*) inst);

                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
            else if(constRHS){
                inst->InstNum = currInstNum;
                inst->op = SUBI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
            else{
                inst->InstNum = currInstNum;
                inst->op = SUB;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp(lhs.first, lhs.second);
                addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = currInstNum++;
                nextChar();
                    
                add = (CURR == '+');
                continue;
            }
        }
    }
    return lhs;
}


// ========================================= MAY NOT REQUIRE RETURN ============================
void relation(std::string target){ 
    std::cout << "RELATION" << std::endl;

    std::pair<std::string, int> lhs = expression();
    Ops op = relOp();
    std::pair<std::string, int> rhs = expression();

    bool constLHS = (lhs.first[0] == '#');
    bool constRHS = (rhs.first[0] == '#');
    

    struct Instruction* inst;
    bool neg = false;
    if(lhs.first[0] == '#'){
        // if two constant: (1 > 2)
        if(rhs.first[0] == '#'){
            addCommentInst(("Comparing two constant: "
                + std::to_string(lhs.second) + ", " + 
                std::to_string(rhs.second)
            ));
            inst = newInstruction();
            inst->op = BRA;
            inst->a = newOp(target, -1);
            int diff = lhs.second - rhs.second;
            switch(op){
                case EQ: // branch when not equal
                    if(diff != 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                case NE:
                    if(diff == 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                case GT:
                    if(diff <= 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                case GE:
                    if(diff < 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                case LT:
                    if(diff >= 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                case LE:
                    if(diff > 0){
                        inst->InstNum = currInstNum++;
                        addInst((INST*) inst);
                    }
                    break;
                default:
                    throw std::invalid_argument("Unknow relOp\n");
                
            }
            return;
        }
        // else: (exp: 2 < a)
        else{ 
            std::pair<std::string, int> s = lhs;
            lhs = rhs;
            rhs = s;

            neg = true;
        }
    }

    if(rhs.first[0] == '#'){
        inst = newInstruction();
        inst->op = CMPI;
        inst->a = newOp(lhs.first, lhs.second);
        inst->b = newOp("#", rhs.second);
        inst->InstNum = currInstNum++;
        addInst((INST*) inst);

        if(neg){
            inst = newInstruction();
            inst->op = NEG;
            inst->a = newOp("-", currInstNum-1);
            inst->InstNum = currInstNum++;
            addInst((INST*) inst);
        }

        inst = newInstruction();
        switch(op){
            case EQ: // branch when not equal
                inst->op = BNE;
                break;
            case NE:
                inst->op = BEQ;
                break;
            case GT:
                inst->op = BLE;
                break;
            case GE:
                inst->op = BLT;
                break;
            case LT:
                inst->op = BGE;
                break;
            case LE:
                inst->op = BGT;
                break;
            default:
                throw std::invalid_argument("Unknow relOp\n");
        }
        inst->a = newOp("-", currInstNum-1);
        inst->b = newOp(target, -1);
        inst->InstNum = currInstNum++;
        addInst((INST*) inst);
        return;
    }
    inst = newInstruction();
    inst->op = CMP;
    inst->a = newOp(lhs.first, lhs.second);
    inst->b = newOp(rhs.first, rhs.second);
    inst->InstNum = currInstNum++;
    addInst((INST*) inst);
    inst = newInstruction();
    // relation only called when branch is required
    // ****** Branch to ELSE *******
    switch(op){
        case EQ: // branch when not equal
            inst->op = BNE;
            break;
        case NE:
            inst->op = BEQ;
            break;

        case GT:
            inst->op = BLE;
            break;
        case GE:
            inst->op = BLT;
            break;
        case LT:
            inst->op = BGE;
            break;
        case LE:
            inst->op = BGT;
            break;
        default:
            throw std::invalid_argument("Unknow relOp\n");
    }
    inst->a = newOp("-", currInstNum-1);
    inst->b = newOp(target, -1);
    inst->InstNum = currInstNum++;
    addInst((INST*) inst);

}

// ====================== ASSIGNMENT ========================
void assignment(){ 
    std::cout << "ASSIGNMENT" << std::endl;
    nextChar();
    std::string name = ident();
    nextChar();
    if(nextIs("<-")){
        skipNext(2);
    }
    else{
        throw std::invalid_argument("Assignment expecting \"<-\" after ident");
    }
    std::pair<std::string, int> val = expression();
    if(val.first[0] == '#'){ 
        insertCV(name, val.second);

        addCommentInst("Let " + name + " <- #" + 
            std::to_string(val.second)
        );
    }
    else{
        insertVT(name, val.second);
        addCommentInst("Let " + name + " <- (" + 
            std::to_string(varRef(name).first)+")"
        );
    }
}

int funcCall(){
    std::cout << "FUNCALL" << std::endl;
    nextChar();
    std::string funcName = ident();
    
    // **************************************************
    struct Instruction* inst = newInstruction();
    int instNum = currInstNum;
    inst->InstNum = currInstNum++;
    inst->a = newOp(funcName, -1);
    inst->op = BRA;
    addInst((INST*) inst);
    // **************************************************

    nextChar();
    if(CURR != '('){
        throw std::invalid_argument("FuncCall expecting \"(\" after ident");
    }
    next();
    nextChar();
    if(CURR != ')') {
        expression();
        nextChar();
        while(CURR == ','){
            next();
            expression();
            nextChar();
        }
        if(CURR != ')'){
            throw std::invalid_argument("FuncCall expecting \")\" after expression(s)");
        }
    }
    next();
    return instNum;
}

void ifStatement(){
    std::cout << "IF" << std::endl;

    std::string* labels = getIf();

    // Init instruction blocks;
    struct InstBlock* ifBlock = newInstBlock(labels[0]);
    struct InstBlock* thenBlock = newInstBlock(labels[1]);
    struct InstBlock* elseBlock = newInstBlock(labels[2]);
    struct InstBlock* fiBlock = newInstBlock(labels[3]);
    struct InstBlock* endBlock = newInstBlock(labels[4]);

    // Connect instruction Blocks;
    ifBlock->next = (INST*) thenBlock; 
    ifBlock->next2 = (INST*) elseBlock;
    thenBlock->next = (INST*) fiBlock; 
    elseBlock->next = (INST*) fiBlock; 
    fiBlock->next = (INST*) endBlock;

    bool inWhileSave = InWhile;
    bool whileIfSave = WhileIf;
    WhileIf = WhileIf || InWhile;
    InWhile = false;
    // Pointers to join block
    struct Instruction* joinHead = (Instruction*) fiBlock->head;
    struct Instruction* joinTail = joinHead;

    // Save JoinBlock (from outer block)
    //  Replace JoinBlock with current joinBlock (fiBlock).
    struct INST* savedJoin = JoinBlock;
    JoinBlock = (INST*) fiBlock;

    // Jump into if block <Check condition>
    addInst( (INST*) ifBlock);
    InstTail = ifBlock->head;

    nextChar();
    relation(labels[2]); 
    nextChar();

    // Then here
    if(!nextIs("then")){
        throw std::invalid_argument("IfStatement expecting \"then\" after relation");
    }
    skipNext(4); // eat "then"
    
    // Jump into then block
    InstTail = thenBlock->head;

    // ******************* Sub Value Table *******************
    InsertVTLayer();

    statSequence();
    nextChar();

    // Else statement start here. (else)

    // If statement need to skip it
    struct Instruction* inst = newInstruction();
    inst->op = BRA;
    inst->a = newOp(fiBlock->name, -1);
    inst->InstNum = currInstNum++;
    addInst((INST*) inst);

    // ******************* Insert PHI FUnction *******************
    // ******************* Sub Value Table *******************
    for(std::pair<std::string, int> i : ValueTable[ValueTable.size()-1]){
        joinTail = addPhiInst(joinTail, newOp(i.first, i.second), NULL);
    }
    // ******************* Sub Const Val *******************
    for(std::pair<std::string, int> i: ConstVal[ConstVal.size()-1]){
        // If value stores a constant,
        //  Store it into instruction
        //  Update Value table with newest instruction
        //  Add PHI function with previously stored value

        inst = newInstruction();
        inst->op = STORECONST;
        inst->InstNum = currInstNum;
        inst->a = newOp(i.first, i.second);
        joinTail->next = (INST*) inst;
        joinTail = inst;

        insertVT(i.first, currInstNum);

        joinTail = addPhiInst(joinTail, newOp(i.first, currInstNum++), NULL);
    }


    
    ClearLastLayer();

    // Jump into else block
    InstTail = elseBlock->head;
    if(nextIs("else")){
        // eat "else"
        skipNext(4);
        statSequence();
    }
    nextChar();



    // ******************* Update Phi Function *******************
    struct Instruction* phiInst = (Instruction*) joinHead->next;
    int idx = ValueTable.size()-1;
    std::string idnt;
    std::pair<int, int> getVTSave;
    while(phiInst != NULL){     // Fill out existing Instruction
        if(phiInst->op != PHI){
            phiInst = (Instruction*)  phiInst->next;
            continue;
        }
        idnt = phiInst->a->name;
        if(ValueTable[idx].find(idnt) != ValueTable[idx].end()){
            phiInst->b = newOp(idnt, ValueTable[idx][idnt]);
            ValueTable[idx].erase(idnt);
        }
        else if(ConstVal[idx].find(idnt) != ConstVal[idx].end()){
            inst = newInstruction();
            inst->op = STORECONST;
            inst->InstNum = currInstNum;
            inst->a = newOp(idnt, ConstVal[idx][idnt]);
            inst->next = (INST*) joinHead->next;
            joinHead->next = (INST*) inst;
            phiInst->b = newOp(idnt, currInstNum++);
            ConstVal[idx].erase(idnt);
        }
        else{
            getVTSave = getVT(idnt);
            if(getVTSave.first >= 0){ // if it's instruction
                phiInst->b = newOp(idnt, getVT(idnt).first);
            }
            else{ // if it's constant
                inst = newInstruction();
                inst->op = STORECONST;
                inst->InstNum = currInstNum;
                inst->a = newOp(idnt, getVTSave.second);
                inst->next = (INST*) joinHead->next;
                joinHead->next = (INST*) inst;
                phiInst->b = newOp(idnt, currInstNum++);
                
            }
        }
        phiInst->InstNum = currInstNum++;
        phiInst = (Instruction*) phiInst->next;
    }
    // ******************* Sub Value Table *******************
    for(std::pair<std::string, int> i : ValueTable[ValueTable.size()-1]){
        getVTSave = getPrevVT(i.first);
        if(getVTSave.first >= 0){
            joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, getVTSave.first));
        }
        else{
            inst = newInstruction();
            inst->op = STORECONST;
            inst->InstNum = currInstNum;
            inst->a = newOp(idnt, getVTSave.second);
            joinTail->next = (INST*) inst;
            joinTail = inst;

            joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, currInstNum++));
            
        }
        joinTail->InstNum = currInstNum++;
    }
    // ******************* Sub Const Val *******************
    for(std::pair<std::string, int> i: ConstVal[ConstVal.size()-1]){
        // If value stores a constant,
        //  Store it into instruction
        //  Update Value table with newest instruction

        inst = newInstruction();
        inst->op = STORECONST;
        inst->InstNum = currInstNum;
        inst->a = newOp(i.first, i.second);
        joinTail->next = (INST*) inst;
        joinTail = inst;

        insertVT(i.first, currInstNum);
        joinTail = addPhiInst(joinTail, newOp(i.first, currInstNum++), NULL);

        getVTSave = getPrevVT(i.first);
        if(getVTSave.first >= 0){
            joinTail->b = newOp(i.first, getVTSave.first);
        }
        else{
            inst = newInstruction();
            inst->op = STORECONST;
            inst->InstNum = currInstNum;
            inst->a = newOp(idnt, getVTSave.second);
            inst->next = joinHead->next;
            joinHead->next = (INST*) inst;

            joinTail->b = newOp(i.first, currInstNum++);
        }
        joinTail->InstNum = currInstNum++;
    }
    


    if(!nextIs("fi")){
        throw std::invalid_argument("IfStatement expecting \"fi\" to end if");
    }
    skipNext(2);
    
    
    // Jump into fi block
    InstTail = (INST*) endBlock->head;
    // ******************* Sub Value Table *******************
    RemoveVTLayer();

    InWhile = inWhileSave;
    WhileIf = whileIfSave;
    // ========== update current layer after removeal =============
    phiInst = (Instruction*) joinHead->next;
    while(phiInst != NULL){
        if(phiInst->op != PHI){ 
            phiInst = (Instruction*) phiInst->next;
            continue;
        }
        std::cout << " =" <<  ( phiInst->a == NULL ) 
        << std::endl;
        idnt = phiInst->a->name;

        insertVT(idnt, phiInst->InstNum);
        addCommentInst("Let " + idnt + " <- (" + 
            std::to_string(phiInst->InstNum) + ")"
        );
        phiInst = (Instruction*) phiInst->next;
    }
}


/*******************************************************
Condition is in the join block.
add phi to front of join block.

save current instruction (saveInstruct)
start new instruction for join block, 
start new instruction for block inside while, 
    replace it with current instruction
*******************************************************/
void whileStatement(){
    std::cout << "WHILE" << std::endl;
    std::string* labels = getWhile();

    // Init instruction blocks;
    struct InstBlock* whileBlock = newInstBlock(labels[0]);
    struct InstBlock* doBlock = newInstBlock(labels[1]);
    struct InstBlock* odBlock = newInstBlock(labels[2]);
    struct InstBlock* endBlock = newInstBlock(labels[3]);

    odBlock->next = (INST*) whileBlock;
    whileBlock->next = (INST*) doBlock;
    whileBlock->next2 = (INST*) endBlock;
    doBlock->next = (INST*) whileBlock;

    // Join block should be infront of everything.
    addInst( (INST*) odBlock);
    // struct Instruction* joinHead = (Instruction*) odBlock->head;
    // struct Instruction* joinTail = joinHead;


    // =========================================================
    // ================== INDICATE WHILE STARTED ===============
    // =========================================================
    bool inWhileSave = InWhile;

    struct Instruction* whileConditionSave = WhileCondition;
    struct Instruction* whileDoSave = WhileDo;
    struct Instruction* whileJoinSave = WhileJoin;

    InWhile = true;
    WhileCondition = (Instruction*) whileBlock->head;
    WhileDo        = (Instruction*) doBlock->head;
    WhileJoin      = (Instruction*) odBlock->head;


    // Jump into while block  <Check condition>
    InstTail = whileBlock->head;

    relation(labels[2]);
    nextChar();

    if(!nextIs("do")){
        throw std::invalid_argument("whileStatement expecting \"do\" after relation");
    }
    skipNext(2);
    // Do start
    // ******************* Sub Value Table *******************
    InsertVTLayer();

    // Jump into do block
    InstTail = doBlock->head;

    statSequence();
    nextChar();
    if(!nextIs("od")){
        throw std::invalid_argument("whileStatement expecting \"od\" to end the loop");
    }
    skipNext(2); 

    // *********** DO BLOCK END ***************
    // jump back to while block
    struct Instruction* inst = newInstruction();
    inst->op = BRA;
    inst->a = newOp(whileBlock->name, -1);
    inst->InstNum = currInstNum++;
    addInst((INST*) inst);

    // Jump into od block
    InstTail = endBlock->head;
    // ******************* Sub Value Table *******************
    // ******************* Insert PHI FUnction *******************
    // for(std::pair<std::string, int> i : ValueTable[ValueTable.size()-1]){
    //     joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, getVT(i.first).first));
        
    // }
    RemoveVTLayer();
    // Exit this while loop;
    InWhile = inWhileSave;
    WhileCondition = whileConditionSave;
    WhileDo = whileDoSave;
    WhileJoin = whileJoinSave;

    struct Instruction* phiInst = (Instruction*) odBlock->head->next;
    std::string idt;
    while(phiInst != NULL){
        if(phiInst->op != PHI){
            phiInst = (Instruction*) phiInst->next;
            continue;
        }
        idt = phiInst->a->name;
        insertVT(idt, phiInst->InstNum);
        addCommentInst("Let " + idt + " <- (" + 
            std::to_string(phiInst->InstNum) +")"
        );

        phiInst = (Instruction*) phiInst->next;
    }
}

void returnStatement(){ // only call when expression is guaranteed
    std::cout << "RETURN STATEMENT" << std::endl;
    expression();
}

void statement(){
    std::cout << "STATEMENT" << std::endl;
    nextChar();
    if(nextIs("let")){
        skipNext(3);
        assignment();
    }
    else if(nextIs("call")){
        skipNext(4);
        funcCall();
    }
    else if(nextIs("if")){
        skipNext(2);
        ifStatement();
    }
    else if(nextIs("while")){
        skipNext(5);
        whileStatement();
    }
    else if(nextIs("return")){
        skipNext(6);
        nextChar();
        // statement only called by statSequence,
        //  statement end with ';' or end with statSequence.
        if(CURR != ';' && !endStatSequence()){
            returnStatement();
        }
    }
    else{
        throw std::invalid_argument("Statement UNKNOWN STATEMENT");
    }
}

void statSequence(){ 

    std::cout << "STATSEQUENCE" << std::endl;
    statement();
    nextChar();
    while(CURR == ';'){
        next();
        nextChar();
        if(endStatSequence()){
            // if ';' is the end of last statement;
            break;
        }
        statement();
        nextChar();
    }
}

void varDecl(){ 
    std::cout << "VARDECL" << std::endl;
    nextChar(); // unnecessary since ident() have it in first line.
    declareVar(ident());
    nextChar();
    while(CURR == ','){
        next();
        declareVar(ident());
        nextChar();
    }
    nextChar();
    if(CURR != ';'){
        throw std::invalid_argument("VarDecl expecting \";\" after indent(s)");
    }
    next();
}

void funcDecl(){ 
    std::cout << "FUNCDECL" << std::endl;
    ident();
    formalParam(); 
    nextChar();
    if(CURR != ';'){
        throw std::invalid_argument("FuncDecl expecting \";\" after formalParam");
    }
    next();
    funcBody();
    nextChar();
    if(CURR != ';'){
        throw std::invalid_argument("FuncDecl expecting \";\" after funcBody");
    }
    next();
}

void formalParam(){
    std::cout << "FORMAL PARAM" << std::endl;
    nextChar();
    if(CURR != '('){
        throw std::invalid_argument("formalParam expecting \"(\" before ident");
    }
    next();
    nextChar();
    if(CURR != ')'){
        ident();
        nextChar();
        while(CURR == ','){
            next();
            ident();
            nextChar();
        }
    }
    nextChar();
    next();

}

void funcBody(){
    std::cout << "FUNCBODY" << std::endl;
    nextChar();
    while(nextIs("var")){
        skipNext(3);
        varDecl();
        nextChar();
    }
    // nextChar();
    if(CURR != '{'){
        throw std::invalid_argument("FuncBody expecting \"{\" before statSequence");
    }
    next();
    nextChar();
    if(CURR != '}'){
        statSequence();
    }
    if(CURR != '}'){
        throw std::invalid_argument("FuncBody expecting \"}\" after statSequence");
    }
    next();
}

void computation(){
    std::cout << "COMPUTATION" << std::endl;
    nextChar();

    if(CURR == -1) return;
    std::string s = ident();
    if(s.compare("main") != 0){
        throw std::invalid_argument("Computation expecting \"main\" at the top of the file");
    }
    
    nextChar();
    while(CURR != '{'){
        if(nextIs("var")){
            skipNext(3);
            varDecl();
        }
        else if(nextIs("void")){
            skipNext(4);
            nextChar();
            if(nextIs("function")){
                skipNext(8);
                funcDecl();
            }
            else{
                throw std::invalid_argument("Computation UNKNOWN TOKEN, expecting keyword \"function\"#");
            }
        }
        else if(nextIs("function")){
            skipNext(8);
            funcDecl();
        }
        else{
            throw std::invalid_argument("Computation expecting varDecl | funcDecl");
        }
        nextChar();
    }
    // next must be '{' to exit loop
    next(); // eat '{'
    statSequence();
    nextChar();
    if(CURR != '}'){
        std::cout << CURR << std::endl;
        throw std::invalid_argument("Computation expecting \"}\" to end statSequence");
    }
    next(); // eat '}'
    nextChar();
    if(CURR != '.'){
        throw std::invalid_argument("Computation expecting \".\" to end computation");
    }
    next(); // eat '.', which is end of computation;
}


