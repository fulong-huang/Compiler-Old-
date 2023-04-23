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

struct Instruction* addPhiInst(struct Instruction* inst, std::shared_ptr<Opr> a, std::shared_ptr<Opr> b);
void PrintInst(struct INST* currInst);
void PrintInstBlock(struct INST* instBlock); 

void updateInst(std::shared_ptr<Opr> oldOp, Instruction* newInst);
std::shared_ptr<Opr> updateOp(std::shared_ptr<Opr> thisOp, std::shared_ptr<Opr> targetOp, Instruction* newInst);
void updateIndivInst(INST* instruction, std::shared_ptr<Opr> oldOp, Instruction* newInst);
void updateBlockInst(INST* blockInst, std::shared_ptr<Opr> oldOp, Instruction* newInst);

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
