#pragma once

#include <string>
#include <iostream>
#include <fstream>

int openReadFile(std::string fileName);
void closeReadFile();
void next();
void nextChar();
char peek();
void skipNext(int n);
bool isChar();
bool isSpace(char c);
bool nextIs(std::string s);


bool endStatSequence();
char getCurr();
