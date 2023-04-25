#include "CodeGenVisitor.h"
#include <cstring>
using namespace std;

CodeGenVisitor::CodeGenVisitor(CFG &cfg) : cfg(cfg) {}
CodeGenVisitor::~CodeGenVisitor() {}

void CodeGenVisitor::addVariable(string name,string type, int size)
{
	variables[name].first = type;
	const char* typec = type.c_str();
	if(strcmp(typec,"int")==0|strcmp(typec,"float")==0){
		size = 4;
	}else if(strcmp(typec,"char")==0) {
		size = 1;
	}else if(strcmp(typec,"double")){
		size= 8;
	}
	
	int offset = ++varCounter * -size;
	variables[name].second = offset;
}

void CodeGenVisitor::addArray(string name, string type, int size){
	variables[name].first = type;
	const char* typec = type.c_str();
	int x=0;
	if(strcmp(typec,"int")==0|strcmp(typec,"float")==0){
		x = 4;
	}else if(strcmp(typec,"char")==0) {
		x = 1;
	}else if(strcmp(typec,"double")){
		x= 8;
	}
	//int offset = ++varCounter * -(4*x);
	variables[name].second = size*x;
	varCounter = varCounter + size;
}

bool CodeGenVisitor::doesExist(string var){
	if(variables.find(var) == variables.end()){ //si variable inexistante dans la table
		return false;   
	}
	return true;
}

bool CodeGenVisitor::isVariable(string var){
	if (var.substr(0, 1) == "$"){
		return false;
	}
	return true;
}

void CodeGenVisitor::checkDeclaredExpr(string var1, string var2){
	if(isVariable(var1) && isVariable(var2) && !doesExist(var1) && !doesExist(var2)){
		std::cerr << "Error: variable '" << var1 << "' undefined\n";
		std::cerr << "Error: variable '" << var2 << "' undefined\n";
		throw "Error undefined variable";
	}
	if(isVariable(var1) && !doesExist(var1)){
		std::cerr << "Error: variable '" << var1 << "' undefined\n";
		throw "Error undefined variable";	
	}
	if(isVariable(var2) && !doesExist(var2)){
		std::cerr << "Error: variable '" << var2 << "' undefined\n";
		throw "Error undefined variable";	
	}
}

