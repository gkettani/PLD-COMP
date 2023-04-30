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

        BasicBlock *createTestBB();
        BasicBlock *createWhileBB();
        BasicBlock *createAfterWhileBB();

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

        /* Compte le nombre de While dans le cfg */
        int getCurrentNumberOfWhile() {
            return currentNumberOfWhile;
        }

        /* Incrémente le nombre de While dans le cfg */
        void incrementCurrentNumberOfWhile()
        {
            ++currentNumberOfWhile;
        }

        /* Décrémente le nombre de While dans le cfg */
        void decrementCurrentNumberOfWhile()
        {
            --currentNumberOfWhile;
        }

    protected:
        map<string, pair<string,int>> *variables;   /* Pointeur sur la table des symboles */
        vector<BasicBlock *> bbs;                   /* Tous les basic blocks du CFG*/
        int nbWhile = 0;
        int currentNumberOfWhile = 0;
};

#endif