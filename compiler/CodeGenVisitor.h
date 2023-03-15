#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include <map>


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
		virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;

    virtual antlrcpp::Any visitDeclare(ifccParser::DeclareContext *context) override;

    virtual antlrcpp::Any visitType(ifccParser::TypeContext *context) override;

    virtual antlrcpp::Any visitInstruction(ifccParser::InstructionContext *context) override;

    virtual antlrcpp::Any visitRet(ifccParser::RetContext *context) override;

	protected:
		std::map<std::string, int> variables;
		int varCounter = 0;



};
