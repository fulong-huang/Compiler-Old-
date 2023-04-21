#pragma once
#include <iostream>
#include <string>

void InitGraph();

void addToGraph(std::string content);

std::string getGraph();

std::string getGraphConnection();

void addToGraphConnection(std::string content);

std::string getPrevLabel();
void setPrevLabel(std::string label);
