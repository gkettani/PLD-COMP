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
        void add_IRInstr(IRInstr::Operation op, vector<string> params, map<string, int> *variables);
        
    protected:
        BasicBlock *exit_true;    /**< pointer to the next basic block, true branch. If nullptr, return from procedure */
        BasicBlock *exit_false;   /**< pointer to the next basic block, false branch. If null_ptr, the basic block ends with an unconditional jump */
        string test_var_name;     /** < when generating IR code for an if(expr) or while(expr) etc, store here the name of the variable that holds the value of expr */

        string label;             /**< label of the BB, also will be the label in the generated code */
        CFG *cfg;                 /** < CFG auquel le block appartient */
        vector<IRInstr *> instrs; /** < Les instructions du bloc */

};