#include "CodeGenVisitor.h"
using namespace std;

CodeGenVisitor::CodeGenVisitor(CFG &cfg) : cfg(cfg) {}
CodeGenVisitor::~CodeGenVisitor() {}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{
	// cout<<".globl	main\n"
	// 	" main: \n"
	// 	" 	# prologue \n"
	// 	" 	pushq %rbp # save %rbp on the stack \n"
	// 	" 	movq %rsp, %rbp # define %rbp for the current function \n";
	// 	// " 	movl	$"<<retval<<", %eax\n"
	// 	visitChildren(ctx);
	// 	cout<<" 	# epilogue \n"
	// 	" 	popq %rbp # restore %rbp from the stack \n"
	// 	" 	ret\n";

	visitChildren(ctx);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitType(ifccParser::TypeContext *ctx)
{
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare(ifccParser::DeclareContext *ctx)
{
	string var = ctx->VAR()->getText();
	string varValue = "0";

	varCounter += 1;
	int offset = varCounter * -4;
	variables[var] = offset;

	cfg.current_bb->add_IRInstr(IRInstr::decl, {var, varValue}, &variables);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRet(ifccParser::RetContext *ctx)
{
	string var = "";
	string varType = "";

	if(ctx->expr()){
		var = (string)visit(ctx->expr());
		cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	}else{
		var = "$0";
		cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	}

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{

	string var = ctx->VAR()->getText();
	if (ctx->type())
	{
		varCounter += 1;
		int offset = varCounter * -4;
		variables[var] = offset;
	}

	string varTmp = (string)visit(ctx->expr());

	if(varTmp[0] == '$' | varTmp == "%eax"){
		cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, varTmp}, &variables);
	}else{
		cfg.current_bb->add_IRInstr(IRInstr::affect, {var, varTmp}, &variables);
	}

	return 0;

}

antlrcpp::Any CodeGenVisitor::visitVarExpr(ifccParser::VarExprContext *ctx)
{
	string var = ctx->VAR()->getText();
	return var;
}

antlrcpp::Any CodeGenVisitor::visitConstExpr(ifccParser::ConstExprContext *ctx)
{
	string var = "$" + ctx->CONST()->getText();
	return var;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (!(var1[0] == '$' && var2[0] == '$'))
	{
		// Si les opérandes ne sont pas toutes les deux des constantes
		cfg.current_bb->add_IRInstr(IRInstr::op_or, {var1, var2}, &variables);
		resultStr = "%eax";
	}
	else
	{
		// Si les opérandes sont toutes les deux des constantes
		int val1 = stoi(ctx->expr(0)->getText());
		int val2 = stoi(ctx->expr(1)->getText());
		int result = val1 | val2;
		resultStr = "$" + to_string(result);
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (!(var1[0] == '$' && var2[0] == '$'))
	{
		// Si les opérandes ne sont pas toutes les deux des constantes
		cfg.current_bb->add_IRInstr(IRInstr::op_xor, {var1, var2}, &variables);
		resultStr = "%eax";
	}
	else
	{
		// Si les opérandes sont toutes les deux des constantes
		int val1 = stoi(ctx->expr(0)->getText());
		int val2 = stoi(ctx->expr(1)->getText());
		int result = val1 ^ val2;
		resultStr = "$" + to_string(result);
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if(!(var1[0] == '$' && var2[0] == '$')) {
		// Si les opérandes ne sont pas toutes les deux des constantes
		cfg.current_bb->add_IRInstr(IRInstr::op_and, {var1, var2}, &variables);
		resultStr = "%eax";
	}else{
		// Si les opérandes sont toutes les deux des constantes
		int val1 = stoi(ctx->expr(0)->getText());
		int val2 = stoi(ctx->expr(1)->getText());
		int result = val1 & val2;
		resultStr = "$" + to_string(result);
	}

	return resultStr;
}
