#pragma once
#include "HandleWriteFile.h"


int openWriteFile(std::string fileName){
    int RETURN_VAL = 0;
    try{
        ofile.open(fileName);
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
        RETURN_VAL = 1;
    }
    return RETURN_VAL;
}
void put(std::string instruction){
    ofile  << "\t" << instruction << std::endl;
}
void addLabel(std::string label){
    ofile << label << ": " << std::endl;
}

void closeWriteFile(){
    try{
        ofile.close();
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
}
