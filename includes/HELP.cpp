
#include <iostream>
#include <string>

int ifNum, doNum, mainNum;

bool VTcmp(std::pair<std::string, int> a, std::pair<std::string, int> b) 
{ return a.second < b.second; }

bool CTcmp(std::pair<int, int> a, std::pair<int, int> b) 
{ return a.second < b.second; }

std::string* getIf(){
    std::string* result = new std::string[4];
    result[0] = "IF_" + std::to_string(ifNum);
    result[1] = "THEN_" + std::to_string(ifNum);
    result[2] = "ELSE_" + std::to_string(ifNum);
    result[3] = "JOIN_IF_" + std::to_string(ifNum);
    ifNum++;
    return result;
}
std::string* getWhile(){
    std::string* result = new std::string[3];
    result[0] = "WHILE_" + std::to_string(doNum);
    result[1] = "DO_" + std::to_string(doNum);
    result[2] = "JOIN_WHILE_" + std::to_string(doNum);
    doNum++;
    return result;
}


void InitHelp(){
    doNum = ifNum = doNum = mainNum = 0;
}

