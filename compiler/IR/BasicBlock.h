#include <string>
#include <vector>
#include <map>
#include "IRInstr.h"

class CFG;

using namespace std;

class BasicBlock
{
    public:
        BasicBlock(CFG *cfg, string entry_label);
        ~BasicBlock();

        /* Génère le code assembleur x86 d'un basic block*/
        void gen_asm(ostream &o); 

        /* Ajoute une instruction à un basic block*/
        void add_IRInstr(IRInstr::Operation op, vector<string> params, map<string, pair<string,int>> *variables);
        
        /* Récupérer le label du basic block*/
        string getLabel(){
            return label;
        }

        void setExitTrue(BasicBlock* bb){
            this->exit_true = bb;
        }

        BasicBlock* getExitTrue(){
            return this->exit_true;
        }

        void setExitFalse(BasicBlock* bb){
            this->exit_false = bb;
        }

        BasicBlock* getExitFalse(){
            return this->exit_false;
        }

        string getTestVarName()
        {
            return test_var_name;
        }

        void setTestVarName(string testVar)
        {
            test_var_name = testVar;
        }

    protected:
        string label;                       /* Label du bloc */
        CFG *cfg;                           /* CFG auquel le block appartient */
        vector<IRInstr *> instrs;           /* Les instructions du bloc */

        BasicBlock *exit_true = nullptr;    /* Pointeur sur le bloc suivant (branche true). Si nullptr, return from procedure */
        BasicBlock *exit_false = nullptr;   /* Pointeur sur le bloc suivant (branche false). Si nullptr, le bloc termine avec un unconditional jump */
        string test_var_name;               /* Quand on génère du code IR pour un if(expr) ou while(expr) ect, on stocke is le nom de la variable qui contient la valeur de expr*/
};