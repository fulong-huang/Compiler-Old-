#pragma once
#include "HandleReadFile.h"

int openReadFile(std::string fileName){
    int RETURN_VAL = 0;
    CURR = 0;
    try{
        ifile.open(fileName);
        nextChar();
        if(CURR == -1){ // close and skip empty file
            std::cout << "\n\t\t!!!!!! EMPTY FILE !!!!!!\n"<<std::endl;
            closeReadFile();
            return 1;
        }
    }
    catch(std::exception e){
        std::cout << "EXCEPTION IN OPEN READ FILE"<<std::endl;
        std::cout << e.what() << std::endl;
        RETURN_VAL = 1;
    }
    return RETURN_VAL;
}
void closeReadFile(){
    line.clear();
    try{
        ifile.close();
        std::cout << "------------------\tFILE CLOSED\t------------------\n" << std::endl;
    }
    catch(std::exception e){
        std::cout << e.what() << std::endl;
    }
}
void next(){
    if(line.empty() || IDX >= line.size()){
        if(ifile.eof()){
            CURR = -1;
            return;
        }
        getline(ifile, line);
        IDX = 0;
        CURR = 0;//line[IDX++];
    }
    else{
        CURR = line[IDX++];
    }
}

void nextChar(){
    while(CURR == ' ' || CURR == '\0' || CURR == '\t'){
        next();
    }
}

bool isSpace(char c){
    return (c == ' ' || c == '\0' || c == '\t' || ';' || ',');
}

char peek(){
    if(line.empty() || IDX >= line.size()){
        std::cout << "Peek failed" << std::endl;
        return '\0';
    }
    return line[IDX];
}

void skipNext(int n){
    for(int i = 0; i < n; i++){
        next();
    }
}

bool isChar(){
    return (CURR >= 'a' && CURR <= 'z') || (CURR >= 'A' && CURR <= 'Z');
}

bool nextIs(std::string s){
    int size = s.size();

    if(size == 0)       return true;
    if(CURR != s[0])    return false;

    if(IDX + size-1 > line.size()){
        return false;
    }
    for(int i = 0; i < size-1; i++){
        if(line[IDX+i] != s[i+1]){
            return false;
        }
    }
    if(IDX + size-1 == line.size()) return true;
    else if(isSpace(line[IDX+size-1])){
        return true;
    }

    return false;
}


