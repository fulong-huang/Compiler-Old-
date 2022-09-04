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
        result.first = ""; // constant
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
            funcCall();     // ------------- change result -------------
        }
        else{                   // varRef
            std::string idt = ident();
            std::pair<int, int> val = varRef(idt);
            if(val.first == -1){
                result.first = "";
                result.second = val.second;
            }
            else if(val.first == -2){
                result.first = idt;
                result.second = -2;
            }
            else{
                result.first = "-";
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
        constLHS = (lhs.first == "");
        constRHS = (rhs.first == "");
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
                inst->b = newOp(rhs.first, rhs.second);

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
                inst->b = newOp("", lhs.second);
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
                inst->b = newOp(rhs.first, rhs.second);

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
        constLHS = (lhs.first == "");
        constRHS = (rhs.first == "");
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
                inst->b = newOp(lhs.first, lhs.second);
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
                inst->b = newOp(rhs.first, rhs.second);
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
        if(CURR == '-'){ // ================ SUB =====================
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
                inst->op = SUB;
                inst->a = newOp("-", 0);
                inst->b = newOp(rhs.first, rhs.second);
                addInst((INST*) inst);

                //  result + (lhs)
                inst = newInstruction();
                inst->InstNum = currInstNum;
                inst->op = ADDI;
                inst->a = newOp("-", currInstNum-1);
                inst->b = newOp("", lhs.second);
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
                inst->b = newOp(rhs.first, rhs.second);
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

    bool constLHS = (lhs.first == "");
    bool constRHS = (rhs.first == "");
    

    struct Instruction* inst = newInstruction();
    bool neg = false;
    if(lhs.first == ""){
        // if two constant: (1 > 2)
        if(rhs.first == ""){
            inst->op = LABEL;
            inst->InstNum = 100;
            inst->a = newOp("___Comparing two constant", -1);
            addInst((INST*) inst);
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
            inst->op = NEG;
            inst->a = newOp(lhs.first, lhs.second);
            inst->InstNum = currInstNum++;
            addInst((INST*) inst);

            inst = newInstruction();

            std::pair<std::string, int> s = lhs;
            lhs = rhs;
            rhs = s;

            neg = true;
        }
    }

    if(rhs.first == ""){
        inst->op = CMPI;
        inst->a = newOp(lhs.first, lhs.second);
        inst->b = newOp(rhs.first, rhs.second);
        inst->InstNum = currInstNum++;
        addInst((INST*) inst);

        switch(op){
            case EQ: // branch when not equal
                if(neg) inst->op = BEQ;
                else inst->op = BNE;
                break;
            case NE:
                if(neg) inst->op = BNE;
                else inst->op = BEQ;
                break;
            case GT:
                if(neg) inst->op = BGT;
                else inst->op = BLE;
                break;
            case GE:
                if(neg) inst->op = BGE;
                else inst->op = BLT;
                break;
            case LT:
                if(neg) inst->op = BLT;
                else inst->op = BGE;
                break;
            case LE:
                if(neg) inst->op = BLE;
                else inst->op = BGT;
                break;
            default:
                throw std::invalid_argument("Unknow relOp\n");
        }
        inst->a = newOp("", currInstNum-1);
        inst->b = newOp(target, -1);
        inst->InstNum = currInstNum++;
        addInst((INST*) inst);
        return;
    }

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
    inst->a = newOp("", currInstNum-1);
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
    if(val.first == ""){ 
        insertCV(name, val.second);
        
    }
    else{
        put("*** ASSIGN INST NUMBER " +
            std::to_string(val.second) + " TO "
            + name + " ***");
        
        insertVT(name, val.second);
    }
}

void funcCall(){
    std::cout << "FUNCALL" << std::endl;
    nextChar();
    ident();
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
}

void ifStatement(){
    std::cout << "IF" << std::endl;

    std::string* labels = getIf();

    // Init instruction blocks;
    struct InstBlock* ifBlock = newInstBlock(labels[0]);
    struct InstBlock* thenBlock = newInstBlock(labels[1]);
    struct InstBlock* elseBlock = newInstBlock(labels[2]);
    struct InstBlock* fiBlock = newInstBlock(labels[3]);

    // Connect instruction Blocks;
    ifBlock->next = (INST*) thenBlock; 
    ifBlock->next2 = (INST*) elseBlock;
    thenBlock->next = (INST*) fiBlock; 
    elseBlock->next = (INST*) fiBlock; 

    // Save JoinBlock (from outer block)
    //  Replace JoinBlock with current joinBlock (fiBlock).
    struct INST* savedJoin = JoinBlock;
    JoinBlock = (INST*) fiBlock;
    // State If statement started;
    int saveStatementType = statementType;
    statementType = 1;

    // Jump into if block
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

    statSequence();
    nextChar();

    // Else statement start here. (else)

    // If statement need to skip it
    struct Instruction* inst = newInstruction();
    inst->op = BRA;
    inst->a = newOp(fiBlock->name, -1);
    inst->InstNum = currInstNum++;
    addInst((INST*) inst);


    if(nextIs("else")){
        // eat "else"
        skipNext(4);
        statSequence();
    }
    nextChar();

    // Jump into else block
    InstTail = elseBlock->head;


    // End of If statement. (fi;)

    if(!nextIs("fi")){
        throw std::invalid_argument("IfStatement expecting \"fi\" to end if");
    }
    skipNext(2); 
    
    // Jump into fi block
    InstTail = fiBlock->head;

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

    whileBlock->next = (INST*) doBlock;
    whileBlock->next2 = (INST*) odBlock;
    doBlock->next = (INST*) whileBlock;


    // Jump into while block
    addInst( (INST*) whileBlock);
    InstTail = whileBlock->head;

    relation(labels[2]);
    nextChar();

    if(!nextIs("do")){
        throw std::invalid_argument("whileStatement expecting \"do\" after relation");
    }
    skipNext(2);
    // Do start

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
    InstTail = odBlock->head;

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
    nextChar(); // unnecessary
    ident();
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
                throw std::invalid_argument("Computation UNKNOWN TOKEN, expecting keyword \"function\"");
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


