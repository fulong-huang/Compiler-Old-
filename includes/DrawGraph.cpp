#include "DrawGraph.h"

void InitGraph(){
    graph = "digraph G { \nMAIN [shape=record, label=\"<b>MAIN | {";
    graphConnection = "";
    prevLabel = "MAIN";
}