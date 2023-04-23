#include "Parser.h"


Ops relOp(){
    nextChar();
    Ops op;
    switch(getCurr()){
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
            if(getCurr() == '='){
                op = LE;
                next();
            }
            else{ 
                op = LT;
            }
            break;
        case '>':
            next();
            if(getCurr() == '='){
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
    while(isChar() || isdigit(getCurr())){
        ident.push_back(getCurr());
        next();
    }
    if(!isSpace(getCurr())){
        throw std::invalid_argument("Ident unknown token found");
    }
    return ident;
}

int number(){
    // std::cout << "NUMBER" << std::endl;
    nextChar();
    if(!isdigit(getCurr())){
        throw std::invalid_argument("Number need to start with 0-9");
    }
    // intNum = true;
    int val = getCurr() - '0';
    // int factor = 10;
    next();
    while(isdigit(getCurr())){
        val *= 10;
        val += getCurr() - '0';
        next();
    }
    char c = getCurr();
    if(!isSpace(c) && !(c == '+' || c == '-' ||
                            c == '*' || c == '/')){
        throw std::invalid_argument("Number unknown token found");
    }
    return val;
}

Instruction* varRef(std::string ident){ // return <name, inst number> of var
    std::cout << "VARREF" << std::endl;
    return getVT(ident);
    // find inst num using ident();
}

// NEED: 
// ************ Function call *************
std::pair<std::string, Instruction*> factor(){
    std::cout << "FACTOR" << std::endl;

    std::pair<std::string, Instruction*> result;
    nextChar();
    if(isdigit(getCurr())){          // number
        int n = number();
        result.first[0] = '#'; // constant
        result.second = createConst(n);
    }
    else if(getCurr() == '('){       // '(' expression ')'
        next();
        result = expression();       // ------------- change result -------------
        if(getCurr() != ')'){
            throw std::invalid_argument("Factor expecting token ')' after expression");
        }
    }
    else if(isChar()){
        if(nextIs("call")){     // function Call
            skipNext(4);       // must eat call before funCall();
     // ------------- change result -------------
            result.first = "~FunCall";
            result.second = funcCall();
        }
        else{                   // varRef
            std::string idt = ident();
            Instruction* val = varRef(idt);
            result.first = idt;
            result.second = val;
            if(val->TYPE == INT){
                int valInt = ((InstInt*) val)->num;
                if(valInt == -1){
                    delete (InstInt*) result.second;
                //     if(WhileIf || InWhile){
                //         result.first = idt;
                //         insertCT(idt, val.second); 
                //         result.second = varRef(idt).first;
                //         return result;
                //     }
                    result.second = newInstInt(0);
                    addCommentInst("WARNING: Use of UNINITIALIZED variable: " + idt);
                }
                else if(valInt == -2){
                    delete (InstInt*) result.second;
                    result.second = newInstInt(0);
                    addCommentInst("WARNING: Use of UNDECLARED variable: " + idt);
                }
                result.first = "#" + idt;
            }
        }
    }
    else{                       // END
        throw std::invalid_argument("Factor should ONLY be varRef|number|'('expression')'|funcCall");
    }
    return result;
}

std::pair<std::string, Instruction*> term(){
    std::cout << "TERM" << std::endl;

    std::pair<std::string, Instruction*> lhs = factor();
    std::pair<std::string, Instruction*> rhs;
    bool constLHS, constRHS, mul;
    nextChar();
    mul = (getCurr() == '*');
    int savedNum;
    while( mul || getCurr() == '/'){
        next();
        rhs = factor();
        constLHS = (lhs.first[0] == '#');
        constRHS = (rhs.first[0] == '#');
        struct Instruction* inst = newInstruction();
        if(constRHS){
            savedNum = ((InstInt*) rhs.second)->num;
            delete (InstInt*) rhs.second;
            rhs.second = getCT(savedNum);
        }
        if(mul){ // ================ MULT =====================
            if(constLHS){
                savedNum = ((InstInt*) lhs.second)->num;
                delete (InstInt*) lhs.second;
                lhs.second = getCT(savedNum);
                if(constRHS){
                    ((InstInt*)lhs.second)->num = ((InstInt*)lhs.second)->num * ((InstInt*)rhs.second)->num;
                    nextChar();
                    savedNum = ((InstInt*) lhs.second)->num;
                    delete (InstInt*) lhs.second;
                    lhs.second = createConst(savedNum);
                    mul = (getCurr() == '*');
                    delete inst;
                    delete (InstInt*) rhs.second;
                    continue;
                }
                savedNum = ((InstInt*)lhs.second)->num;
                delete (InstInt*) lhs.second;
                inst->InstNum = getCurrInstNum();
                inst->op = MULI;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp("#", newInstInt(savedNum));
                // addInst((INST*) inst);
                appendLL(inst, lMULI);
                lhs.first = "-";
                lhs.second = inst;
                incCurrInstNum();// newInstInt(currInstNum++);

                mul = (getCurr() == '*');    
                nextChar();
                continue;
            }
            else if(constRHS){
                savedNum = ((InstInt*)rhs.second)->num;
                delete (InstInt*) rhs.second;
                inst->InstNum = getCurrInstNum();
                inst->op = MULI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", newInstInt(savedNum));

                // addInst((INST*) inst);
                appendLL(inst, lMULI);
                lhs.first = "-";
                lhs.second = inst;
                incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                
                mul = (getCurr() == '*');    
                continue;
            }
            else{
                inst->InstNum = getCurrInstNum();
                inst->op = MUL;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);

                // addInst((INST*) inst);
                appendLL(inst, lMUL);
                lhs.first = "-";
                lhs.second = inst;
                incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                
                mul = (getCurr() == '*');    
                continue;
            }
        }
        else{    // ================ DIV =====================
            if(constLHS){
                savedNum = ((InstInt*) lhs.second)->num;
                delete (InstInt*) lhs.second;
                lhs.second = getCT(savedNum);
                if(constRHS){

                    ((InstInt*)lhs.second)->num = ((InstInt*)lhs.second)->num / ((InstInt*)rhs.second)->num;
                    nextChar();
                    savedNum = ((InstInt*) lhs.second)->num;
                    delete (InstInt*) lhs.second;
                    lhs.second = createConst(savedNum);
                    mul = (getCurr() == '*');
                    delete inst;
                    continue;


                    // lhs.second = getCT(((InstInt*) lhs.second)->num);
                    // lhs.second = newInstInt(((InstInt*)lhs.second)->num / ((InstInt*)rhs.second)->num);
                    // nextChar();
                
                    // mul = (CURR == '*');    
                    // continue;
                }
                // const lhs
                //  (lhs)/(rhs) ==       (1/(rhs))   *   (lhs)

                //   1 / (rhs):
                // inst->InstNum = currInstNum++;
                // inst->op = DIV;
                // inst->a = newOp("-", newInstInt(1));
                // inst->b = newOp(rhs.first, rhs.second);
                // addInst((INST*) inst);

                //  result * (lhs)
                inst->InstNum = getCurrInstNum();
                inst->op = DIV;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lDIV);

                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                
                mul = (getCurr() == '*');    
                continue;
            }
            else if(constRHS){
                inst->InstNum = getCurrInstNum();
                inst->op = DIVI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lDIVI);

                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                
                mul = (getCurr() == '*');    
                continue;
            }
            else{
                inst->InstNum = getCurrInstNum();
                inst->op = DIV;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);

                // addInst((INST*) inst);
                appendLL(inst, lDIV);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                
                mul = (getCurr() == '*');    
                continue;  // some continue wasn't necessary
            }
        }
    }
    // if(rhs.second != NULL && rhs.second->TYPE == INT){
    //     delete (InstInt*) rhs.second;
    // }
    return lhs;
}


