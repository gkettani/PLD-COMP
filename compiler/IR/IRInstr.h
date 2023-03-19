#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

class BasicBlock;

class IRInstr{
    public:
        typedef enum
        {
            decl,
            ret,
            affect,
            ldconst,
            copy,
            add,
            sub,
            mul,
            rmem,
            wmem,
            call,
            cmp_eq,
            cmp_lt,
            cmp_le, 
            op_and,
            op_or,
            op_xor
        } Operation;

        /**  constructor */
        IRInstr(BasicBlock *bb, IRInstr::Operation op, vector<string> params, map<string, int> *symboleTable);

        /** Actual code generation */
        void gen_asm(ostream &o); /**< x86 assembly code generation for this IR instruction */

    private:

        /* The BB this instruction belongs to, which provides a pointer to the CFG this instruction belong to */
        BasicBlock *bb;

        /* The operator of the instruction*/
        Operation op;

        /* The parameters of the instruction (source, destination...)*/
        /* For 3-op instrs: d, x, y; for ldconst: d, c;  For call: label, d, params;  for wmem and rmem: choose yourself */
        vector<string> params;
    
        /* Symbol table */
        map<string, int>* variables;
};