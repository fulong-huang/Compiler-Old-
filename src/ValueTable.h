#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Structures.h"
#include "Parser.h"


void InitVT();
void DestroyVT();

void declareVar(std::string ident);

void insertVT(std::string ident, Instruction* inst);
void insertCT(std::string ident, int val);
void insertCT(std::string ident, Instruction* inst);
Instruction* createConst(int val);

Instruction* getVT(std::string ident);
Instruction* getPrevVT(std::string ident);

Instruction* getCT(int instNum); // find value of instNum, type cast to Instruction*

// void insertCV(std::string ident, int inst);
// std::pair<bool, int> getCV(std::string ident);

void InsertVTLayer();
void RemoveVTLayer();
void ClearLastLayer();

bool getInWhile();
bool getInIf();
void setInWhile(bool b);
void setInIf(bool b);

std::vector<std::unordered_map<std::string, Instruction*> > getValueTable();
void setValueTable(std::vector<std::unordered_map<std::string, Instruction*> > newVT);



struct Instruction* getWhileJoin();
void setWhileJoin(struct Instruction* newWhileJoin);

void DestroyTable(std::vector<std::unordered_map<std::string, Instruction*> > table);


void PrintTable();