string CodeGenVisitor::convertCharToInt(string var)
{
	if(var[0] == '@'){
		int firstChar = var.at(2);
		int alp = firstChar;
		stringstream ss;
		ss << alp;
		string str = ss.str();
		char c = '$';
		str.insert(0, 1, c);
		return str;
	}
	return var;
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx)
{	
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitStartMainBlock(ifccParser::StartMainBlockContext *ctx) {
	/* On save le %rbp sur la stack et on définit le rbp de la fonction courrante*/
	cfg.current_bb->add_IRInstr(IRInstr::save_rbp, {}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitEndMainBlock(ifccParser::EndMainBlockContext *ctx) {
	/* On restore le %rbp dans le bloc final et on ajoute une instruction au bloc courant qui permet de sauter au bloc final*/
	cfg.final_bb->add_IRInstr(IRInstr::restore_rbp, {}, &variables);
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
antlrcpp::Any CodeGenVisitor::visitListVarDec(ifccParser::ListVarDecContext *ctx){
	string var = ctx->listvar()->getText();

	//Vérifier si la variable a déjà été déclarée 
	if (doesExist(var)) {
		cerr << "Error: variable '" << var << "' already defined\n";
		throw "Error duplicate variable declaration";
	}

	string type = ctx->type()->getText();
	set<string> values = visit(ctx->listvar());
	for (auto var : values) {
		addVariable(var, type);
	}
	return 0;
}
antlrcpp::Any CodeGenVisitor::visitArrayDec(ifccParser::ArrayDecContext *ctx){
	string type = ctx->type()->getText();
	string variableName = ctx->VAR()->getText();
	if(doesExist(variableName)){
		cerr << "Error: variable '" << variableName << "' already defined\n";
		throw "Error duplicate variable declaration";
	}
	string size = ctx->CONST()->getText();
	int s= stoi(size);
	addArray(variableName, type, s);

	cfg.current_bb->add_IRInstr(IRInstr::decltab, {variableName,size,type}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRetVar(ifccParser::RetVarContext *ctx){
	string var = ctx->VAR()->getText();
	/*Si on essaie de retourner une variable qui n'a pas été déclarée, on throw une erreur*/
	if(!doesExist(var)){ //si variable inexistante dans la table
		std::cerr << "Error: variable '" << var << "' undefined\n";
		throw "Error undefined variable";
	}else{
		cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	}

	string temp="";
	
	//iterate over the unused variables and print them
	for (auto it = variablesUsageCounter.begin(); it != variablesUsageCounter.end(); ++it)
	{
		if((it->second).second == 0){
			temp += it->first+",";
		}
	}

	//Warning message:unused variables gcc 
	if(temp != ""){
		cerr<<"#warning unused variables: "<<temp<<endl;
	}
	//move to the last basic block
	cfg.current_bb->setExitTrue(cfg.final_bb);
	cfg.current_bb->setExitFalse(nullptr);
	cfg.current_bb=cfg.final_bb;
	cfg.final_bb->add_IRInstr(IRInstr::absolute_jump, {".endLabel"}, &variables);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRetConst(ifccParser::RetConstContext *ctx){
	string var = "$" + ctx->CONST()->getText();
	cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	//move to the last basic block
	cfg.current_bb->setExitTrue(cfg.final_bb);
	cfg.current_bb->setExitFalse(nullptr);
	cfg.current_bb=cfg.final_bb;
	cfg.final_bb->add_IRInstr(IRInstr::absolute_jump, {".endLabel"}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRetExpr(ifccParser::RetExprContext *ctx){
	string var = visit(ctx->expr()).as<string>();
	var = convertCharToInt(var);
	cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	//move to the last basic block
	cfg.current_bb->setExitTrue(cfg.final_bb);
	cfg.current_bb->setExitFalse(nullptr);
	cfg.current_bb=cfg.final_bb;
	cfg.final_bb->add_IRInstr(IRInstr::absolute_jump, {".endLabel"}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRetNothing(ifccParser::RetNothingContext *ctx){
	string var = "$0";
	cfg.current_bb->add_IRInstr(IRInstr::ret, {var}, &variables);
	//move to the last basic block
	cfg.current_bb->setExitTrue(cfg.final_bb);
	cfg.current_bb->setExitFalse(nullptr);
	cfg.current_bb=cfg.final_bb;
	cfg.final_bb->add_IRInstr(IRInstr::absolute_jump, {".endLabel"}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffDecVar(ifccParser::AffDecVarContext *ctx){
		string var = ctx->VAR()->getText();
		string type= ctx->type()->getText();
		/*Vérifier d'abord si cette variable a déjà été déclarée
		Dans ce cas, we throw an error*/
		if (variables.find(var) != variables.end()) {
			cerr << "Error: variable '" << var << "' already defined\n";
			throw "Error duplicate variable declaration";
		}
		//Sinon, on l'ajoute dans la table des symboles
		else{
			addVariable(var, type);
			variablesUsageCounter.insert({var,{type,0}});
		}
		/* On récupère la variable ou la constante qui se trouve en partie droite de l'affectation*/
		string varTmp = visit(ctx->expr()).as<string>();
		if(varTmp[0] == '$'){
			cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, varTmp}, &variables);
		}else
		{
			string str = convertCharToInt(varTmp);
			if(str==varTmp){
				cfg.current_bb->add_IRInstr(IRInstr::copy, {var, str}, &variables);
				return 0;
			}
			cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, str}, &variables);
		}
		return 0;
}
antlrcpp::Any CodeGenVisitor::visitAffVar(ifccParser::AffVarContext *ctx){
	string var = ctx->VAR()->getText();
	/*Si on essaie d'affecter une expression à une variable mais qu'elle n'a pas été déclarée, on throw une erreur*/
	if(!doesExist(var)){ //si variable inexistante dans la table
		//alors on a une erreur
		std::cerr << "Error: variable '" << var << "' undefined\n";
		throw "Error undefined variable";
	}
	/* On récupère la variable ou la constante qui se trouve en partie droite de l'affectation*/
	string varTmp = visit(ctx->expr()).as<string>();
	if(varTmp[0] == '$'){
		cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, varTmp}, &variables);
	}else
	{
		string str = convertCharToInt(varTmp);
		if(str==varTmp){
			cfg.current_bb->add_IRInstr(IRInstr::copy, {var, str}, &variables);
			return 0;
		}
		cfg.current_bb->add_IRInstr(IRInstr::ldconst, {var, str}, &variables);
	}
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffArrayConst(ifccParser::AffArrayConstContext *ctx){
	string var = ctx->VAR()->getText();
	/*Si variable inexistante dans la table, on throw une erreur*/
	if(!doesExist(var)){
		std::cerr << "Error : variable '" << var << "'undefined\n";
		throw "Error undefined var";
	}
	
	string index = ctx->CONST(0)->getText();
	string valeur = ctx->CONST(1)->getText();

	cfg.current_bb->add_IRInstr(IRInstr::afftab, {var,index ,valeur}, &variables);
	return 0;
}
antlrcpp::Any CodeGenVisitor::visitAffArrayVar(ifccParser::AffArrayVarContext *ctx){
	string var = ctx->VAR(0)->getText();
	/*Si variable inexistante dans la table, on throw une erreur*/
	if(!doesExist(var)){
		std::cerr << "Error : variable '" << var << "'undefined\n";
		throw "Error undefined var";
	}
	string index = ctx->CONST()->getText();
	string valeur = ctx ->VAR(1)->getText();

	cfg.current_bb->add_IRInstr(IRInstr::afftabvar, {var,valeur,index}, &variables);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitAffArrayExpr(ifccParser::AffArrayExprContext *ctx){
	string var = ctx->VAR()->getText();
	/*Si on essaie d'affecter une expression à une variable mais qu'elle n'a pas été déclarée, on throw une erreur*/
	if(!doesExist(var)){ //si variable inexistante dans la table
		//alors on a une erreur
		std::cerr << "Error: variable '" << var << "' undefined\n";
		throw "Error undefined variable";
	}
	
	visit(ctx -> expr());
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitVarExpr(ifccParser::VarExprContext *ctx)
{
	string var = visit(ctx->usedvar());
	variablesUsageCounter[var].second++;
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
	

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	if(isVariable(expr) && !doesExist(expr)){
		std::cerr << "Error: variable '" << expr << "' undefined\n";
		throw "Error undefined variable";
			
	}

	return expr;
}

antlrcpp::Any CodeGenVisitor::visitOrExpr(ifccParser::OrExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

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

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

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

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

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

	string varTmp = "!tmp" + varCounter;
	addVariable(varTmp);

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

	if(ctx->COMPAREOP()->getText() == ">"){
		cfg.current_bb->add_IRInstr(IRInstr::op_sup, {var1, var2, varTmp}, &variables);
	}if(ctx->COMPAREOP()->getText() == "<"){
		cfg.current_bb->add_IRInstr(IRInstr::op_min, {var1, var2, varTmp}, &variables);
	}
	resultStr = varTmp;
	
	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitEqualExpr(ifccParser::EqualExprContext *ctx){
	string resultStr = "";

	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	string varTmp = "!tmp" + varCounter;
	addVariable(varTmp);

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

	if(ctx->EQUALOP()->getText() == "=="){
		cfg.current_bb->add_IRInstr(IRInstr::op_equal, {var1, var2, varTmp}, &variables);
	}if(ctx->EQUALOP()->getText() == "!="){
		cfg.current_bb->add_IRInstr(IRInstr::op_diff, {var1, var2, varTmp}, &variables);
	}
	resultStr = varTmp;
	
	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitUnaryExpr(ifccParser::UnaryExprContext *ctx){
	string var = visit(ctx->expr());
	/*Si la variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	if(isVariable(var) && !doesExist(var)){
		std::cerr << "Error: variable '" << var << "' undefined\n";
		throw "Error undefined variable";	
	}

	string resultStr = "";
	if(var[0] == '$' ) {
		int val = stoi(var.substr(1));
		int result;
		if(ctx->unaryop()->getText() == "!"){
			result = !val;
		}
		else if(ctx->unaryop()->getText() == "-")
		{
			result = -val;
		}
		resultStr = "$" + to_string(result);

	}else{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		if(ctx->unaryop()->getText() == "!"){
			cfg.current_bb->add_IRInstr(IRInstr::op_not, {var, varTmp}, &variables);
		}
		else if(ctx->unaryop()->getText() == "-")
		{
			cfg.current_bb->add_IRInstr(IRInstr::op_neg, {var, varTmp}, &variables);
		}

		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitAddSubExpr(ifccParser::AddSubExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result;
		if(ctx->addsubop()->getText() == "+"){
			result = val1 + val2;
		}
		else if(ctx->addsubop()->getText() == "-")
		{
			result = val1 - val2;
		}
		resultStr = "$" + to_string(result);
	}
	else
	{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		if(ctx->addsubop()->getText() == "+"){
			cfg.current_bb->add_IRInstr(IRInstr::add, {var1, var2, varTmp}, &variables);
		}
		else if(ctx->addsubop()->getText() == "-")
		{
			cfg.current_bb->add_IRInstr(IRInstr::sub, {var1, var2, varTmp}, &variables);
		}
		resultStr = varTmp;
	}

	return resultStr;
}

antlrcpp::Any CodeGenVisitor::visitMultDivExpr(ifccParser::MultDivExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	/*Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur*/
	checkDeclaredExpr(var1, var2);

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		int result;
		if(ctx->multdivop()->getText() == "*"){
			result = val1 * val2;
		}
		else if(ctx->multdivop()->getText() == "/")
		{
			if (val2 == 0)
			{
				cerr<< "Error : Division by 0" << endl;
				throw "Division by 0";
			}
			result = val1 / val2;
		}
		else if(ctx->multdivop()->getText() == "%"){
			if (val2 == 0)
			{
				cerr<< "Error : Division by 0" << endl;
				throw "Division by 0";
			}
			result = val1 % val2;
		}
		resultStr = "$" + to_string(result);
	}
	else
	{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		if(ctx->multdivop()->getText() == "*"){
			cfg.current_bb->add_IRInstr(IRInstr::mul, {var1, var2, varTmp}, &variables);
		}
		else if(ctx->multdivop()->getText() == "/")
		{
			cfg.current_bb->add_IRInstr(IRInstr::div, {var1, var2, varTmp}, &variables);
		}
		else if(ctx->multdivop()->getText() == "%")
		{
			cfg.current_bb->add_IRInstr(IRInstr::mod, {var1, var2, varTmp}, &variables);
		}
		resultStr = varTmp;
	}

	return resultStr;
}

/*	
antlrcpp::Any CodeGenVisitor::visitModExpr(ifccParser::ModExprContext *ctx)
{
	string var1 = visit(ctx->expr(0));
	string var2 = visit(ctx->expr(1));

	//Si une variable est utilisée dans une expression et qu'elle n'a pas été déclarée alors c'est une erreur
	checkDeclaredExpr(var1, var2);

	string resultStr = "";

	if (var1[0] == '$' && var2[0] == '$')
	{
		int val1 = stoi(var1.substr(1));
		int val2 = stoi(var2.substr(1));
		if (val2 == 0)
		{
			std::cerr<< "Error : Division by 0" << endl;
			//throw "Division by 0";
		}
		int result = val1 % val2;
		resultStr = "$" + to_string(result);
	}
	else
	{
		string varTmp = "!tmp" + varCounter;
		addVariable(varTmp);

		cfg.current_bb->add_IRInstr(IRInstr::mod, {var1, var2, varTmp}, &variables);
		resultStr = varTmp;
	}

	return resultStr;
}*/

antlrcpp::Any CodeGenVisitor::visitListvar(ifccParser::ListvarContext *ctx)
{
	//récuperer la liste des variables 
	string var = ctx->getText();
	string delimiter = ",";
	string varValue;
	size_t pos = 0;
	string token;
	set<string> values;
	
	//split la liste des variables
	while ((pos = var.find(delimiter)) != string::npos) {
		token = var.substr(0, pos);
		varValue = "0";
		values.insert(token);
		variablesUsageCounter[token].second = 0;//ajout d'une variable non utilisée
		cfg.current_bb->add_IRInstr(IRInstr::decl, {token, varValue}, &variables);
		var.erase(0, pos + delimiter.length());
	}
	varValue = "0";
	values.insert(var);
	variablesUsageCounter[var].second = 0;//ajout d'une variable non utilisée
	
	cfg.current_bb->add_IRInstr(IRInstr::decl, {var, varValue}, &variables);
	return values;
}

antlrcpp::Any CodeGenVisitor::visitUsedvar(ifccParser::UsedvarContext *ctx) {
	string var = ctx->VAR()->getText();
	return var;
};

antlrcpp::Any CodeGenVisitor::visitAlphabets(ifccParser::AlphabetsContext *ctx) {
	string var = "@" + ctx->ABCD()->getText();
	return var;
};

antlrcpp::Any CodeGenVisitor::visitIncrdecr(ifccParser::IncrdecrContext *ctx)
{
	// TODO: indiquer que la variable est utilisée
	string var = ctx->VAR()->getText();
	string varTmp = "!tmp" + varCounter;
	addVariable(varTmp);

	string op = ctx->incrdecrop()->getText();

	if (op == "++")
	{
		cfg.current_bb->add_IRInstr(IRInstr::add, {var, "$1", varTmp}, &variables);
	}
	else if (op == "--")
	{
		cfg.current_bb->add_IRInstr(IRInstr::sub, {var, "$1", varTmp}, &variables);
	}
	cfg.current_bb->add_IRInstr(IRInstr::copy, {var, varTmp}, &variables);

	return var;
}

antlrcpp::Any CodeGenVisitor::visitIncrdecrExpr(ifccParser::IncrdecrExprContext *ctx)
{
	string var = visit(ctx->incrdecr());
	return var;
}

antlrcpp::Any CodeGenVisitor::visitAddAffect(ifccParser::AddAffectContext *ctx)
{
	string var = ctx->VAR()->getText();
	// TODO: verifier que la variable est déclarée, et indiquer qu'elle est utilisée
	string var2 = visit(ctx->expr());

	string varTmp = "!tmp" + varCounter;
	addVariable(varTmp);

	cfg.current_bb->add_IRInstr(IRInstr::add, {var, var2, varTmp}, &variables);
	cfg.current_bb->add_IRInstr(IRInstr::copy, {var, varTmp}, &variables);

	return var;
}

antlrcpp::Any CodeGenVisitor::visitSubAffect(ifccParser::SubAffectContext *ctx)
{
	string var = ctx->VAR()->getText();
	// TODO: verifier que la variable est déclarée, et indiquer qu'elle est utilisée
	string var2 = visit(ctx->expr());
	string varTmp = "!tmp" + varCounter;
	addVariable(varTmp);

	cfg.current_bb->add_IRInstr(IRInstr::sub, {var, var2, varTmp}, &variables);
	cfg.current_bb->add_IRInstr(IRInstr::copy, {var, varTmp}, &variables);

	return var;
}

antlrcpp::Any CodeGenVisitor::visitIfStatement(ifccParser::IfStatementContext *ctx) 
{
	/* Basic Block courant*/
	BasicBlock *testBB = cfg.current_bb;

	/* On visite l'expression de la condition du if. On recupère le nom de la variable temporaire qui contient la valeur booléenne de cette expression*/
	string conditionEval = visit(ctx->expr());

	/* On stocke dans l'attribut TestVarName le nom de cette variable temporaire */
	testBB->setTestVarName(conditionEval);

	/* Création d'un basic block correspondant au then*/
	BasicBlock *thenBB = cfg.createBB();

	/* Création d'un basic block correspondant au endIf */
	BasicBlock *endIfBB = cfg.createBB();

	/* endIfBB recupère les successeurs de testBB*/
	endIfBB->setExitTrue(testBB->getExitTrue());
	endIfBB->setExitFalse(testBB->getExitFalse());

	/* Dans le cas où l'expression du if est True, le bloc suivant du bloc testBB est thenBB*/
	testBB->setExitTrue(thenBB);

	/* Blocs suivants de thenBB*/
	thenBB->setExitTrue(endIfBB);
	thenBB->setExitFalse(nullptr);

	/* Vérification de la présence d'un elseStatement*/
	bool hasElseStatement = ctx->elseStatement();
	if (hasElseStatement)
	{
		/* Création d'un basic block correspondant au else*/
		BasicBlock *elseBB = cfg.createBB();

		/* Dans le cas où l'expression du if est False, le bloc suivant du bloc courant est elseBB*/
		testBB->setExitFalse(elseBB);

		/* Successeurs de elseBB*/
		elseBB->setExitTrue(endIfBB);
		elseBB->setExitFalse(nullptr);

		/* On remplit le bloc elseBB avec ses intructions*/
		cfg.current_bb = elseBB;
		visit(ctx->elseStatement());
	}
	else
	{
		/* S'il n'y a pas de else et dans le cas où l'expression du if est False, le bloc suivant du bloc courant est endIfBB*/
		testBB->setExitFalse(endIfBB);
	}

	/* On remplit le bloc thenBB avec ses intructions*/
	cfg.current_bb = thenBB;
	visit(ctx->blocInstr());
	/* On ajoute l'instruction qui permet de sauter au bloc qui suit thenBB */
	thenBB->add_IRInstr(IRInstr::absolute_jump, {thenBB->getExitTrue()->getLabel()}, &variables);

	/* On ajoute au bloc testBB l'instruction qui permet de savoir vers quel bloc on saute depuis testBB */
	testBB->add_IRInstr(IRInstr::conditional_jump, {testBB->getTestVarName(), testBB->getExitTrue()->getLabel(), testBB->getExitFalse()->getLabel()}, &variables);

	/* On ajoute l'instruction qui permet de sauter au bloc qui suit endIfBB */
	endIfBB->add_IRInstr(IRInstr::absolute_jump, {endIfBB->getExitTrue()->getLabel()}, &variables);
	
	/* A la fin du if...else, le bloc courant est le bloc qui suit endIfBB */
	cfg.current_bb = endIfBB->getExitTrue();

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitElseStatement(ifccParser::ElseStatementContext *ctx) 
{
	BasicBlock* elseBB = cfg.current_bb;

	/* On remplit le bloc elseBB avec ses intructions*/
	visit(ctx->blocInstr());

	/* On ajoute l'instruction qui permet de sauter au bloc qui suit thenBB */
	elseBB->add_IRInstr(IRInstr::absolute_jump, {elseBB->getExitTrue()->getLabel()}, &variables);
	
	return 0; 
}