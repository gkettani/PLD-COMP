#include "IRInstr.h"
#include <iostream>

class BasicBlock;

using namespace std;

IRInstr::IRInstr(BasicBlock *bb, IRInstr::Operation op, vector<string> params, map<string, int>* symboleTable) : bb(bb), op(op), params(params), variables(symboleTable) {}

void binaryOperation(ostream & o, string var1, string var2, string varTmp, map<string, int> *variables, string operation){
    if (var1[0] != '$' && var2[0] == '$')
    {
        o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
        o << operation << var2 << ", %eax\n";
    }

    if (var1[0] == '$' && var2[0] != '$')
    {
        o << "	movl	" << var1 << ", %eax\n";
        o << operation << (*variables)[var2] << "(%rbp), %eax\n";
    }

    if (var1[0] != '$' && var2[0] != '$')
    {
        o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
        o << operation << (*variables)[var2] << "(%rbp), %eax\n";
    }

    o << "	movl	%eax, " << (*variables)[varTmp] << "(%rbp)\n";
}

void compareOperation(ostream & o, string var1, string var2, map<string, int> *variables, string operation){
    if(var1[0] != '$' && var2[0] == '$'){
        o << "	cmpl	" << var2 << ", " << (*variables)[var1] << "(%rbp)\n";
    }

    if(var1[0] == '$' && var2[0] != '$'){
        o << "	cmpl	" << var1 << ", " << (*variables)[var2] << "(%rbp)\n";
    }

    if(var1[0] != '$' && var2[0] != '$'){
        o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
        o << "	cmpl	" << (*variables)[var2] << "(%rbp), %eax\n";
    }

    o << operation <<  "%al\n";
    o << "  andb	$1, %al\n";
    o << "  movzbl	%al, %eax\n";

}

void IRInstr::gen_asm(ostream & o){
    switch(op){
        case IRInstr::decl:
        {
            string var = params[0];
            string varValue = params[1];
            o << " 	movl	$" << varValue << ", " << (*variables)[var] << "(%rbp)\n";
            break;
        }

        case IRInstr::ret:
        {
            string var = params[0];
            if (var[0] == '$'){
                o << " 	movl	" << var << ", %eax\n";
            }else if (var != "%eax"){ 
                o << " 	movl	" << (*variables)[var] << "(%rbp), %eax\n";
            }
            break;
        }

        case IRInstr::ldconst:
        {
            string var = params[0];
            string constStr = params[1];
            o << " 	movl	" << constStr << ", " << (*variables)[var] << "(%rbp)\n";
            break;
        }
            
        case IRInstr::copy:
        {
            string var = params[0];
            string varTmp = params[1];
            o << "  movl	" << (*variables)[varTmp] << "(%rbp), %eax\n";
            o << "	movl 	%eax, " << (*variables)[var] << "(%rbp)\n";
            break;
        }
            break;

        case IRInstr::add:
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    addl  ");
            break;
        }

        case IRInstr::sub:
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    subl   ");
            break;
        }

        case IRInstr::mul:
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    imull   ");
            break;
        }

        case IRInstr::div:
        {
            string var1 = params[0];
            string var2 = params[1];
            string varTmp = params[2];

            if (var1[0] != '$' && var2[0] == '$')
            {
                if(var2 == "$0")
                {
			        std::cerr<< "Error : Division by 0" << endl;
			        throw "Division by 0";
                }
                else
                {
                    o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                    o << " cltd\n";
                    o << "	idivl	" << var2 << ", %eax\n";
                }
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << var1 << ", %eax\n";
                o << "	movl	" << (*variables)[var2] << "(%rbp), %ebx\n";
                o << "  cltd\n";
                o << "	idivl	%ebx, %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	movl	" << (*variables)[var2] << "(%rbp), %ebx\n";
                o << "  cltd\n";
                o << "	idivl	%ebx, %eax\n";
            }

            o << "	movl	%eax, " << (*variables)[varTmp] << "(%rbp)\n";

            break;
        }


        case IRInstr::wmem:
            {
                string var = params[0];
                string offset = params[1];
                string value = params[2];

               //change the value of the variable
                o << "	movl	"<<value<<", " << (*variables)[var] << "(%rbp)\n";
                break;
            }
        case IRInstr::rmem:
            break;

        case IRInstr::call:
            break;

        case IRInstr::cmp_eq:
            break;

        case IRInstr::cmp_lt:
            break;

        case IRInstr::cmp_le:
            break;

        case IRInstr::op_and:
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    andl   ");
            break;
        }

        case IRInstr::op_or:
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    orl   ");
            break;
        }
            
        case IRInstr::op_xor: 
        {
            binaryOperation(o, params[0], params[1], params[2], variables, "    xorl   ");
            break;
        }

        case IRInstr::op_sup:
        {
            compareOperation(o, params[0], params[1], variables, "    setg   ");
            break;
        }

        case IRInstr::op_min:
        {
            compareOperation(o, params[0], params[1], variables, "    setl   ");
            break;
        }

        case IRInstr::op_equal:
        {
            compareOperation(o, params[0], params[1], variables, "    sete   ");
            break;
        }

        case IRInstr::op_diff:
        {
            compareOperation(o, params[0], params[1], variables, "    setne   ");
            break;
        }
        

        case IRInstr::op_neg: 
        {
            string var = params[0];

            o << "	movl	" << (*variables)[var] << "(%rbp), %eax\n";
            o << "    negl	%eax\n";

            break;
        }
        case IRInstr::op_not: 
        {
            string var = params[0];

            o << "  cmpl	$0, "<<(*variables)[var]<<"(%rbp)\n";
            o << "  sete  %al\n";
            o << "  movzbl	%al, %eax\n";
            
            break;
        }
    }
}
