#include <iostream>
#include <string>

#include "HandleFiles/HandleReadFile.cpp"
#include "HandleFiles/HandleWriteFile.cpp"
#include "Structures.cpp"
// #include "InstValue.cpp"
#include "ValueTable.cpp"
#include "HELP.cpp"

bool intNum;

Ops relOp();
std::string ident();
// number() reads digits, return inst number of this digit.
int number();
// varRef() reads name, return (instNum, value) of this ref.
std::pair<int, int> varRef(std::string ident);
std::pair<std::string, int> factor();
std::pair<std::string, int> term();
std::pair<std::string, int> expression();
void relation(std::string target);

void assignment();


// ****************************************
int funcCall();
// ****************************************


void ifStatement();
void whileStatement();
void returnStatement();

void statement();
void statSequence();

void varDecl();
void funcDecl();
void formalParam();
void funcBody();
void computation();





