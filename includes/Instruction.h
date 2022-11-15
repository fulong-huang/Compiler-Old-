#pragma once
#include "HELP.cpp"
int currInstNum;
struct INST *InstHead, *InstTail;

struct INST *JoinBlock;

std::string stringIndent = "";

struct Instruction *WhileCondition, *WhileDo;

struct LinkedInst* LinkedInstHead[LICOUNT];

struct LinkedInst* LinkedInstruction[LICOUNT];

bool eliminating;


void InitInstruction();

// struct Instruction* newInstruction();
// struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);
void addLabelInst(std::string labl);
void addCommentInst(std::string comt);

struct Instruction* addPhiInst(struct Instruction* inst, struct Opr* a, struct Opr* b);
void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run

void updateInst(Opr* oldOp, Instruction* newInst);
Opr* updateOp(Opr* thisOp, Opr* targetOp, Instruction* newInst);
void updateIndivInst(INST* instruction, Opr* oldOp, Instruction* newInst);
void updateBlockInst(INST* blockInst, Opr* oldOp, Instruction* newInst);
