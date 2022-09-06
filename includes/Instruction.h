#pragma once

int currInstNum;
struct INST *InstHead, *InstTail;

bool InMain;
struct INST *JoinBlock;

void InitInstruction();

struct Instruction* newInstruction();
struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);
void addLabelInst(std::string labl);
void addCommentInst(std::string comt);

struct Instruction* addPhiInst(struct Instruction* inst, struct Opr* a, struct Opr* b);
void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run
