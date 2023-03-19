#include "CodeGenVisitor.h"
using namespace std;

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
	cout<<".globl	_main\n"
		" _main: \n"
		" 	# prologue \n"
		" 	pushq %rbp # save %rbp on the stack \n"
		" 	movq %rsp, %rbp # define %rbp for the current function \n";
		// " 	movl	$"<<retval<<", %eax\n"
		visitChildren(ctx);
		cout<<" 	# epilogue \n"
		" 	popq %rbp # restore %rbp from the stack \n"
		" 	ret\n";

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitInstruction(ifccParser::InstructionContext *ctx)
{
	cout << " 	# instruction \n";
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitType(ifccParser::TypeContext *ctx)
{
	cout << " 	# type \n";
	visitChildren(ctx);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclare(ifccParser::DeclareContext *ctx)
{
	int retval = ctx->CONST() ? stoi(ctx->CONST()->getText()) : 0;
	varCounter += 1;
	int offset = varCounter * -4;
	variables[ctx->VAR()->getText()] = offset;
	cout<<" 	movl	$"<<retval<<", "<< offset <<"(%rbp)\n";
	
	visitChildren(ctx);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitRet(ifccParser::RetContext *ctx)
{
	if (ctx->VAR()) {
		string var = ctx->VAR()->getText();
		int offset = variables[var];
		cout << " 	movl	" << offset << "(%rbp), %eax\n";
	
	}

	if (ctx->CONST()) {
		string var = ctx->CONST()->getText();
		cout << " 	movl	$" << var << ", %eax\n";
	}

	// cout << var << endl;
	// offset = variables[var];
	// visitChildren(ctx);
	return 0;
}
antlrcpp::Any CodeGenVisitor::visitAffectation(ifccParser::AffectationContext *ctx)
{
	cout << " 	# Affectation \n";
	//Exemple pour mieux comprendre : Pour int b=a
	//On va extraire l'adresse de 'a' de la table 'variables'
	string  var = ctx->VAR()[1]->getText();
	int offset = variables[var];

	//je mets le contenu de a dans le registre eax
	cout << " 	movl	" << offset << "(%rbp), %eax\n";

	//je mets le contenu du registre eax dans b : b n'a pas encore 
	//d'adresse, il faut donc pouvoir lui en donner une sans oublier
	//de l'enregistrer dans la table 'variables'
	
	varCounter += 1;
	offset = varCounter * -4;
	variables[ctx->VAR()[0]->getText()] = offset;

	cout << "	movl 	%eax, " << offset << "(%rbp)\n" ;
	visitChildren(ctx);

	return 0;

}

antlrcpp::Any CodeGenVisitor::visitSup(ifccParser::SupContext *ctx){
	cout<<" # comparaison superieur  \n";
	string var1 = ctx->expr()[0]->getText();
	string var2 = ctx->expr()[2]->getText();
	
	int offset1 = variables[var1];
	int offset2 = variables[var2];

	cout << " 	movl	" << offset1 << "(%rbp), %eax\n";
	cout << " 	movl	" << offset2 << "(%rbp), %ebx\n";

	visitChildren(ctx);

	return 0;
}

antlrcpp::Any CodeGenVisitor::visitMin(ifccParser::MinContext *context){
	visitChildren(context);
	return 0;

}

antlrcpp::Any CodeGenVisitor::visitDiff(ifccParser::DiffContext *context){
	visitChildren(context);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitEqual(ifccParser::EqualContext *context) {
	visitChildren(context);
	return 0;
}

antlrcpp::Any CodeGenVisitor::visitParComparisons(ifccParser::ParComparisonsContext *context){
	visitChildren(context);
	return 0;
}