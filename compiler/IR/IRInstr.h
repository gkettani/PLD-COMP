#include <vector>
#include <string>
#include <sstream>
#include <map>

using namespace std;

class BasicBlock;

/* Cette classe permet de traduire les différents instructions en langage assembleur*/

class IRInstr{
    public:
        typedef enum
        {
            save_rbp,
            restore_rbp,
            decl,
            decltab,
            afftab,
            afftabvar,
            ret,
            ldconst,
            copy,
            add,
            sub,
            mul,
            div,
            mod,
            rmem,
            wmem,
            call,
            cmp_eq,
            cmp_lt,
            cmp_le,
            op_and,
            op_or,
            op_xor,
            op_sup,
            op_min,
            op_equal,
            op_diff,
            op_neg,
            op_not,
            conditional_jump,
            absolute_jump,
            putcharChar,
            putcharVar,
            getchar
        } Operation;

        IRInstr(BasicBlock *bb, IRInstr::Operation op, vector<string> params, map<string, pair<string,int>> *symboleTable);

        /* Génère le code assembleur x86 de l'instruction IR*/
        void gen_asm(ostream &o);
        /* Génère le code assembleur x86 d'une opération binaire*/
        void binaryOperation(ostream &o, string operation);
        /* Génère le code assembleur x86 d'une opération de comparaison*/
        void compareOperation(ostream &o, string operation);

    private:
        BasicBlock *bb;        /** < BasicBlock auquel l'instruction IR appartient */
        Operation op;          /** < Opérateur de l'instruction*/
        vector<string> params; /** < Paramètres de l'instruction */
        map<string, pair<string,int>> *variables; /** < Pointeur sur la Table des symboles */
};