#include "DrawGraph.h"

void InitGraph(){
    graph = "digraph G { \nBB0 [shape=record, label=\"<b>BB0 | {\\<CONST\\>";
    graphConnection = "";
    prevLabel = "BB0";
}