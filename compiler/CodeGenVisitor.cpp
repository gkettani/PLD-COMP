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

	visit(ctx->listvar());
	
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRet(ifccParser::RetContext *ctx)
{
	string var = "";
	string varType = "";

	if(ctx->expr()){
		var = visit(ctx->expr()).as<string>();
		cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	}else{
		/* Si on a juste un return sans expression derrière, c'est comme si on return 0*/
		var = "$0";
		cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	}

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{

	string var = ctx->VAR()->getText();

	//S'il y a un type devant le nom de la variable alors c'est une initialisation, il faut mettre à jour la table des symboles
	if (ctx->type())
	{
		varCounter += 1;
		int offset = varCounter * -4;
		variables[var] = offset;
	}



	/* On récupère la variable ou la constante qui se trouve en partie droite de l'affectation*/
	string varTmp = visit(ctx->expr()).as<string>();

	
	/* Le cas varTmp == "%eax" est utile pour construire la 3ème instruction assembleur quand on fait une opération binaire*/
	if(varTmp[0] == '$' | varTmp == "%eax"){
		cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, varTmp}, &variables);
	}
	else
	{
		cfg.current_bb->add_IRInstr(IRInstr::copy, {var, varTmp}, &variables);
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
	/* Le fait de rajouter le $ permet de répérer facilement les const*/
	string var = "$" + ctx->CONST()->getText();
	return var;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		// Si les opérandes sont toutes les deux des constantes, on fait directement remonter le résultat qui est une constante
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 | val2;
		resultStr = "$" + to_string(result); 
	}
	else
	{
		// Si les opérandes ne sont pas toutes les deux des constantes, il faut construire des instructions assembleurs en plus
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;

		cfg.current_bb->add_IRInstr(IRInstr::op_or, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitXorExpr(ifccParser::XorExprContext *ctx)
{
	// Même logique que visitOrExpr

	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 ^ val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;

		cfg.current_bb->add_IRInstr(IRInstr::op_xor, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitAndExpr(ifccParser::AndExprContext *ctx)
{
	// Même logique que visitOrExpr

	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if(var1[0] == '$' && var2[0] == '$') {
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 & val2;
		resultStr = "$" + to_string(result);

	}else{
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;
		
		cfg.current_bb->add_IRInstr(IRInstr::op_and, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitCompareExpr(ifccParser::CompareExprContext *ctx){
	string resultStr = "";

	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	if(ctx->COMPAREOP()->getText() == ">"){
		cfg.current_bb->add_IRInstr(IRInstr::op_sup, {var1, var2}, &variables);
	}if(ctx->COMPAREOP()->getText() == "<"){
		cfg.current_bb->add_IRInstr(IRInstr::op_min, {var1, var2}, &variables);
	}
	resultStr = "%eax";
	
	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitEqualExpr(ifccParser::EqualExprContext *ctx){
	string resultStr = "";

	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	if(ctx->EQUALOP()->getText() == "=="){
		cfg.current_bb->add_IRInstr(IRInstr::op_equal, {var1, var2}, &variables);
	}if(ctx->EQUALOP()->getText() == "!="){
		cfg.current_bb->add_IRInstr(IRInstr::op_diff, {var1, var2}, &variables);
	}
	resultStr = "%eax";
	
	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitNegExpr(ifccParser::NegExprContext *ctx)
{
	string var =visit(ctx->expr());

	string resultStr = "";

	if(var[0] == '$' ) {
		int val = stoi(ctx->expr()->getText());
		int result = - val;
		resultStr = "$" + to_string(result);

	}else{
		cfg.current_bb->add_IRInstr(IRInstr::op_neg, {var}, &variables);
		resultStr = "%eax";
	}
	return resultStr;
}
antlrcpp::Any CodeGenVisitor::visitNotExpr(ifccParser::NotExprContext *ctx)
{
	string var =visit(ctx->expr());

	string resultStr = "";

	if(var[0] == '$' ) {
		int val = stoi(ctx->expr()->getText());
		int result = ! val;
		resultStr = "$" + to_string(result);

	}else{
		cfg.current_bb->add_IRInstr(IRInstr::op_not, {var}, &variables);
		resultStr = "%eax";
	}
	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitPlusExpr(ifccParser::PlusExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 + val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;

		cfg.current_bb->add_IRInstr(IRInstr::add, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitMinusExpr(ifccParser::MinusExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 - val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;

		cfg.current_bb->add_IRInstr(IRInstr::sub, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitMultExpr(ifccParser::MultExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result = val1 * val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		varCounter += 1;
		string varTmp = "!tmp" + varCounter;
		int offset = varCounter * -4;
		variables[varTmp] = offset;

		cfg.current_bb->add_IRInstr(IRInstr::mul, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitListvar(ifccParser::ListvarContext *ctx)
{
	
	//récuperer la liste des variables 
	string var = ctx->getText();
	string delimiter = ",";
	string varValue;
	size_t pos = 0;
	string token;
	//split la liste des variables
	while ((pos = var.find(delimiter)) != std::string::npos) {
    token = var.substr(0, pos);
	varValue = "0";
	varCounter += 1;
	int offset = varCounter * -4;
	variables[token] = offset;
	cfg.current_bb->add_IRInstr(IRInstr::decl, {token, varValue}, &variables);
    var.erase(0, pos + delimiter.length());
	}
	varValue = "0";
	varCounter += 1;
	int offset = varCounter * -4;
	variables[var] = offset;
	cfg.current_bb->add_IRInstr(IRInstr::decl, {var, varValue}, &variables);


return 0;

}
