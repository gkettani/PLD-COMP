#ifndef CFG_H
#define CFG_H

#include "BasicBlock.h"

// class DefFonction;

class CFG
{
    public:
        // CFG(DefFonction *ast);
        // DefFonction *ast; /**< The AST this CFG comes from */

        CFG();
        ~CFG();

        BasicBlock * createBB();
        void add_bb(BasicBlock *bb);

        // x86 code generation: could be encapsulated in a processor class in a retargetable compiler
        void gen_asm(ostream &o);
        void gen_asm_prologue(ostream &o);
        void gen_asm_epilogue(ostream &o);

        /*
        // symbol table methods
        void add_to_symbol_table(string var);
        string create_new_tempvar(Type t);
        int get_var_index(string name);
        Type get_var_type(string name);
        */

        // string IR_reg_to_asm(string reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */

        BasicBlock *current_bb;

    protected:
        // map<string, Type> SymbolType; /**< part of the symbol table  */
        // map<string, int> SymbolIndex; /**< part of the symbol table  */
        // int nextFreeSymbolIndex;      /**< to allocate new symbols in the symbol table */
        // int nextBBnumber;             /**< just for naming */

        map<string, int>* variables;
        vector<BasicBlock *> bbs; /**< all the basic blocks of this CFG*/
};

#endif