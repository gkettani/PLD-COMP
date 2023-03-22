#include "IRInstr.h"
#include <iostream>

class BasicBlock;

using namespace std;

IRInstr::IRInstr(BasicBlock *bb, IRInstr::Operation op, vector<string> params, map<string, int>* symboleTable) : bb(bb), op(op), params(params), variables(symboleTable) {}

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
            o << " 	movl	" << (*variables)[varTmp] << "(%rbp), %eax\n";
            o << "	movl 	%eax, " << (*variables)[var] << "(%rbp)\n";
            break;
        }
            break;

        case IRInstr::add:
        {
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	addl	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var2] << "(%rbp), %eax\n";
                o << "	addl	" << var1 << ", %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	addl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }
            break;
        }

        case IRInstr::sub:
        {
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	subl	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << var1 << ", %eax\n";
                o << "	subl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	subl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }
            break;
        }

        case IRInstr::mul:
        {
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	imull	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << var1 << ", %eax\n";
                o << "	imull	" << (*variables)[var2] << "(%rbp), %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	imull	" << (*variables)[var2] << "(%rbp), %eax\n";
            }
            break;
        }

        case IRInstr::rmem:
            break;

        case IRInstr::wmem:
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
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	andl	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var2] << "(%rbp), %eax\n";
                o << "	andl	" << var1 << ", %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	andl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }

            break;
        }

        case IRInstr::op_or:
        {
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	orl	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var2] << "(%rbp), %eax\n";
                o << "	orl	" << var1 << ", %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	orl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }

            break;
        }
            
        case IRInstr::op_xor: 
        {
            string var1 = params[0];
            string var2 = params[1];

            if (var1[0] != '$' && var2[0] == '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	xorl	" << var2 << ", %eax\n";
            }

            if (var1[0] == '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var2] << "(%rbp), %eax\n";
                o << "	xorl	" << var1 << ", %eax\n";
            }

            if (var1[0] != '$' && var2[0] != '$')
            {
                o << "	movl	" << (*variables)[var1] << "(%rbp), %eax\n";
                o << "	xorl	" << (*variables)[var2] << "(%rbp), %eax\n";
            }

            break;
        }

        case IRInstr::op_sup:
        {
            string var1 = params[0];
            string var2 = params[1];

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

            o << "	setg	"<<  "%al\n";
            o << "  andb	$1, %al\n";
            o << "  movzbl	%al, %eax\n";

            break;
        }

        case IRInstr::op_min:
        {
            string var1 = params[0];
            string var2 = params[1];

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

            o << "	setl	"<<  "%al\n";
            o << "  andb	$1, %al\n";
            o << "  movzbl	%al, %eax\n";

            break;
        }

        case IRInstr::op_equal:
        {
            string var1 = params[0];
            string var2 = params[1];

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

            o << "	sete	"<<  "%al\n";
            o << "  andb	$1, %al\n";
            o << "  movzbl	%al, %eax\n";

            break;
        }

        case IRInstr::op_diff:
        {
            string var1 = params[0];
            string var2 = params[1];

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

            o << "	setne	"<<  "%al\n";
            o << "  andb	$1, %al\n";
            o << "  movzbl	%al, %eax\n";

            break;
        }
        

        case IRInstr::op_neg: 
        {
            string var = params[0];

            o << "	movl	" << (*variables)[var] << "(%rbp), %eax\n";
            o << "negl	%eax\n";

            break;
        }
        case IRInstr::op_not: 
        {
            string var = params[0];

            o << "cmpl	$0, "<<(*variables)[var]<<"(%rbp)\n";
            o << "sete  %al\n";
            o << "movzbl	%al, %eax\n";
            
            break;
        }
    }
}

