
out: main.o handleReadFile.o handleWriteFile.o drawGraph.o function.o HELP.o \
		instruction.o parser.o structures.o valueTable.o

	g++ main.o handleReadFile.o handleWriteFile.o drawGraph.o function.o HELP.o \
		instruction.o parser.o structures.o valueTable.o -o out

	rm *.o

main.o: main.cpp
	g++ -c main.cpp

handleReadFile.o: src/HandleFiles/HandleReadFile.h src/HandleFiles/HandleReadFile.cpp
	g++ -c src/HandleFiles/HandleReadFile.cpp

handleWriteFile.o: src/HandleFiles/HandleWriteFile.h src/HandleFiles/HandleWriteFile.cpp
	g++ -c src/HandleFiles/HandleWriteFile.cpp

drawGraph.o: src/DrawGraph.cpp src/DrawGraph.h
	g++ -c src/DrawGraph.cpp

function.o: src/Function.cpp src/Function.h
	g++ -c src/Function.cpp

HELP.o: src/HELP.cpp
	g++ -c src/HELP.cpp

instruction.o: src/Instruction.cpp src/Instruction.h
	g++ -c src/Instruction.cpp

parser.o: src/Parser.cpp src/Parser.h
	g++ -c src/Parser.cpp

structures.o: src/Structures.cpp
	g++ -c src/Structures.cpp

valueTable.o: src/ValueTable.cpp src/ValueTable.h
	g++ -c src/ValueTable.cpp

clean:
	rm *.o out

