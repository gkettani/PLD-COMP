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

        /* Créer les basics blocks du if..else avec un nommage particulier */
        BasicBlock *createTestIfBB();
        BasicBlock *createThenBB();
        BasicBlock *createElseBB();
        BasicBlock *createAfterIfBB();

        /* Créer les basics blocks du while avec un nommage particulier */
        BasicBlock *createTestWhileBB();
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
        int getCurrentNumberOfWhileOrIf() {
            return CurrentNumberOfWhileOrIf;
        }

        /* Incrémente le nombre de While dans le cfg */
        void incrementCurrentNumberOfWhileOrIf()
        {
            ++CurrentNumberOfWhileOrIf;
        }

        /* Décrémente le nombre de While dans le cfg */
        void decrementCurrentNumberOfWhileOrIf()
        {
            --CurrentNumberOfWhileOrIf;
        }

    protected:
        map<string, pair<string,int>> *variables;   /* Pointeur sur la table des symboles */
        vector<BasicBlock *> bbs;                   /* Tous les basic blocks du CFG*/

        /* Compteur de While et de If par Cfg pour le nommage des basics blocks (Utile pour le Debuggage)*/
        int nbWhile = 0;
        int nbIf = 0;

        /* Numéro du if ou du while imbriqué (1 pour parent, puis on inscrémente pour les enfants)*/
        int CurrentNumberOfWhileOrIf = 0;
};

#endif