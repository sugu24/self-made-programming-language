#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include<cstdio>
#include<cstdlib>
#include<map>
#include<tuple>
#include<string>
#include<vector>
#include<llvm/ADT/APInt.h>
#include<llvm/Constants.h>
#include<llvm/ExecutionEngine/ExecutionEngine.h>
#include<llvm/ExecutionEngine/JIT.h>
#include<llvm/Linker.h>
#include<llvm/LLVMContext.h>
#include<llvm/Module.h>
#include<llvm/Metadata.h>
#include<llvm/Support/Casting.h>
#include<llvm/IRBuilder.h>
#include<llvm/Support/IRReader.h>
#include<llvm/MDBuilder.h>
#include<llvm/ValueSymbolTable.h>
#include"APP.hpp"
#include"AST.hpp"

/**
 * コード生成クラス
 */
class CodeGen{
	private:
		llvm::Function *CurFunc;    // 現在コード生成中のFunction
		llvm::Module *Mod;          // 生成したModuleを格納
		llvm::IRBuilder<> *Builder; // LLVM-IRを生成するIRBuilder
		
	public:
		CodeGen();
		~CodeGen();
		bool doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit);
		llvm::Module &getModule();
		bool CORRECT = true;

	private:
		bool generateTranslationUnit(TranslationUnitAST &tunit, std::string name);
		llvm::Function *generateFunctionDefinition(FunctionAST *func, llvm::Module *mod);
		llvm::Function *generatePrototype(PrototypeAST *proto, llvm::Module *mod);
		llvm::Value *generateFunctionStatement(FunctionStmtAST *func_stmt, llvm::Function *func);
		llvm::Value *generateVariableDeclaration(VariableDeclAST *vdecl);
		llvm::Value *generateStatement(BaseAST *stmt, FunctionStmtAST *func_stmt);
		llvm::Value *generateBinaryExpression(BinaryExprAST *bin_expr, FunctionStmtAST *func_stmt);
		bool generateDenominatorCheck(std::string op, BaseAST *rhs, int line, FunctionStmtAST *func_stmt);
		llvm::Value *generateCallExpression(CallExprAST *call_expr, FunctionStmtAST *func_stmt);
		llvm::Value *generateReturnStatement(ReturnStmtAST *jump_stmt, FunctionStmtAST *func_stmt);
		llvm::Value *generateVariable(VariableAST *var, FunctionStmtAST *func_stmt);
		llvm::Value *generateNumber(double value);
		llvm::Value *generateString(std::string str);
		bool linkModule(llvm::Module *dest, std::string file_name);

		llvm::Value *generateComparison(BaseAST *lhs, BaseAST *rhs, std::string op, FunctionStmtAST *func_stmt);
		llvm::Value *generateForStatement(ForStatementAST *for_expr, llvm::BasicBlock *bcond, llvm::BasicBlock *bbody, FunctionStmtAST *func_stmt);
		llvm::BasicBlock *generateForEndStatement(llvm::BasicBlock *bcond, llvm::BasicBlock *bbody, llvm::BasicBlock *binc, llvm::BasicBlock *bend, llvm::Value *fcmp, ForStatementAST *for_expr, FunctionStmtAST *func_stmt);
};

#endif
