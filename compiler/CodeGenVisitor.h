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
		virtual antlrcpp::Any visitRet(ifccParser::RetContext *context) override;
		virtual antlrcpp::Any visitAffectation(ifccParser::AffectationContext *context) override;
		virtual antlrcpp::Any visitVarExpr(ifccParser::VarExprContext *ctx) override;
		virtual antlrcpp::Any visitConstExpr(ifccParser::ConstExprContext *ctx) override;
		virtual antlrcpp::Any visitParExpr(ifccParser::ParExprContext *ctx) override;
		virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
		virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
		virtual antlrcpp::Any visitAlphabets(ifccParser::AlphabetsContext *ctx) override;
		virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
		virtual antlrcpp::Any visitNotExpr(ifccParser::NotExprContext *ctx) override;
		virtual antlrcpp::Any visitNegExpr(ifccParser::NegExprContext *ctx) override;
		virtual antlrcpp::Any visitMultExpr(ifccParser::MultExprContext *ctx) override;
		virtual antlrcpp::Any visitPlusExpr(ifccParser::PlusExprContext *ctx) override;
		virtual antlrcpp::Any visitMinusExpr(ifccParser::MinusExprContext *ctx) override;
		virtual antlrcpp::Any visitCompareExpr(ifccParser::CompareExprContext *context) override;
		virtual antlrcpp::Any visitEqualExpr(ifccParser::EqualExprContext *context) override;
		virtual antlrcpp::Any visitListvar(ifccParser::ListvarContext *ctx) override;
		virtual antlrcpp::Any visitUsedvar(ifccParser::UsedvarContext *context) override;
		virtual antlrcpp::Any visitDivExpr(ifccParser::DivExprContext *context) override;


	protected:
		CFG& cfg;
		map<string, int> variables;
		map<string, int> variablesUsageCounter;
		int varCounter = 0;

		void addVariable(string name, int size = 4);

};

#endif
