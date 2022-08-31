#include <iostream>
#include <string>
#include <unordered_map>
#include "Instruction.cpp" 


/**
 * Save the instruction number of the value givin name stored.
 * ValueTable[ VarName ] = InstNumber of newest version 
 */

std::unordered_map<std::string, int> ValueTable;
std::unordered_map<std::string, int> ConstVal; // store instNum of constant;
void InitVT();

void insertVT(std::string ident, int inst);
std::pair<int, int> getVT(std::string ident);

void insertCV(std::string ident, int inst);
// std::pair<bool, int> getCV(std::string ident);

