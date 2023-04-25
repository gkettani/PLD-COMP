#include "IRInstr.h"
#include <iostream>

class BasicBlock;

using namespace std;

IRInstr::IRInstr(BasicBlock *bb, IRInstr::Operation op, vector<string> params, map<string, pair<string, int>> *symboleTable) : bb(bb), op(op), params(params), variables(symboleTable) {}

void IRInstr::binaryOperation(ostream &o, string operation)
{
    string var1 = params[0];
    string var2 = params[1];
    string varTmp = params[2];

    if (var1[0] != '$' && var2[0] == '$')
    {
        o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
        o << "\t" + operation + "\t" << var2 << ", %eax\n";
    }

    if (var1[0] == '$' && var2[0] != '$')
    {
        o << "    movl	" << var1 << ", %eax\n";
        o << "\t" + operation + "\t" << (*variables)[var2].second << "(%rbp), %eax\n";
    }

    if (var1[0] != '$' && var2[0] != '$')
    {
        o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
        o << "\t" + operation + "\t" << (*variables)[var2].second << "(%rbp), %eax\n";
    }

    // On stocke le résultat de l'opération (qui est pour l'instant dans le registre %eax) à l'addresse de la variable temporaire
    o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";
}

void IRInstr::compareOperation(ostream &o, string operation)
{
    string var1 = params[0];
    string var2 = params[1];
    string varTmp = params[2];

    if (var1[0] != '$' && var2[0] == '$')
    {
        o << "    cmpl	" << var2 << ", " << (*variables)[var1].second << "(%rbp)\n";
    }

    if (var1[0] == '$' && var2[0] != '$')
    {
        o << "    cmpl	" << var1 << ", " << (*variables)[var2].second << "(%rbp)\n";
    }

    if (var1[0] != '$' && var2[0] != '$')
    {
        o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
        o << "    cmpl	" << (*variables)[var2].second << "(%rbp), %eax\n";
    }

    o << "\t" + operation + "\t"
      << "%al\n";
    o << "    andb	$1, %al\n";
    o << "    movzbl	%al, %eax\n";



    // On stocke le résultat de l'opération (qui est pour l'instant dans le registre %eax) à l'addresse de la variable temporaire
    o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";
}

