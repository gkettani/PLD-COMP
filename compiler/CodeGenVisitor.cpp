#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
	std::cout<<".globl	main\n"
		" main: \n"
		" 	# prologue \n"
		" 	pushq %rbp # save %rbp on the stack \n"
		" 	movq %rsp, %rbp # define %rbp for the current function \n";
		// " 	movl	$"<<retval<<", %eax\n"
		visitChildren(ctx);
		std::cout<<" 	# epilogue \n"
		" 	popq %rbp # restore %rbp from the stack \n"
		" 	ret\n";

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
	std::cout << " 	# instruction \n";
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitType(ifccParser::TypeContext *ctx)
{
	std::cout << " 	# type \n";
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare(ifccParser::DeclareContext *ctx)
{
	int retval = stoi(ctx->CONST()->getText());
	varCounter += 1;
	int offset = varCounter * -4;
	variables[ctx->VAR()->getText()] = offset;
	std::cout<<" 	movl	$"<<retval<<", "<< offset <<"(%rbp)\n";
	
	visitChildren(ctx);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRet(ifccParser::RetContext *ctx)
{
	if (ctx->VAR()) {
		std::string var = ctx->VAR()->getText();
		int offset = variables[var];
		std::cout << " 	movl	" << offset << "(%rbp), %eax\n";
	
	}

	if (ctx->CONST()) {
		std::string var = ctx->CONST()->getText();
		std::cout << " 	movl	$" << var << ", %eax\n";
	}

	// std::cout << var << std::endl;
	// offset = variables[var];
	// visitChildren(ctx);
	return 0;
}
