#include "DrawGraph.h"

std::string graph;
std::string graphConnection;
std::string prevLabel;

void InitGraph(){
    graph = "digraph G { \n";
    graphConnection = "";
}

void addToGraph(std::string content){
    graph += content;
}

std::string getGraph(){
    return graph;
}


std::string getGraphConnection(){
    return graphConnection;
}
void addToGraphConnection(std::string content){
    graphConnection += content;
}
std::string getPrevLabel(){
    return prevLabel;
}
void setPrevLabel(std::string label){
    prevLabel = label;
}

