#pragma once

int currInstNum;
struct INST *InstHead, *InstTail;

int statementType = 0; // 0: main,  1: if,     2: while
struct INST *JoinBlock;

void InitInstruction();

struct Instruction* newInstruction();
struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);

void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run
