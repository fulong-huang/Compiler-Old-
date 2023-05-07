# SSA format IR diagram

<br/>

## Description:
&emsp;&emsp;This was my school project, with no starter code, with loose instructions/requirement, and built from the ground up, using only standard libraries. 
It reads inputs from **tests** folder, which is written in a subset of the language “*Tiny*”, the syntax of the language can be found in the **EBNF** file. The outputs are intermediate representations of Static-Single Assignment form compiler design, written in DOT language, which Graphviz can read and generate graphs. Output files will locate inside the folder **outputs**, and each input file will map into an output file with the exact same name.
<br/><br/>
&emsp;&emsp;A Static-Single Assignment (SSA) compiler design keeps track of the version numbers of each variable, that represent the instruction number those variables were assigned to. The information about the active time of the instructions helps better determine how to find a more efficient way to allocate registers.
<br/><br/>
&emsp;&emsp;I also perform some other optimizations such as pre-computing operations of constants, common subexpression elimination, and copy propagation.
<br/><br/>
Here is video demonstration of an example: **https://youtu.be/nKmsC9gf2Bg**

## How to RUN:
&emsp;&emsp;Create new or modify existing files inside tests, and make sure it matches the exact syntax provided in the **EBNF** file.
<br/>
&emsp;&emsp;Run the command: 
<br/>
```
    ./out
```
<br/>If above command does not work, try the following commands:
```
    ./make
    ./out
```

