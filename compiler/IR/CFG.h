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

        /* Créer un basic block dans le CFG */
        BasicBlock * createBB();

        /* Ajouter un basic block au CFG */
        void add_bb(BasicBlock *bb);

        /* Génère le code assembleur x86 d'un CFG */
        void gen_asm(ostream &o);
        void gen_asm_prologue(ostream &o);
        void gen_asm_epilogue(ostream &o);

        /* Pointeur sur le basic bloc courant */
        BasicBlock *current_bb;

        /*
        // symbol table methods
        void add_to_symbol_table(string var);
        string create_new_tempvar(Type t);
        int get_var_index(string name);
        Type get_var_type(string name);
        */

        // string IR_reg_to_asm(string reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */

    protected:
        // map<string, Type> SymbolType; /**< part of the symbol table  */
        // map<string, int> SymbolIndex; /**< part of the symbol table  */
        // int nextFreeSymbolIndex;      /**< to allocate new symbols in the symbol table */
        // int nextBBnumber;             /**< just for naming */

        map<string, int> *variables; /**< Pointeur sur la table des symboles */
        vector<BasicBlock *> bbs; /**< all the basic blocks of this CFG*/
};

#endif