#ifndef PARSER_HPP
#define PARSER_HPP

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "APP.hpp"
#include "AST.hpp"
#include "lexer.hpp"

/**
 * 構文解析・意味解析クラス
 */
class Parser{
	private:
		TokenStream *Tokens;
		TranslationUnitAST *TU;
		
		// 意味解析用各種識別子標
		std::vector<std::string> VariableTable;
		std::map<std::string, int> PrototypeTable;
		std::map<std::string, int> FunctionTable;
	public:
		Parser(std::string filename);
		~Parser() {
			SAFE_DELETE(TU);
			SAFE_DELETE(Tokens);
		}
		bool doParse();
		TranslationUnitAST &getAST();

	private:
		/**
		 * 各種構文解析メソッド
		 */
		bool visitTranslationUnit();
		bool visitExternalDeclaration(TranslationUnitAST *tunit);
		PrototypeAST *visitFunctionDeclaration();
		FunctionAST *visitFunctionDefinition();
		PrototypeAST *visitPrototype();
		FunctionStmtAST *visitFunctionStatement(PrototypeAST *proto);
		VariableDeclAST *visitVariableDeclaration();
		BaseAST *visitStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitExpressionStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitReturnStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitAssignmentExpression(FunctionStmtAST *func_stmt);
		BaseAST *visitAdditiveExpression(BaseAST *lhs, FunctionStmtAST *func_stmt);
		BaseAST *visitMultiplicativeExpression(BaseAST *lhs, FunctionStmtAST *func_stmt);
		BaseAST *visitPostfixExpression(FunctionStmtAST *func_stmt);
		BaseAST *visitPrimaryExpression(FunctionStmtAST *func_stmt);
		BaseAST *visitGlobalStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitIfStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitIfEndStatement();
		ForStatementAST *visitForStatement(FunctionStmtAST *func_stmt);
		BaseAST *visitForEndStatement();
		BaseAST *visitBreakStatement();
		BaseAST *visitContinueStatement();
};

#endif

