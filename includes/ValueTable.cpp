#include "ValueTable.h"


void declareVar(std::string ident){
    insertVT(ident, -3);
}


void insertVT(std::string ident, int inst){
    if(InMain){
        std::unordered_map<std::string, int>::const_iterator cv = ConstVal.find(ident);
        ValueTable[ident] = inst;
        if(cv != ConstVal.end()){
            ConstVal.erase(ident);
        }
    }
    else{
        std::unordered_map<std::string, int>::const_iterator scv = SubConstVal.find(ident);
        std::unordered_map<std::string, int>::const_iterator svt = SubValueTable.find(ident);
        if(svt != SubValueTable.end()){
            // value exist in sub value table
            if(SubValueTable[ident] == inst){
                // if instruction number does not change
                return;
            }
        }
        SubValueTable[ident] = inst;
        if(scv != SubConstVal.end()){
            SubConstVal.erase(ident);
        }
    }
}

std::pair<int, int> getVT(std::string ident){
    std::pair<int, int> result;
    std::unordered_map<std::string, int>::const_iterator vt = ValueTable.find(ident);
    std::unordered_map<std::string, int>::const_iterator svt = SubValueTable.find(ident);
    std::unordered_map<std::string, int>::const_iterator cv = ConstVal.find(ident);
    std::unordered_map<std::string, int>::const_iterator scv = SubConstVal.find(ident);
    
    if(InMain){
        if(vt != ValueTable.end()){
            result.first = ValueTable[ident];
        }
        else if(cv != ConstVal.end()){
            result.first = -1;
            result.second = ConstVal[ident];
        }
        else{
            // Value not exist, return -2;
            result.first = -2;
        }
    }
    else{
        if(svt != SubValueTable.end()){
            result.first = SubValueTable[ident];
        }
        else if(scv != SubConstVal.end()){
            result.first = -1;
            result.second = SubConstVal[ident];
        }
        else if(vt != ValueTable.end()){
            result.first = ValueTable[ident];
        }
        else if(cv != ConstVal.end()){
            result.first = -1;
            result.second = ConstVal[ident];
        }
        else{
            // Value not exist, return -2;
            result.first = -2;
        }
    }
    return result;
}


void insertCV(std::string ident, int inst){
    std::unordered_map<std::string, int>::const_iterator vt = ValueTable.find(ident);
    if(InMain){
        ConstVal[ident] = inst;
        if(vt != ValueTable.end()){
            ValueTable.erase(ident);
        }
    }
    else{ 
        // If two constant merge:
        //  Store both number,
        //  Phi function on the new instruction number
        std::unordered_map<std::string, int>::const_iterator svt = SubValueTable.find(ident);
        std::unordered_map<std::string, int>::const_iterator scv = SubConstVal.find(ident);
        if(scv != SubConstVal.end()){
            // if constant already exist
            if(SubConstVal[ident] == inst){
                // if it will be replaced as same number, then skip
                return;
            }
        }
        SubConstVal[ident] = inst;
        if(svt != SubValueTable.end()){
            SubValueTable.erase(ident);
        }
    }
}



void InitVT(){
    ValueTable.clear();
    ConstVal.clear();
    SubValueTable.clear();
    SubConstVal.clear();
    currInstNum = 2;
}
