#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


/**
 * Save the instruction number of the value givin name stored.
 * ValueTable[ VarName ] = InstNumber of newest version 
 */

// ValueTable:      <ident, instNumber>
//  var x, y; (inst 25) x = Call func(); (inst 26) y = Call func();
// x and y will be stored in ValueTable, ValueTable = {[x:25], [y:26]}
//  var z; z = x+y;                      ValueTable = {[x:25], [y:26], [z:27]}      1 instruction created
std::vector<std::unordered_map<std::string, Instruction*> > ValueTable;

// ConstVal:        <ident, storedValue>
//  var x, y; x = 5; y = 3;
// x and y will be stored in ConstVal, ConstVal = {[x:5], [y:3]}
//  var z; z = x+y;                    ConstVal = {[x:5], [y:3], [z:8]}             0 instruction created
// std::vector<std::unordered_map<std::string, int> > ConstVal; // store instNum of constant;


std::unordered_map<int, Instruction*> ConstTable;

int currConstNum;

bool InWhile, InIf;
struct Instruction* WhileJoin;

void InitVT();

void declareVar(std::string ident);

void insertVT(std::string ident, Instruction* inst);
void insertCT(std::string ident, int val);
Instruction* createConst(int val);

Instruction* getVT(std::string ident);
Instruction* getPrevVT(std::string ident);

Instruction* getCT(int instNum); // find value of instNum, type cast to Instruction*

// void insertCV(std::string ident, int inst);
// std::pair<bool, int> getCV(std::string ident);

void InsertVTLayer();
void RemoveVTLayer();
void ClearLastLayer();
