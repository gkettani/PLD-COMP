#ifndef CODEGENVISITOR_H
#define CODEGENVISITOR_H

#include "IR/CFG.h"
#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>

using namespace std;

class  CodeGenVisitor : public ifccBaseVisitor {
	public:
		CodeGenVisitor(CFG &cfg);
		~CodeGenVisitor();

		virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
		virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext *context) override;
		virtual antlrcpp::Any visitType(ifccParser::TypeContext *context) override;
		virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *context) override;
		virtual antlrcpp::Any visitRetVar(ifccParser::RetVarContext *context) override;
		virtual antlrcpp::Any visitRetConst(ifccParser::RetConstContext *context) override;
		virtual antlrcpp::Any visitRetExpr(ifccParser::RetExprContext *context) override;
		virtual antlrcpp::Any visitRetNothing(ifccParser::RetNothingContext *context) override;
		virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *context) override;
		virtual antlrcpp::Any visitVarExpr(ifccParser::VarExprContext *ctx) override;
		virtual antlrcpp::Any visitConstExpr(ifccParser::ConstExprContext *ctx) override;
		virtual antlrcpp::Any visitParExpr(ifccParser::ParExprContext *ctx) override;
		virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
		virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
		virtual antlrcpp::Any visitAlphabets(ifccParser::AlphabetsContext *ctx) override;
		virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
		virtual antlrcpp::Any visitUnaryExpr(ifccParser::UnaryExprContext *ctx) override;
		virtual antlrcpp::Any visitAddSubExpr(ifccParser::AddSubExprContext *ctx) override;
		virtual antlrcpp::Any visitMultDivExpr(ifccParser::MultDivExprContext *ctx) override;
		virtual antlrcpp::Any visitCompareExpr(ifccParser::CompareExprContext *context) override;
		virtual antlrcpp::Any visitEqualExpr(ifccParser::EqualExprContext *context) override;
		virtual antlrcpp::Any visitListvar(ifccParser::ListvarContext *ctx) override;
		virtual antlrcpp::Any visitUsedvar(ifccParser::UsedvarContext *context) override;
		virtual antlrcpp::Any visitIncrdecr(ifccParser::IncrdecrContext *context) override;
		virtual antlrcpp::Any visitIncrdecrExpr(ifccParser::IncrdecrExprContext *context) override;


	protected:
		CFG& cfg;
		map<string, pair<string,int>> variables;
		map<string, pair<string,int>> variablesUsageCounter;
		int varCounter = 0;

		void addVariable(string name,string type = "int", int size = 4);
		bool doesExist(string var);
		bool isVariable(string var);
		void checkDeclaredExpr(string var1, string var2);

};

#endif
