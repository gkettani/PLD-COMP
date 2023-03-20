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
		virtual antlrcpp::Any visitOrExpr(ifccParser::OrExprContext *ctx) override;
		virtual antlrcpp::Any visitXorExpr(ifccParser::XorExprContext *ctx) override;
		virtual antlrcpp::Any visitAndExpr(ifccParser::AndExprContext *ctx) override;
		virtual antlrcpp::Any visitNotExpr(ifccParser::NotExprContext *ctx) override;
    	virtual antlrcpp::Any visitNegExpr(ifccParser::NegExprContext *ctx) override;
    
	protected:
		CFG& cfg;
		map<string, int> variables;
		int varCounter = 0;

};

#endif
