# PLD_Compilateur 

## Description

C-like compiler, programmed in C++ and based on ANTLR4. 

The compiler takes a C-program as input and outputs the corresponding x86 ASM code. 

## List of features supported by our compiler ifcc

- Comments and preprocessor directives are ignored 
- 32 bits int data type can be used 
- Return instruction
- Variable declaration
- Multiple variable declaration
- Variable assignement  
- Variable initialization
- Memory address management (Symbol table)
- Arithmetic operations (+, -, *, /)
- Bit operations (&, |, ^)
- Comparison operations (==, !=, <, >)
- Unary operations (!, -)
- Expressions (multiple variables or constants combined with operators)
- Order of operations, operators associativity
- Parentheses around expression  
- Static checking: the same variable (same type and same name) can not be declared multiple times
- Static checking: each declared variable is used at least once
- IR architecture (IRInstr, BasicBlock, CFG)

**Clarification**: Our symbol table is a map where the key is the name of the variable. We don't take into account the type (it will be done in the second sprint). Constants can also be stored in our symbol table, we give them a name that starts with a '$'. 

## Quick description of the code base 

Main components of PLD-COMP base code:
- **github \ workflows** directory: It contains a yml file that automates the tests run before merging to master on GitHub (CI/CD pipeline). 
- **compiler** directory: It contains the ifcc.g4 file, the back-end of our compiler, and some scripts to build the compiler.  
The ifcc.g4 defines a grammar for the parser generated by ANTLR4. It is used by the C++ files CodeGenVisitor to visit each node of the input C-program (in a particular order).  
The back-end of our compiler consists of an IR architecture (IRInstr, BasicBlock, CFG) that translates C-instructions to x86 ASM code. IRInstr functions are called by CodeGenVisitor C++ code. 
- **tests directory**: It contains all the tests files used to validate the features supported by our compiler, and the scripts used to launch these tests. 

## Build 

**WARNING**: To be able to use our compiler on **INSA Lyon Linux machines**, please go to `PLD-COMP` directory then run:
```sh
$ ./install-antlr.sh
```
To build the binary, please go to `PLD-COMP/compiler` directory and run:
```sh
$ make
```


If you want to use our compiler on your **personal machine**, please follow these instructions: 

ANTLR4 is pre-compiled on Ubuntu 20.04. For any Ubuntu version > 20, you should install these required packages:
```sh
$ sudo apt install antlr4 libantlr4-runtime-dev default-jdk
```

To build the binary, please go to `PLD-COMP/compiler` directory and run:
```sh
$ ./runmake_ubuntu.sh
```

Once built, an executable `ifcc` should appear in current directory.

## Run 

You should make sure that you have correctly built the project beforehand. 

To get the x86 ASM code of your C-program, please go to `PLD-Comp/compiler` directory and run:
```sh
$ ./ifcc your_input_file.c 
```

## Tests

You should make sure that you have correctly built the project beforehand. 

To launch all the tests, please go to `PLD-COMP/tests` and run:
```sh
$ python3 ifcc-test.py testfiles/ 
```

You can target specific tests by replacing `testfiles/` with a specific path. 

The script uses our binary (ifcc) and an official C-compiler (GCC 9.4.0 in our case) to compile the files of the tests directory. It then compares the returned results to validate or not the test. 

For each test, there are 5 possible outcomes:
- TEST OK,
- TEST FAIL (your compiler accepts an invalid program),
- TEST FAIL (your compiler rejects a valid program),
- TEST FAIL (your compiler produces incorrect assembly),
- TEST FAIL (different results at execution).

**We did our tests on Ubuntu 20.04 with the following GCC version: 9.4.0**  
**All tests should be OK.**   
**Some tests may fail if you use a more recent version of GCC.**


## Authors
Aymane GHANAM \
Ghali KETTANI \
Nour NOUNAH \
Najwa ELOUENASS \
Mohamed Malek MAHJOUB \
Mohamed Sayed HMIDI \
Malak FAWZI \
Taha NABIL

Hexanome H4232