#pragma once
#include <iostream>
#include <string>
#include <fstream>

int openWriteFile(std::string fileName);
void closeWriteFile(std::string name);

void put(std::string instruction);
void addLabel(std::string label);


