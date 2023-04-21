#pragma once
#include "HELP.h"
#include "Structures.h"
#include "HandleFiles/HandleWriteFile.h"
#include "HandleFiles/HandleReadFile.h"
#include "DrawGraph.h"

void InitInstruction();
void DestroyInstruction();

// struct Instruction* newInstruction();
// struct InstBlock* newInstBlock(std::string blockName);

void addInst(struct INST* inst);
void addLabelInst(std::string labl);
void addCommentInst(std::string comt);

void setInstTail(INST* newTail);
INST* getInstTail();

struct LinkedInst* getLI(int idx);
void setLI(int idx, LinkedInst* newLI);

struct Instruction* addPhiInst(struct Instruction* inst, struct Opr* a, struct Opr* b);
void PrintInst(struct INST* currInst);
struct INST* PrintInstBlock(struct INST* instBlock); // return next instruction to run

void updateInst(Opr* oldOp, Instruction* newInst);
Opr* updateOp(Opr* thisOp, Opr* targetOp, Instruction* newInst);
void updateIndivInst(INST* instruction, Opr* oldOp, Instruction* newInst);
void updateBlockInst(INST* blockInst, Opr* oldOp, Instruction* newInst);

void appendLL(Instruction* inst, LIidx n);

void CommonSubElim();
bool getEliminating();
void setEliminating(bool e);

struct INST* getInstHead();
void setInstHead(INST* newInstHead);
int getCurrInstNum();
int incCurrInstNum();


void setWhileCondiction(Instruction* newCondition);
void setWhileDo(Instruction* newDo);
struct Instruction* getWHileCondiction();
struct Instruction* getWhileDo();

struct INST* getJoinBlock();
void setJoinBlock(INST* newJoinBlock);

void InitializeInstruction();
