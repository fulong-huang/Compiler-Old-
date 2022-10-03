#pragma once
#include <string>
#include <iostream>

static const std::string opText[] = {
    // [0] for ADD
    "add","sub","mul","div","mod","cmp","or","and","bic","xor","lsh","ash","chk",
    // [13] for ADDI
    "addi","subi","muli","divi","modi","cmpi","ori","andi","bici","xori","lshi","ashi","chki",
    "ldw","ldx","pop","stw","stx","psh",
    // [32] FOR BEQ
    "beq","bne","blt","bge","ble","bgt","bsr","jsr","ret","rdd","wrd","wrh","wrl", 
    "bra", "neg", "phi", "const", "label", "comment",
    
    "ifloop", "whileloop", 
    };


enum InstType{
    // Single instruction
    SINGLE,

    // Multiple instruction
    BLOCK,

    INT, // just a number, not an instruction
};

enum Ops{
    EQ,
    NE,
    LT,
    GE,
    LE,
    GT
};

enum Mnemonic{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    CMP,
    OR,
    AND,
    BIC,
    XOR,
    LSH,
    ASH,
    CHK,

    ADDI,
    SUBI,
    MULI,
    DIVI,
    MODI,
    CMPI,
    ORI,
    ANDI,
    BICI,
    XORI,
    LSHI,
    ASHI,
    CHKI,
    
    LDW,
    LDX,
    POP,
    STW,
    STX,
    PSH,

    BEQ,
    BNE,
    BLT,
    BGE,
    BLE,
    BGT,
    BSR,
    JSR,
    RET,

    RDD,
    WRD,
    WRH,
    WRL,

    BRA,
    NEG,

    PHI,
    CONST,


    LABEL,
    COMMENT,
    NLL,
    // IFLOOP,
    // WHILELOOP
};

enum LIidx{
    lADD, lADDI,
    lSUB, lSUBI, 
    lMUL, lMULI,
    lDIV, lDIVI,



    LICOUNT, // total number item in this enum (exclude this)
};

/*
    Type cast instruction and instBlock into INST
*/
struct INST
{
    InstType TYPE;
    INST* next;
};
struct Opr;
// Single instruction
struct Instruction
{
    InstType TYPE;// = SINGLE;
    INST* next;
    int InstNum;
    Mnemonic op;
    Opr* a;
    Opr* b;
};

// Block of instruction
struct InstBlock
{
    InstType TYPE;// = BLOCK;
    INST* next;  // Fall-through
    INST* next2; // Branch
    INST* head; // first inst in block;
    std::string name;
};

struct InstInt
{
    InstType TYPE;
    int num;
};



struct LinkedInst{
    struct Instruction* inst;
    struct LinkedInst* next;
    struct LinkedInst* next2;
};

struct Opr{
    std::string name;
    Instruction* inst;
};

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
