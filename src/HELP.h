#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "Structures.h"


bool VTcmp(std::pair<std::string, Instruction*> a, std::pair<std::string, Instruction*> b);
bool CTcmp(std::pair<int, int> a, std::pair<int, int> b);
std::vector<std::string> getIf();
std::vector<std::string> getWhile();
std::string getBlock();
void InitHelp();