void IRInstr::gen_asm(ostream &o)
{
    switch (op)
    {
    case IRInstr::save_rbp:
    {
        o << " 	# prologue \n";
        o << " 	pushq %rbp # save %rbp on the stack \n";
        o << " 	movq %rsp, %rbp # define %rbp for the current function \n";
        break;
    }

    case IRInstr::restore_rbp:
    {
        o << ".endLabel:\n";
        o << " 	# epilogue \n";
        o << " 	popq %rbp # restore %rbp from the stack \n";
        o << " 	ret\n";

        break;
    }

    case IRInstr::decl:
    {
        string var = params[0];
        string varValue = params[1];
        o << "    movl	$" << varValue << ", " << (*variables)[var].second << "(%rbp)\n";
        break;
    }

    case IRInstr::ret:
    {
        string var = params[0];

        if (var[0] == '$')
        {
            o << "    movl	" << var << ", %eax\n";
        }
        else if (var != "%eax")
        {
            string moveType = "    movl    ";
            if ((*variables)[var].first == "char")
            {
                moveType = "    movsbl  ";
            }
            o << moveType << (*variables)[var].second << "(%rbp), %eax\n";
        }
        break;
    }

    case IRInstr::ldconst:
    {
        string var = params[0];
        string constStr = params[1];
        string type = (*variables)[var].first;
        if (type == "char")
        {
                //Take into account test case of negation of char
                if(constStr=="%eax")
                {
                    constStr = "%al";
                }
            o << "    movb	" << constStr << ", " << (*variables)[var].second << "(%rbp)\n";
        }
        else
        {
            o << "    movl	" << constStr << ", " << (*variables)[var].second << "(%rbp)\n";
        }
        break;
    }

    case IRInstr::copy:
    {
        /** Copy param[1]'s value into  param[0]*/
        string var = params[0];
        string varTmp = params[1];
        string type = (*variables)[var].first;
        cout << "#" << varTmp << endl;
        if (type == "char")
        {
            o << "    movzbl    " << (*variables)[varTmp].second << "(%rbp), %eax\n";
            o << "    movb      %al, " << (*variables)[var].second << "(%rbp)\n";
        }
        else
        {
            o << "    movl	" << (*variables)[varTmp].second << "(%rbp), %eax\n";
            o << "    movl 	%eax, " << (*variables)[var].second << "(%rbp)\n";
        }
        break;
    }
    break;

    case IRInstr::add:
    {
        binaryOperation(o, "addl");
        break;
    }

    case IRInstr::sub:
    {
        binaryOperation(o, "subl");
        break;
    }

    case IRInstr::mul:
    {
        binaryOperation(o, "imull");
        break;
    }

    case IRInstr::div:
    {
        string var1 = params[0];
        string var2 = params[1];
        string varTmp = params[2];

        if (var1[0] != '$' && var2[0] == '$')
        {
            if (var2 == "$0")
            {
                std::cerr << "Error : Division by 0" << endl;
                throw "Division by 0";
            }
            else
            {
                o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
                o << "    cltd\n";
                o << "    idivl	" << var2 << ", %eax\n";
            }
        }

        if (var1[0] == '$' && var2[0] != '$')
        {
            o << "    movl	" << var1 << ", %eax\n";
            o << "    movl	" << (*variables)[var2].second << "(%rbp), %ebx\n";
            o << "    cltd\n";
            o << "    idivl	%ebx, %eax\n";
        }

        if (var1[0] != '$' && var2[0] != '$')
        {
            o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
            o << "    movl	" << (*variables)[var2].second << "(%rbp), %ebx\n";
            o << "    cltd\n";
            o << "    idivl	%ebx, %eax\n";
        }

        o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";

        break;
    }

    case IRInstr::mod:
    {
        string var1 = params[0];
        string var2 = params[1];
        string varTmp = params[2];

        if (var1[0] != '$' && var2[0] == '$')
        {
            if (var2 == "$0")
            {
                std::cerr << "Error : Division by 0" << endl;
                throw "Division by 0";
            }
            else
            {
                o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
                o << "    movl	$0 , %edx\n";
                o << "    cltd\n";
                o << "    idivl	" << var2 << ", %eax\n";
                o << "    movl	%edx , %eax\n";
            }
        }

        if (var1[0] == '$' && var2[0] != '$')
        {
            o << "    movl	" << var1 << ", %eax\n";
            o << "    movl	" << (*variables)[var2].second << "(%rbp), %ebx\n";
            o << "    movl	$0 , %edx\n";
            o << "    cltd\n";
            o << "    idivl	%ebx, %eax\n";
            o << "    movl	%edx , %eax\n";
        }

        if (var1[0] != '$' && var2[0] != '$')
        {
            o << "    movl	" << (*variables)[var1].second << "(%rbp), %eax\n";
            o << "    movl	" << (*variables)[var2].second << "(%rbp), %ebx\n";
            o << "    movl	$0 , %edx\n";
            o << "    cltd\n";
            o << "    idivl	%ebx, %eax\n";
            o << "    movl	%edx , %eax\n";
        }

        o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";

        break;
    }

    case IRInstr::wmem:
    {
        string var = params[0];
        string offset = params[1];
        string value = params[2];

        // change the value of the variable
        o << "    movl	" << value << ", " << (*variables)[var].second << "(%rbp)\n";
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
        binaryOperation(o, "andl");
        break;
    }

    case IRInstr::op_or:
    {
        binaryOperation(o, "orl");
        break;
    }

    case IRInstr::op_xor:
    {
        binaryOperation(o, "xorl");
        break;
    }

    case IRInstr::op_sup:
    {
        compareOperation(o, "setg");
        break;
    }

    case IRInstr::op_min:
    {
        compareOperation(o, "setl");
        break;
    }

    case IRInstr::op_equal:
    {
        compareOperation(o, "sete");
        break;
    }

    case IRInstr::op_diff:
    {
        compareOperation(o, "setne");
        break;
    }

    case IRInstr::op_neg:
    {
        string var = params[0];
        string varTmp = params[1];

        o << "    movl	" << (*variables)[var].second << "(%rbp), %eax\n";
        o << "    negl	%eax\n";

        // On stocke le résultat de l'opération (qui est pour l'instant dans le registre %eax) à l'addresse de la variable temporaire
        o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";

        break;
    }

    case IRInstr::op_not:
    {
        string var = params[0];
        string varTmp = params[1];

        o << "    cmpl	$0, " << (*variables)[var].second << "(%rbp)\n";
        o << "    sete  %al\n";
        o << "    movzbl	%al, %eax\n";

        // On stocke le résultat de l'opération (qui est pour l'instant dans le registre %eax) à l'addresse de la variable temporaire
        o << "    movl	%eax, " << (*variables)[varTmp].second << "(%rbp)\n";

        break;
    }

    case IRInstr::conditional_jump:
    {
        string testVarName = params[0];
        string labelTrue = params[1];
        string labelFalse = params[2];

        o << "    cmpl	$0, " << (*variables)[testVarName].second << "(%rbp)\n";
        o << "    je      " << labelFalse << "\n";
        o << "    jmp     " << labelTrue << "\n";

        break;
    }

    case IRInstr::absolute_jump:
    {
        string nextLabel = params[0];

        o << "    jmp     " << nextLabel << "\n";

        break;
    }
    case IRInstr::putcharChar:
    {
        string var = params[0];
        o << "\tmovl\t"
          << "$" << var << ", %edi\n";
        o << "\tcall\tputchar@PLT\n";
        break;
    }
    case IRInstr::putcharVar:
    {
        string var = params[0];
        o << "\tmovzbl\t" << var << "(%rbp), %eax\n";
        o << "\tmovl\t%eax, %edi\n";
        o << "\tcall\tputchar@PLT\n";     
        break;
    }

    case IRInstr::getchar:
    {
        string var = params[0];
        o << "\tmovl\t$0,%eax\n";
        o << "\tcall\tgetchar\n";
        o << "\tmovl\t%eax,"<< var<< "(%rbp)\n";
        break;
    }
    }
}
