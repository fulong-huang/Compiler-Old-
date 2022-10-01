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
    "bra", "neg", "phi", "const",
    
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



struct InstLinkedList
{
    struct Instruction* inst;
    struct Instruction* next;
};

struct LinkedInst{
    struct Instruction* inst;
    struct Instruction* next;
};

struct Opr{
    std::string name;
    Instruction* inst;
};

struct Opr* newOp(std::string name, Instruction* inst){
    struct Opr* result = new struct Opr();
    result->name = name;
    result->inst = inst;
    return result;
}
struct Instruction* newInstInt(int n){
    InstInt* iint = new struct InstInt();
    iint->TYPE = INT;
    iint->num = n;
    return (Instruction*) iint;
}

