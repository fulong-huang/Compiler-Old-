#ifndef HANDLEREADFILE_H
#define HANDLEREADFILE_H

#include <string>
#include <iostream>
#include <fstream>

std::ifstream ifile;
std::string line;
int IDX;
char CURR;

int openReadFile(std::string fileName);
void closeReadFile();
void next();
void nextChar();
char peek();
void skipNext(int n);
bool isChar();
bool isSpace(char c);
bool nextIs(std::string s);


bool endStatSequence(){
    nextChar();
    return (CURR == '}' || nextIs("od") || nextIs("fi") || nextIs("else"));
}

#endif