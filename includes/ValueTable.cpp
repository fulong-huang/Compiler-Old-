#include "ValueTable.h"


void declareVar(std::string ident){
    insertVT(ident, 0);
}


void insertVT(std::string ident, int inst){
    ValueTable[ident] = inst;
    std::unordered_map<std::string, int>::const_iterator result = ConstVal.find(ident);
    if(result != ConstVal.end()){
        ConstVal.erase(ident);
    }
}

std::pair<int, int> getVT(std::string ident){
    std::pair<int, int> result;
    std::unordered_map<std::string, int>::const_iterator v = ValueTable.find(ident);
    std::unordered_map<std::string, int>::const_iterator v2 = ConstVal.find(ident);
    if(v != ValueTable.end()){
        result.first = ValueTable[ident];
    }
    else if(v2 == ConstVal.end()){
        // Value not exist, return -2;
        result.first = -2;
    }
    else{
        result.first = -1;
        result.second = ConstVal[ident];
    }
    return result;
}


void insertCV(std::string ident, int inst){
    ConstVal[ident] = inst;
    std::unordered_map<std::string, int>::const_iterator result = ValueTable.find(ident);
    if(result != ValueTable.end()){
        ValueTable.erase(ident);
    }
}



void InitVT(){
    ValueTable.clear();
    ConstVal.clear();
    currInstNum = 2;
}