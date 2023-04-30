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
		virtual antlrcpp::Any visitStartMainBlock(ifccParser::StartMainBlockContext *ctx) override;
		virtual antlrcpp::Any visitEndMainBlock(ifccParser::EndMainBlockContext *ctx) override;
		virtual antlrcpp::Any visitListVarDec(ifccParser::ListVarDecContext *ctx) override;
		virtual antlrcpp::Any visitArrayDec(ifccParser::ArrayDecContext *ctx) override;
		virtual antlrcpp::Any visitType(ifccParser::TypeContext *ctx) override;
		virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *ctx) override;
		virtual antlrcpp::Any visitRetVar(ifccParser::RetVarContext *ctx) override;
		virtual antlrcpp::Any visitRetExpr(ifccParser::RetExprContext *ctx) override;
		virtual antlrcpp::Any visitRetNothing(ifccParser::RetNothingContext *ctx) override;
		virtual antlrcpp::Any visitAffDecVar(ifccParser::AffDecVarContext *ctx) override;
		virtual antlrcpp::Any visitAffVar(ifccParser::AffVarContext *ctx) override;
		virtual antlrcpp::Any visitAffArrayConst(ifccParser::AffArrayConstContext *ctx) override;
		virtual antlrcpp::Any visitAffArrayVar(ifccParser::AffArrayVarContext *ctx) override;
		virtual antlrcpp::Any visitAffArrayExpr(ifccParser::AffArrayExprContext *ctx) override;
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
		virtual antlrcpp::Any visitCompareExpr(ifccParser::CompareExprContext *ctx) override;
		virtual antlrcpp::Any visitEqualExpr(ifccParser::EqualExprContext *ctx) override;
		virtual antlrcpp::Any visitListvar(ifccParser::ListvarContext *ctx) override;
		virtual antlrcpp::Any visitUsedvar(ifccParser::UsedvarContext *ctx) override;
		virtual antlrcpp::Any visitIncdec(ifccParser::IncdecContext *ctx) override;
		virtual antlrcpp::Any visitIncdecExpr(ifccParser::IncdecExprContext *ctx) override;
		virtual antlrcpp::Any visitAddAffect(ifccParser::AddAffectContext *ctx) override;
		virtual antlrcpp::Any visitSubAffect(ifccParser::SubAffectContext *ctx) override;
		virtual antlrcpp::Any visitIfStatement(ifccParser::IfStatementContext *ctx) override;
		virtual antlrcpp::Any visitElseStatement(ifccParser::ElseStatementContext *ctx) override;
		virtual antlrcpp::Any visitWhileStatement(ifccParser::WhileStatementContext *ctx) override;
		virtual antlrcpp::Any visitPutcharVar(ifccParser::PutcharVarContext *ctx) override;
		virtual antlrcpp::Any visitPutcharChar(ifccParser::PutcharCharContext *ctx) override;
		virtual antlrcpp::Any visitGetchar(ifccParser::GetcharContext *ctx) override;

	protected:
		CFG& cfg;
		map<string, pair<string,int>> variables;
		map<string, pair<string,int>> variablesUsageCounter;
		
		int varCounter = 0;

		void addVariable(string name,string type = "int", int size = 4);
		void addArray(string name, string type, int size);
		bool doesExist(string var);
		bool isVariable(string var);
		void checkDeclaredExpr(string var1, string var2);
		string convertCharToInt(string var);
};

#endif
