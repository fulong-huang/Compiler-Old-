#pragma once
int currInstNum;
struct INST *InstHead, *InstTail;

bool InMain;
struct INST *JoinBlock;

std::string stringIndent = "";

struct Instruction *WhileCondition, *WhileDo;

void InitInstruction();

struct Instruction* newInstruction();
struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);
void addLabelInst(std::string labl);
void addCommentInst(std::string comt);

struct Instruction* addPhiInst(struct Instruction* inst, struct Opr* a, struct Opr* b);
void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run

void updateInst(Opr* oldOp, int newNum);
void updateOp(Opr* thisOp, Opr* targetOp, int num);
void updateIndivInst(INST* instruction, Opr* oldOp, int newNum);
void updateBlockInst(INST* blockInst, Opr* oldOp, int newNum);
