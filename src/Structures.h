#pragma once
#include <string>
#include <iostream>

#ifndef OPTEXT_H
#define OPTEXT_H
static const std::string opText[] = {
    // [0] for ADD
    "add","sub","mul","div","mod","cmp","or","and","bic","xor","lsh","ash","chk",
    // [13] for ADDI
    "addi","subi","muli","divi","modi","cmpi","ori","andi","bici","xori","lshi","ashi","chki",
    "ldw","ldx","pop","stw","stx","psh",
    // [32] FOR BEQ
    "beq","bne","blt","bge","ble","bgt","bsr","jsr","ret","rdd","wrd","wrh","wrl", 
    "bra", "neg", "phi", "const", "move", "label", "comment", "return", "store",
    
    "ifloop", "whileloop",
    };
#endif

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
    MOVE,


    LABEL,
    COMMENT,
    RETURN,
    STORE,

    NLL,
    FUNC,
    // IFLOOP,
    // WHILELOOP
};

enum LIidx{
    lADD, lADDI,
    lSUB, lSUBI, 
    lMUL, lMULI,
    lDIV, lDIVI,
    lCMP, lCMPI,



    LICOUNT, // total number item in this enum (exclude this)
};

/*
    Type cast instruction and instBlock into INST
*/
struct Opr;

struct INST
{
    InstType TYPE;
    INST* next;

    ~INST();
};
// Single instruction
struct Instruction
{
    InstType TYPE;// = SINGLE;
    INST* next;
    int InstNum;
    Mnemonic op;
    std::shared_ptr<Opr> a;
    std::shared_ptr<Opr> b;

    ~Instruction();
};

// Block of instruction
struct InstBlock
{
    InstType TYPE;// = BLOCK;
    INST* next;  // Fall-through
    INST* next2; // Branch
    INST* head; // first inst in block;
    std::string name;

    ~InstBlock();
};

struct InstInt
{
    InstType TYPE;
    int num;

    ~InstInt();

};



struct LinkedInst{
    struct Instruction* inst;
    struct LinkedInst* next;
    struct LinkedInst* next2;

    ~LinkedInst();
};

struct Opr{
    std::string name;
    Instruction* inst;

    ~Opr();

};

struct Instruction* newInstInt(int n);
struct std::shared_ptr<Opr> newOp(std::string name, Instruction* inst);

struct Instruction* newInstruction();
struct InstBlock* newInstBlock(std::string blockName, int n);
struct LinkedInst* newLinkedInst();
