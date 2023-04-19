#ifndef CFG_H
#define CFG_H

#include "BasicBlock.h"

// class DefFonction;

class CFG
{
    public:
        CFG();
        ~CFG();

        /* Créer un basic block dans le CFG */
        BasicBlock * createBB();

        /* Ajouter un basic block au CFG */
        void add_bb(BasicBlock *bb);

        /* Ajouter le basic bloc final au CFG*/
        void add_finalBB();

        /* Génère le code assembleur x86 d'un CFG */
        void gen_asm(ostream &o);

        /* Pointeur sur le basic bloc courant */
        BasicBlock *current_bb;

        /* Pointeur sur le dernier basic bloc */
        BasicBlock *final_bb;

        /*
        // symbol table methods
        void add_to_symbol_table(string var);
        string create_new_tempvar(Type t);
        int get_var_index(string name);
        Type get_var_type(string name);
        */

        // string IR_reg_to_asm(string reg); /**< helper method: inputs a IR reg or input variable, returns e.g. "-24(%rbp)" for the proper value of 24 */

    protected:
        map<string, pair<string,int>> *variables;   /* Pointeur sur la table des symboles */
        vector<BasicBlock *> bbs;                   /* Tous les basic blocks du CFG*/
};

#endif