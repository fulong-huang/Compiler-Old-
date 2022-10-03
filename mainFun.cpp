#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>
#include <unistd.h>
#include <vector>

#include "includes/HandleFiles/HandleReadFile.cpp"
#include "includes/HandleFiles/HandleWriteFile.cpp"
#include "includes/Parser.cpp"
int main()
{
    char thisPath[256];
    getcwd(thisPath, 256);
    DIR *dir;
    struct dirent *ent;
    char testPath[256];
    strcpy(testPath, thisPath);
    strcat(testPath, "/tests");
    char ts[256];
    std::cout << "\n********************\t"  << thisPath << 
                 "\t********************\n" << std::endl;
    if ((dir = opendir (testPath)) != NULL) {
        readdir (dir);
        readdir (dir);
        strcat(thisPath, "/outputs");
        /* print all the files and directories within directory */
        bool running = true;
        while ((ent = readdir (dir)) != NULL && running) {
            printf ("+++++++++++++++++++\t%s\t+++++++++++++++++++\n", ent->d_name);
            if(openReadFile(std::string(testPath) +'/' + std::string(ent->d_name))==0){
                // next();
                openWriteFile(std::string(thisPath) +'/' + std::string(ent->d_name));
                try{
                    // *** INIT STUFFS ***
                    InitInstruction();
                    InitVT();
                    InitHelp();
                    InitGraph();
                    
                    nextChar();
                    if(CURR == '-') {
                        next();
                        running = false;
                    }
                    // --- PROGRAM START ---
                    computation();
                    std::cout << "END COMPUTATION" << std::endl;
                    CommonSubElim();
                    std::cout << "End Common Subexpression Elimination" << std::endl;
                }
                catch(const std::exception &e){
                    std::cout << e.what() << std::endl;
                }

                // expression();

                // while(CURR != -1){
                //     std::cout <<'='<<(int)CURR<<'=' ;
                //     next();
                // }
                // std::cout <<'~' << std::endl;
                put( "---------- Printing Instructions ----------\n");
                PrintInst(InstHead);
                put("\n\n");
                put("\n"+graph+"\n\n"+graphConnection+"\n}\n\n");
                // put("\n\n------------------ Values Start Here ------------------");
                // put("Value Table size: " + std::to_string(ValueTable.size()));
                // std::vector<std::pair<std::string, Instruction*> > vt(ValueTable[0].begin(), ValueTable[0].end());
                // std::sort(vt.begin(), vt.end(), VTcmp);

                // for(std::pair<std::string, Instruction*> i : vt){
                //     put(std::to_string(((InstInt*)i.second)->num) + ", " + i.first);
                // }


                put("\n\n");
                // std::vector<std::pair<int, int> > ct(ConstTable.begin(), ConstTable.end());
                // std::sort(ct.begin(), ct.end(), CTcmp);

                // for(std::pair<std::string, int> i : ConstVal[0]){
                //     put(i.first + ", " + std::to_string(i.second));
                // }
                closeWriteFile();
                closeReadFile();
            }


        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
    return 0;
}