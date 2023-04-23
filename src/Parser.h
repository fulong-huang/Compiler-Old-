#pragma once
#include <iostream>
#include <string>

#include "HandleFiles/HandleReadFile.h"
#include "HandleFiles/HandleWriteFile.h"
#include "DrawGraph.h"
#include "Function.h"
#include "HELP.h"
#include "Instruction.h" 
#include "Structures.h"
// #include "InstValue.cpp"
#include "ValueTable.h"


Ops relOp();
std::string ident();
// number() reads digits, return inst number of this digit.
int number();
// varRef() reads name, return (instNum, value) of this ref.
Instruction* varRef(std::string ident);
std::pair<std::string, Instruction*> factor();
std::pair<std::string, Instruction*> term();
std::pair<std::string, Instruction*> expression();
void relation(std::shared_ptr<Opr> target);

void assignment();


// ****************************************
Instruction* funcCall();
// ****************************************


void ifStatement();
void whileStatement();
void returnStatement();

void statement();
void statSequence();

void varDecl();
void funcDecl();
std::vector<std::string> formalParam();
void funcBody();
void computation();





