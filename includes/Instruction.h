#pragma once

int currInstNum;
struct INST *InstHead, *InstTail;

bool InMain;
struct INST *JoinBlock;

void InitInstruction();

struct Instruction* newInstruction();
struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);
void addLabel(std::string labl);

void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run
