#include "CodeGenVisitor.h"
using namespace std;

CodeGenVisitor::CodeGenVisitor(CFG &cfg) : cfg(cfg) {}
CodeGenVisitor::~CodeGenVisitor() {}

void CodeGenVisitor::addVariable(string name, int size)
{
	int offset = ++varCounter * -size;
	variables[name] = offset;
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{	
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
	string var = ctx->listvar()->getText();

	//Vérifier si la variable a déjà été déclarée 
	if (variables.find(var) != variables.end()) {
		cerr << "Error: variable '" << var << "' already defined\n";
		throw "Error duplicate variable declaration";
	}

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
	string temp="";
	

	//iterate over the unused variables and print them
	for (auto it = variablesUsageCounter.begin(); it != variablesUsageCounter.end(); ++it)
	{
		if(it->second == 0){
			temp += it->first+",";
		}
	}

	//Warning message:unused variables gcc 
	if(temp != ""){
		cerr<<"#warning unused variables: "<<temp<<endl;
	}
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{

	string var = ctx->VAR()->getText();
	
	//S'il y a un type devant le nom de la variable alors c'est une initialisation, il faut mettre à jour la table des symboles
	if (ctx->type())
	{
		/*Vérifier d'abord si cette variable a déjà été déclarée
		Dans ce cas, we throw an error*/
		if (variables.find(var) != variables.end()) {
			cerr << "Error: variable '" << var << "' already defined\n";
			throw "Error duplicate variable declaration";
		}
		//Sinon, on l'ajoute dans la table des symboles
		else{
			addVariable(var);
			variablesUsageCounter.insert({var,0});
		}
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
    //le character commence par des "@""
    if(varTmp[0] == '@'){
        int firstChar = varTmp.at(2);
	    int alp = firstChar;
	    stringstream ss;
	    ss << alp;
	    string str = ss.str();
        cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, str}, &variables);
    }

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitVarExpr(ifccParser::VarExprContext *ctx)
{
	string var = visit(ctx->usedvar());
	variablesUsageCounter[var]++;
	return visit(ctx->usedvar());
}

antlrcpp::Any CodeGenVisitor::visitConstExpr(ifccParser::ConstExprContext *ctx)
{
	/* Le fait de rajouter le $ permet de répérer facilement les const*/
	string var = "$" + ctx->CONST()->getText();
	return var;
}

antlrcpp::Any CodeGenVisitor::visitParExpr(ifccParser::ParExprContext *ctx)
{
	string expr = visit(ctx->expr());
	return expr;
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
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

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
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

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

	}
	else
	{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);
		
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
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

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
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		cfg.current_bb->add_IRInstr(IRInstr::sub, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}
// multdiv 
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
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		cfg.current_bb->add_IRInstr(IRInstr::mul, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}

// divexpr taking care of division by 0
antlrcpp::Any CodeGenVisitor::visitDivExpr(ifccParser::DivExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		if (val2 == 0)
		{
			cerr<< "Error : Division by 0" << endl;
			throw "Division by 0";
		}
		int result = val1 / val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		cfg.current_bb->add_IRInstr(IRInstr::div, {var1, var2, varTmp}, &variables);
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
	while ((pos = var.find(delimiter)) != string::npos) {
    token = var.substr(0, pos);
		varValue = "0";
		addVariable(token);
		variablesUsageCounter[token] = 0;//ajout d'une variable non utilisée
		cfg.current_bb->add_IRInstr(IRInstr::decl, {token, varValue}, &variables);
    var.erase(0, pos + delimiter.length());
	}
	varValue = "0";
	addVariable(var);
	variablesUsageCounter[var] = 0;//ajout d'une variable non utilisée
	
	cfg.current_bb->add_IRInstr(IRInstr::decl, {var, varValue}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitUsedvar(ifccParser::UsedvarContext *context) {
	string var = context->VAR()->getText();
	return var;
};

antlrcpp::Any CodeGenVisitor::visitAlphabets(ifccParser::AlphabetsContext *ctx) {
	string var = "@" + ctx->ABCD()->getText();
	return var;
};