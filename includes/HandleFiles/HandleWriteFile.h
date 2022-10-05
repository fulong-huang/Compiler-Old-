#include <iostream>
#include <string>
#include <fstream>

std::ofstream ofile;
int openWriteFile(std::string fileName);
void closeWriteFile(std::string name);

void put(std::string instruction);
void addLabel(std::string label);


