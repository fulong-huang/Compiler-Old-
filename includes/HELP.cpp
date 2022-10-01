#pragma once
#include <iostream>
#include <string>

int ifNum, doNum, mainNum, blockNum;

bool VTcmp(std::pair<std::string, Instruction*> a, std::pair<std::string, Instruction*> b) 
{ return ((InstInt*)a.second)->num < ((InstInt*)b.second)->num; }

bool CTcmp(std::pair<int, int> a, std::pair<int, int> b) 
{ return a.second < b.second; }

std::string* getIf(){
    std::string* result = new std::string[5];
    result[0] = "IF_" + std::to_string(ifNum);
    result[1] = "THEN_" + std::to_string(ifNum);
    result[2] = "ELSE_" + std::to_string(ifNum);
    result[3] = "JOIN_IF_" + std::to_string(ifNum);
    result[4] = "END_IF_" + std::to_string(ifNum);
    ifNum++;
    return result;
}
std::string* getWhile(){
    std::string* result = new std::string[4];
    result[0] = "WHILE_" + std::to_string(doNum);
    result[1] = "DO_" + std::to_string(doNum);
    result[2] = "JOIN_WHILE_" + std::to_string(doNum);
    result[3] = "END_WHILE_" + std::to_string(doNum);
    doNum++;
    return result;
}

std::string getBlock(){
    return "BB"+std::to_string(blockNum++);
}

void InitHelp(){
    doNum = ifNum = doNum = mainNum = 0;
    blockNum = 1;
}