std::pair<std::string, Instruction*> expression(){
    std::cout << "EXPRESSION" << std::endl;
    nextChar();
    std::pair<std::string, Instruction*> lhs = term();
    std::pair<std::string, Instruction*> rhs;
    bool constLHS, constRHS, add;
    nextChar();
    add = (getCurr() == '+');
    int tempNum;
    while(add || getCurr() == '-'){
        next();
        rhs = term();
        constLHS = (lhs.first[0] == '#');
        constRHS = (rhs.first[0] == '#');
        struct Instruction* inst = newInstruction();
        if(constRHS){
            tempNum = ((InstInt*) rhs.second)->num;
            delete (InstInt*) rhs.second;
            rhs.second = getCT(tempNum);
        }
        if(add){ // ================ ADD =====================
            if(constLHS){
                tempNum = ((InstInt*) lhs.second)->num;
                delete (InstInt*) lhs.second;
                lhs.second = getCT(tempNum);
                if(constRHS){
                    ((InstInt*)lhs.second)->num = ((InstInt*)lhs.second)->num + ((InstInt*)rhs.second)->num;
                    nextChar();
                    tempNum = ((InstInt*)lhs.second)->num;
                    delete (InstInt*) lhs.second;
                    lhs.second = createConst(tempNum);
                    add = (getCurr() == '+');
                    delete inst;
                    delete (InstInt*) rhs.second;
                    continue;
                }
                inst->InstNum = getCurrInstNum();
                inst->op = ADDI;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp("#", lhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lADDI);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                continue;
            }
            else if(constRHS){
                inst->InstNum = getCurrInstNum();
                inst->op = ADDI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lADDI);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                    
                add = (getCurr() == '+');
                continue;
            }
            else{
                inst->InstNum = getCurrInstNum();
                inst->op = ADD;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);
                appendLL(inst, lADD);
                // addInst((INST*) inst);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                    
                add = (getCurr() == '+');
                continue;
            }
        }
        else{ // ================ SUB =====================
            if(constLHS){
                if(constRHS){
                    lhs.second = getCT(((InstInt*) lhs.second)->num);
                    ((InstInt*)lhs.second)->num = ((InstInt*)lhs.second)->num - ((InstInt*)rhs.second)->num;
                    nextChar();
                    tempNum = ((InstInt*)lhs.second)->num;
                    delete (InstInt*) lhs.second;
                    lhs.second = createConst(tempNum);
                    nextChar();
                    
                    add = (getCurr() == '+');
                    continue;
                }
                // const lhs
                //  (lhs)-(rhs) =    (0 - (rhs)) + lhs

                //   0 - (rhs):
                // inst->InstNum = currInstNum++;
                // inst->op = NEG;
                // inst->a = newOp(rhs.first, rhs.second);
                // addInst((INST*) inst);

                //  result + (lhs)
                inst->InstNum = getCurrInstNum();
                inst->op = SUB;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp(rhs.first, rhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lSUB);

                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                    
                add = (getCurr() == '+');
                continue;
            }
            else if(constRHS){
                inst->InstNum = getCurrInstNum();
                inst->op = SUBI;
                inst->a = newOp(lhs.first, lhs.second);
                inst->b = newOp("#", rhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lSUBI);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                    
                add = (getCurr() == '+');
                continue;
            }
            else{
                inst->InstNum = getCurrInstNum();
                inst->op = SUB;
                inst->a = newOp(rhs.first, rhs.second);
                inst->b = newOp(lhs.first, lhs.second);
                // addInst((INST*) inst);
                appendLL(inst, lSUB);
                lhs.first = "-";
                lhs.second = inst;incCurrInstNum();// newInstInt(currInstNum++);
                nextChar();
                    
                add = (getCurr() == '+');
                continue;
            }
        }
    }
    return lhs;
}


