#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
	int retval = stoi(ctx->CONST()->getText());
	std::cout<<".globl	main\n"
		" main: \n"
		" 	# prologue \n"
		" 	pushq %rbp # save %rbp on the stack \n"
		" 	movq %rsp, %rbp # define %rbp for the current function \n"
		" 	movl	$"<<retval<<", %eax\n"
		" 	# epilogue \n"
		" 	popq %rbp # restore %rbp from the stack \n"
		" 	ret\n";

	return 0;
}