// ========================================= MAY NOT REQUIRE RETURN ============================
void relation(std::shared_ptr<Opr> target){ 
    std::cout << "RELATION" << std::endl;

    std::pair<std::string, Instruction*> lhs = expression();
    Ops op = relOp();
    std::pair<std::string, Instruction*> rhs = expression();

    bool constLHS = (lhs.first[0] == '#');
    bool constRHS = (rhs.first[0] == '#');
    std::cout << lhs.first<<","<<rhs.first<<std::endl;
    
    int tempNum;

    struct Instruction *inst, *prevInst;
    // bool neg = false;
    if(lhs.first[0] == '#'){
        // if two constant: (1 > 2)
        if(rhs.first[0] == '#'){
            tempNum = ((InstInt*) rhs.second)->num;
            delete (InstInt*) rhs.second;
            rhs.second = getCT(tempNum);
            tempNum = ((InstInt*) lhs.second)->num;
            delete (InstInt*) lhs.second;
            lhs.second = getCT(tempNum);
            addCommentInst(("Comparing two constant: "
                + std::to_string(((InstInt*)lhs.second)->num) + ", " + 
                std::to_string(((InstInt*)rhs.second)->num)
            ));
            inst = newInstruction();
            inst->op = BRA;
            inst->a = target;
            int diff = ((InstInt*)lhs.second)->num - ((InstInt*)rhs.second)->num;
            delete (InstInt*) lhs.second;
            delete (InstInt*) rhs.second;
            switch(op){
                case EQ: // branch when not equal
                    if(diff != 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                case NE:
                    if(diff == 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                case GT:
                    if(diff <= 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                case GE:
                    if(diff < 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                case LT:
                    if(diff >= 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                case LE:
                    if(diff > 0){
                        inst->InstNum = incCurrInstNum();
                        addInst((INST*) inst);
                    }
                    else{
                        delete inst;
                    }
                    break;
                default:
                    throw std::invalid_argument("Unknow relOp\n");
                
            }
            return;
        }
        // else: (exp: 2 < a)
        // else{ 
        //     std::pair<std::string, Instruction*> s = lhs;
        //     lhs = rhs;
        //     rhs = s;

        //     neg = true;
        // }
    }

    inst = newInstruction();
    if(rhs.first[0] == '#'){
        tempNum = ((InstInt*) rhs.second)->num;
        delete (InstInt*) rhs.second;
        rhs.second = getCT(tempNum);
        inst->op = CMPI;
        inst->a = newOp(lhs.first, lhs.second);
        inst->b = newOp("#", rhs.second);
    }
    else{
        inst->op = CMP;
        inst->a = newOp(lhs.first, lhs.second);
        inst->b = newOp(rhs.first, rhs.second);
    }
    inst->InstNum = incCurrInstNum();
    // addInst((INST*) inst);
    appendLL(inst, lCMP);
    prevInst = inst;
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
    // inst->a = newOp("-", newInstInt(currInstNum-1));
    inst->a = newOp("-", prevInst);
    inst->b = target;
    inst->InstNum = incCurrInstNum();
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
    std::pair<std::string, Instruction*> val = expression();
    Instruction* gVT = getVT(name); 
    int tempNum;
    if(val.first[0] == '#'){ 
        if(((InstInt*) gVT)->num == -2){
            addCommentInst("WARNING: Assign to UNDECLARED variable: " + name);
        }
        if(getInIf() || getInWhile()){
            
            Instruction* inst = newInstruction();
            inst->InstNum = incCurrInstNum();
            tempNum = ((InstInt*) val.second)->num;
            delete (InstInt*) val.second;
            inst->a = newOp("#", getCT(tempNum));
            inst->op = CONST;
            addInst((INST*) inst);
            insertVT(name, inst);
            // std::cout << "TRUE " << std::endl;
            // sleep(3);
        }
        else{
            insertCT(name, val.second);
            delete (InstInt*) val.second;
        }

        // addCommentInst("Let " + name + " = #" + 
        //     std::to_string(val.second)
        // );
    }
    else{
        insertVT(name, val.second);
        // std::cout << "---"<< name << ", " << val.second->InstNum <<" ----- " << InWhile << "---" << WhileIf<< std::endl;
        // addCommentInst("Let " + name + " = (" + 
        //     std::to_string(varRef(name).first)+")"
        // );
    }
    if(gVT->TYPE == INT){
        // std::cout << "CONST _" <<val.first<<"_" << ((InstInt*) val.second)->num << std::endl;
        // sleep(1);
        delete (InstInt*) gVT;
    }
}

Instruction* funcCall(){
    std::cout << "FUNCALL" << std::endl;
    nextChar();
    std::string funcName = ident();
    // **************************************************
    struct Instruction* inst;
    // **************************************************

    nextChar();
    if(getCurr() != '('){
        throw std::invalid_argument("FuncCall expecting \"(\" after ident");
    }
    next();
    nextChar();

    // create new block
    std::vector<std::pair<std::__1::string, Instruction *> > oprs;

    if(getCurr() != ')') {
        oprs.push_back(expression());
        nextChar();
        while(getCurr() == ','){
            next();
            oprs.push_back(expression());
            nextChar();
        }
        if(getCurr() != ')'){
            throw std::invalid_argument("FuncCall expecting \")\" after expression(s)");
        }
    }
    if(funcName.compare("InputNum") == 0){
        inst = newInstruction();
        inst->InstNum = incCurrInstNum();
        inst->a = newOp(("read()"), newInstInt(-202));
        inst->op = FUNC;
        addInst((INST*) inst);
    }
    else if(funcName.compare("OutputNum") == 0){
        if(oprs.size() != 1){
            throw std::invalid_argument("Function needs \"1\" argument");
        }
        std::string s = "write";
        inst = newInstruction();
        inst->InstNum = incCurrInstNum();
        inst->a = newOp((s), newInstInt(-202));
        inst->b = newOp(oprs[0].first, oprs[0].second);
        inst->op = FUNC;
        addInst((INST*) inst);
    }
    else if(funcName.compare("OutputNewLine") == 0){
        inst = newInstruction();
        inst->InstNum = incCurrInstNum();
        inst->a = newOp(("writeNL()"), newInstInt(-202));
        inst->op = FUNC;
        addInst((INST*) inst);
    }
    else{
        std::vector<Instruction*> foprs;
        InstBlock* funcBlock;
        auto gfp = getFunctionParam(funcName);
        funcBlock = gfp.first;
        foprs = gfp.second;
        if(oprs.size() != foprs.size()-1){
            throw std::invalid_argument("FuncCall contains too many arguments. Expect "+
                std::to_string(foprs.size()) + " got " +
                std::to_string(oprs.size()));
        }

        struct InstBlock* CallBlock = newInstBlock("Call " + funcName, incCurrInstNum());
        struct InstBlock* returnBlock = newInstBlock("Return " + funcName, incCurrInstNum());
        CallBlock->next = (INST*) funcBlock;
        CallBlock->next2 = (INST*) returnBlock;

        inst = newInstruction();
        inst->b = newOp("-", newInstInt(foprs[0]->InstNum));
        inst->a = newOp("Return", (Instruction*) returnBlock->head);
        inst->op = STORE;
        inst->InstNum = incCurrInstNum();
        addInst((INST*) inst);

        CallBlock->name = getBlock();
        addInst( (INST*) CallBlock);
        setInstTail(CallBlock->head);

        // Moing all parameters         AKA initialize
        for(int i = 0; i < oprs.size(); i++){
            inst = newInstruction();
            inst->InstNum = incCurrInstNum();
            inst->a = newOp(oprs[i].first, oprs[i].second);
            inst->b = newOp("-", newInstInt(foprs[i]->InstNum));
            inst->op = MOVE;
            addInst((INST*) inst);
        }
        inst = newInstruction();
        int instNum = getCurrInstNum();
        inst->InstNum = incCurrInstNum();
        inst->a = newOp(funcName, newInstInt(-1));
        inst->op = BRA;
        addInst((INST*) inst);

        returnBlock->name = getBlock();
        setInstTail(returnBlock->head);
        std::shared_ptr<Opr> returnOp = ((Instruction* )funcBlock->head)->b;
        if(returnOp != NULL){
            inst = newInstruction();
            inst->op = RETURN;
            inst->InstNum = incCurrInstNum();
            inst->a = returnOp;
            addInst((INST*) inst);
        }
    }
    
    next();
    return inst;
}

void ifStatement(){
    std::cout << "IF" << std::endl;

    std::vector<std::string> labels = getIf();
    // LinkedInst 
    struct LinkedInst *LLsave[LICOUNT], *LLnew[LICOUNT], *LL[LICOUNT];

    // Init instruction blocks;
    struct InstBlock* ifBlock = newInstBlock(labels[0], incCurrInstNum());
    struct InstBlock* thenBlock = newInstBlock(labels[1], incCurrInstNum());
    struct InstBlock* elseBlock = newInstBlock(labels[2], incCurrInstNum());
    struct InstBlock* fiBlock = newInstBlock(labels[3], incCurrInstNum());
    struct InstBlock* endBlock = newInstBlock(labels[4], incCurrInstNum());
    // Pointers to join block
    struct Instruction* joinHead = (Instruction*) fiBlock->head;
    struct Instruction* joinTail = joinHead;

    std::shared_ptr<Opr> elseTarget = newOp(labels[2], ((Instruction*) elseBlock->head));
    std::shared_ptr<Opr> fiTarget = newOp(labels[3], joinHead);
    
    // Connect instruction Blocks;
    ifBlock->next = (INST*) thenBlock; 
    ifBlock->next2 = (INST*) elseBlock;
    thenBlock->next = (INST*) fiBlock; 
    elseBlock->next = (INST*) fiBlock; 
    fiBlock->next = (INST*) endBlock;

    bool inWhileSave = getInWhile();
    bool inIfSave = getInIf();
    setInIf( true);
    setInWhile(false);

    // Save JoinBlock (from outer block)
    //  Replace JoinBlock with current joinBlock (fiBlock).
    struct INST* savedJoin = getJoinBlock();
    setJoinBlock((INST*) fiBlock);

    // Jump into if block <Check condition>
    ifBlock->name = getBlock();
    addInst( (INST*) ifBlock);
    setInstTail(ifBlock->head);

    nextChar();
    relation(elseTarget); 
    nextChar();

    // Then here
    if(!nextIs("then")){  // Jump into then block THEN   THEN   THEN   THEN   THEN   THEN   THEN   THEN   THEN   THEN   
        throw std::invalid_argument("IfStatement expecting \"then\" after relation");
    }
    skipNext(4); // eat "then"

    // Save:      - current Linked Instruction to LL
    // Create new empty Linked Inst to store then and else, as next and next2;
    // Save:      - new Linked Inst to LLsave;
    // Work on new Linked Inst for "Then" and "Else" block
    // Wrap back to LL when finish if statement;
    for(int i = 0; i < LICOUNT; i++){
        LL[i] = getLI(i);
        getLI(i)->next2 = newLinkedInst();
        LLsave[i] = getLI(i)->next2;

        LLsave[i]->next = newLinkedInst();
        setLI(i, LLsave[i]->next);
    }

    thenBlock->name = getBlock();
    setInstTail(thenBlock->head);

    // ******************* Sub Value Table *******************
    InsertVTLayer();

    statSequence();
    nextChar();

    // Else statement start here. (else)

    // If statement need to skip it
    struct Instruction* inst = newInstruction();
    inst->op = BRA;
    inst->a = fiTarget;
    inst->InstNum = incCurrInstNum();
    addInst((INST*) inst);

    // ******************* Insert PHI FUnction *******************
    // ******************* Sub Value Table *******************
    std::vector<std::unordered_map<std::string, Instruction*> > vt = getValueTable();
    for(std::pair<std::string, Instruction*> i : vt[vt.size()-1]){
        joinTail = addPhiInst(joinTail, newOp(i.first, i.second), NULL);
    }
    // ******************* Sub Const Val *******************
    // for(std::pair<std::string, int> i: ConstVal[ConstVal.size()-1]){
    //     // If value stores a constant,
    //     //  Store it into instruction
    //     //  Update Value table with newest instruction
    //     //  Add PHI function with previously stored value

    //     inst = newInstruction();
    //     inst->op = STORECONST;
    //     inst->InstNum = currInstNum;
    //     inst->a = newOp(i.first, i.second);
    //     joinTail->next = (INST*) inst;
    //     joinTail = inst;

    //     insertVT(i.first, currInstNum);

    //     joinTail = addPhiInst(joinTail, newOp(i.first, currInstNum++), NULL);
    // }


    
    ClearLastLayer();

    // Jump into else block
    elseBlock->name = getBlock();
    setInstTail(elseBlock->head);
    if(nextIs("else")){ // ELSE   ELSE   ELSE   ELSE   ELSE   ELSE   ELSE   ELSE   ELSE   
        // eat "else"
        skipNext(4);
        // Save current Linked Instruction, 
        //  change current Linked Instruction to be where else is.
        for(int i = 0; i < LICOUNT; i++){
            LLsave[i]->next2 = newLinkedInst();
            setLI(i, LLsave[i]->next2);
        }
        statSequence();
    }
    nextChar();
    
    // return current Linked Instruction to next of end of "if" block, before "then" block
    for(int i = 0; i < LICOUNT; i++){
        LL[i]->next = newLinkedInst();
        setLI(i, LL[i]->next);
    }


    // ******************* Update Phi Function *******************
    struct Instruction* phiInst = (Instruction*) joinHead->next;
    vt = getValueTable();
    int idx = vt.size()-1;
    std::string idnt;
    Instruction* getVTSave;
    while(phiInst != NULL){     // Fill out existing Instruction
        if(phiInst->op != PHI){
            phiInst = (Instruction*)  phiInst->next;
            continue;
        }
        idnt = phiInst->a->name;
        if(vt[idx].find(idnt) != vt[idx].end()){
            phiInst->b = newOp(idnt, vt[idx][idnt]);
            vt[idx].erase(idnt);
        }
        // else if(ConstVal[idx].find(idnt) != ConstVal[idx].end()){
        //     inst = newInstruction();
        //     inst->op = STORECONST;
        //     inst->InstNum = currInstNum;
        //     inst->a = newOp(idnt, ConstVal[idx][idnt]);
        //     inst->next = (INST*) joinHead->next;
        //     joinHead->next = (INST*) inst;
        //     phiInst->b = newOp(idnt, currInstNum++);
        //     ConstVal[idx].erase(idnt);
        // }
        else{
            getVTSave = getVT(idnt);
            // if(getVTSave.first >= 0){ // if it's instruction
                phiInst->b = newOp(idnt, getVTSave);
                // if(getVTSave.first == -3){
                //     addCommentInst("WARNING: variable \"" + idnt + "\" not Initialized in all path");
                // }
            // }
            // else{ // if it's constant
            //     inst = newInstruction();
            //     inst->op = STORECONST;
            //     inst->InstNum = currInstNum;
            //     inst->a = newOp(idnt, getVTSave.second);
            //     inst->next = (INST*) joinHead->next;
            //     joinHead->next = (INST*) inst;
            //     phiInst->b = newOp(idnt, currInstNum++);
                
            // }
        }
        phiInst->InstNum = incCurrInstNum();
        phiInst = (Instruction*) phiInst->next;
    }
    // ******************* Sub Value Table *******************
     vt = getValueTable();
    for(std::pair<std::string, Instruction*> i : vt[vt.size()-1]){
        getVTSave = getPrevVT(i.first);
        // if(getVTSave >= 0){
            joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, getVTSave));
        // }
        // else{
        //     inst = newInstruction();
        //     inst->op = STORECONST;
        //     inst->InstNum = currInstNum;
        //     inst->a = newOp(idnt, getVTSave.second);
        //     joinTail->next = (INST*) inst;
        //     joinTail = inst;

        //     joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, currInstNum++));
            
        // }
        joinTail->InstNum = incCurrInstNum();
    }
    // ******************* Sub Const Val *******************
    // for(std::pair<std::string, int> i: ConstVal[ConstVal.size()-1]){
    //     // If value stores a constant,
    //     //  Store it into instruction
    //     //  Update Value table with newest instruction

    //     inst = newInstruction();
    //     inst->op = STORECONST;
    //     inst->InstNum = currInstNum;
    //     inst->a = newOp(i.first, i.second);
    //     joinTail->next = (INST*) inst;
    //     joinTail = inst;

    //     insertVT(i.first, currInstNum);
    //     joinTail = addPhiInst(joinTail, newOp(i.first, currInstNum++), NULL);

    //     getVTSave = getPrevVT(i.first);
    //     if(getVTSave.first >= 0){
    //         joinTail->b = newOp(i.first, getVTSave.first);
    //     }
    //     else{
    //         inst = newInstruction();
    //         inst->op = STORECONST;
    //         inst->InstNum = currInstNum;
    //         inst->a = newOp(idnt, getVTSave.second);
    //         inst->next = joinHead->next;
    //         joinHead->next = (INST*) inst;

    //         joinTail->b = newOp(i.first, currInstNum++);
    //     }
    //     joinTail->InstNum = currInstNum++;
    // }
    


    if(!nextIs("fi")){
        throw std::invalid_argument("IfStatement expecting \"fi\" to end if");
    }
    skipNext(2);
    
    
    // Jump into fi block
    fiBlock->name = getBlock();
    endBlock->name = getBlock();
    setInstTail((INST*) endBlock->head);
    // ******************* Sub Value Table *******************
    RemoveVTLayer();

    setInWhile(inWhileSave);
    setInIf(inIfSave);
    // ========== update current layer after removeal =============
    phiInst = (Instruction*) joinHead->next;
    setJoinBlock(savedJoin);
    while(phiInst != NULL){
        if(phiInst->op != PHI){ 
            phiInst = (Instruction*) phiInst->next;
            continue;
        }
        idnt = phiInst->a->name;

        insertVT(idnt, phiInst);
        // put("insert " + idnt + ", " + std::to_string(phiInst->TYPE == INT));
        // addCommentInst("Let " + idnt + " = (" + 
        //     std::to_string(phiInst->InstNum) + ")"
        // );
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
    std::vector<std::string> labels = getWhile();
    
    // Linked Instruction;
    LinkedInst* LL[LICOUNT];

    // Init instruction blocks;
    struct InstBlock* whileBlock = newInstBlock(labels[0], incCurrInstNum());
    struct InstBlock* doBlock = newInstBlock(labels[1], incCurrInstNum());
    struct InstBlock* odBlock = newInstBlock(labels[2], incCurrInstNum());
    struct InstBlock* endBlock = newInstBlock(labels[3], incCurrInstNum());

    std::shared_ptr<Opr> whileTarget = newOp(labels[2], ((Instruction*) odBlock->head));
    std::shared_ptr<Opr> odTarget = newOp(labels[3], ((Instruction*) endBlock->head));

    struct INST* savedJoin = getJoinBlock();
    setJoinBlock( (INST*) odBlock);
    odBlock->next = (INST*) whileBlock;
    whileBlock->next = (INST*) doBlock;
    whileBlock->next2 = (INST*) endBlock;
    doBlock->next = (INST*) whileBlock;

    // Join block should be infront of everything.
    odBlock->name = getBlock();
    addInst( (INST*) odBlock);
    // struct Instruction* joinHead = (Instruction*) odBlock->head;
    // struct Instruction* joinTail = joinHead;


    // =========================================================
    // ================== INDICATE WHILE STARTED ===============
    // =========================================================
    bool inWhileSave = getInWhile();
    bool inIfSave = getInIf();

    struct Instruction* whileConditionSave = getWHileCondiction();
    struct Instruction* whileDoSave = getWhileDo();
    struct Instruction* whileJoinSave = getWhileJoin();

    setInWhile( true);
    setInIf(false);
    setWhileCondiction((Instruction*) whileBlock->head);
    setWhileDo( (Instruction*) doBlock->head);
    setWhileJoin((Instruction*) odBlock->head);


    // Jump into while block  <Check condition>
    setInstTail( whileBlock->head);
    whileBlock->name = getBlock();

    relation(odTarget);
    nextChar();

    if(!nextIs("do")){
        throw std::invalid_argument("whileStatement expecting \"do\" after relation");
    }
    skipNext(2);
    // Do start
    // ******************* Sub Value Table *******************
    InsertVTLayer();

    // Jump into do block
    doBlock->name = getBlock();
    setInstTail( doBlock->head);

    // Move to correct Linked Instruction
    for(int i = 0; i < LICOUNT; i++){
        LL[i] = getLI(i);
        getLI(i)->next2 = newLinkedInst();
        setLI(i, getLI(i)->next2);
    }

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
    inst->a = whileTarget;
    inst->InstNum = incCurrInstNum();
    addInst((INST*) inst);


    for(int i = 0; i < LICOUNT; i++){
        // LL[i]->next = newLinkedInst();
        setLI(i, LL[i]);
    }

    // Jump into od block
    endBlock->name = getBlock();
    setInstTail(endBlock->head);
    // ******************* Sub Value Table *******************
    // ******************* Insert PHI FUnction *******************
    // for(std::pair<std::string, int> i : ValueTable[ValueTable.size()-1]){
    //     joinTail = addPhiInst(joinTail, newOp(i.first, i.second), newOp(i.first, getVT(i.first).first));
        
    // }
    RemoveVTLayer();
    // Exit this while loop;
    struct Instruction* phiInst = (Instruction*) odBlock->head->next;
    std::string idt;
    setWhileJoin(whileJoinSave);
    setInWhile(inWhileSave);
    setInIf(inIfSave);
    setWhileCondiction( whileConditionSave);
    setWhileDo( whileDoSave);
    setJoinBlock( savedJoin);
    while(phiInst != NULL){
        if(phiInst->op != PHI){
            phiInst = (Instruction*) phiInst->next;
            continue;
        }
        idt = phiInst->a->name;
        insertVT(idt, phiInst);
        // addCommentInst("Let " + idt + " = (" + 
        //     std::to_string(phiInst->InstNum) +")"
        // );

        phiInst = (Instruction*) phiInst->next;
    }
}

void returnStatement(){ // only call when expression is guaranteed
    std::cout << "RETURN STATEMENT" << std::endl;
    std::pair<std::string, Instruction*> expr = expression();
    getFuncHeadInst()->b = newOp(expr.first, expr.second);
    // std::cout << "Return *" <<expr.first<< "*" << expr.second->InstNum<<"*"<< std::endl;
    // sleep(3);

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
        if(getCurr() != ';' && !endStatSequence()){
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
    while(getCurr() == ';'){
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
    std::string name = ident();
    declareVar(name);

    nextChar();
    while(getCurr() == ','){
        next();
        name = ident();
        declareVar(name);
        nextChar();
    }
    nextChar();
    if(getCurr() != ';'){
        throw std::invalid_argument("VarDecl expecting \";\" after indent(s)");
    }
    next();
}

void funcDecl(){ 
    std::cout << "FUNCDECL" << std::endl;
    std::string functionName = ident();
    std::vector<std::string> params = formalParam(); 

    std::vector<std::unordered_map<std::string, Instruction*> > savedVT = getValueTable();
    std::vector<std::unordered_map<std::string, Instruction*> > newVT;
    setValueTable(newVT);
    InsertVTLayer();

    INST* savedInstTail = getInstTail();
    std::pair<INST*, int> declFunReturn = declareFunction(functionName, getBlock(), params);
    setInstTail(declFunReturn.first);
    ((Instruction*) getInstTail())->b = newOp("return", NULL);
    setFuncHeadInst( (Instruction*) getInstTail());
    nextChar();
    if(getCurr() != ';'){
        throw std::invalid_argument("FuncDecl expecting \";\" after formalParam");
    }
    next();
    funcBody();
    nextChar();
    if(getCurr() != ';'){
        throw std::invalid_argument("FuncDecl expecting \";\" after funcBody");
    }
    next();
    Instruction* inst = newInstruction();
    inst->InstNum = incCurrInstNum();
    inst->a = newOp("-", newInstInt(declFunReturn.second));
    inst->op = RET;
    addInst((INST*) inst);
    setInstTail( savedInstTail);
    DeleteCurrVT(); //**********
    setValueTable(savedVT);
    // PrintTable(); //**********
}

std::vector<std::string> formalParam(){
    std::cout << "FORMAL PARAM" << std::endl;
    std::vector<std::string> params;
    nextChar();
    if(getCurr() != '('){
        throw std::invalid_argument("formalParam expecting \"(\" before ident");
    }
    next();
    nextChar();
    if(getCurr() != ')'){
        params.push_back(ident());
        nextChar();
        while(getCurr() == ','){
            next();
            params.push_back(ident());
            nextChar();
        }
    }
    nextChar();
    next();
    return params;
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
    if(getCurr() != '{'){
        throw std::invalid_argument("FuncBody expecting \"{\" before statSequence");
    }
    next();
    nextChar();
    if(getCurr() != '}'){
        statSequence();
    }
    if(getCurr() != '}'){
        throw std::invalid_argument("FuncBody expecting \"}\" after statSequence");
    }
    next();
}

void computation(){
    std::cout << "COMPUTATION" << std::endl;
    nextChar();

    if(getCurr() == -1) return;
    std::string s = ident();
    if(s.compare("main") != 0){
        throw std::invalid_argument("Computation expecting \"main\" at the top of the file");
    }
    
    nextChar();
    while(getCurr() != '{'){
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
    if(getCurr() != '}'){
        std::cout << getCurr() << std::endl;
        throw std::invalid_argument("Computation expecting \"}\" to end statSequence");
    }
    next(); // eat '}'
    nextChar();
    if(getCurr() != '.'){
        throw std::invalid_argument("Computation expecting \".\" to end computation");
    }
    next(); // eat '.', which is end of computation;
    // Instruction* inst = newInstruction();
    // inst->InstNum = incCurrInstNum();
    // inst->a = newOp("#", newInstInt(0));
    // inst->op = RET;
    // addInst((INST*) inst);
    addCommentInst("END.");
}


