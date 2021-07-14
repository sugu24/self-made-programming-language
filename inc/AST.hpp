#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <map>
#include <vector>
#include <llvm/Support/Casting.h>
#include "APP.hpp"

/*
 * クラス宣言
 */
class BaseAST;
class TranslationUnitAST;
class FunctionAST;
class PrototypeAST;
class FunctionStmtAST;
class VariableDeclAST;
class BinaryExprAST;
class NullExprAST;
class CallExprAST;
class ReturnStmtAST;
class IfStatementAST;
class IfEndAST;
class ForStatementAST;
class ForEndAST;
class ComparisonAST;
class GlobalVariableAST;
class VariableAST;
class NumberAST;
class StringAST;
class BreakAST;
class ContinueAST;
class NewLineAST;

/**
 * ASTの種類
 */
enum AstID{
	BaseID,         // 0
	VariableDeclID, // 1
	BinaryExprID,   // 2
	NullExprID,     // 3 
	CallExprID,     // 4
	ReturnStmtID,   // 5
	IfID,           // 6
	IfEndID,        // 7
	ForID,          // 8
	ForEndID,       // 9
	ComparisonID,   // 10
	GlobalVariableID,//11
	VariableID,     // 12
	NumberID,       // 13
	StringID,       // 14
	BreakID,        // 15
	ContinueID,     // 16
	NewLineID       // 17
};

/**
 * ASTの基底クラス
 */
class BaseAST{
	AstID ID;

	public:
	BaseAST(AstID id):ID(id){}
	virtual ~BaseAST(){}
	AstID getValueID() const {return ID;}
};

/**
 * ソースコードを表すAST
 */
class TranslationUnitAST{
	std::vector<PrototypeAST*> Prototypes;
	std::vector<FunctionAST*> Functions;

	public:
		TranslationUnitAST(){}
		~TranslationUnitAST();

		// モジュールにプロトタイプ宣言を追加する
		bool addPrototype (PrototypeAST *proto);

		// モジュールに関数を追加する
		bool addFunction(FunctionAST *func);

		// モジュールがからか判定する
		bool empty();

		// i番目のプロトタイプ宣言を取得する
		PrototypeAST *getPrototype(int i){
			if (i < Prototypes.size())
				return Prototypes.at(i);
			else
				return NULL;
		}

		// i番目の関数を取得する
		FunctionAST *getFunction(int i){
			if(i < Functions.size())
				return Functions.at(i);
			else
				return NULL;
		}
};

/**
 * 関数宣言を表すAST
 */
class PrototypeAST{
	private:
		std::string FuncIdentify;
		std::string FuncName;
		std::vector<std::string> ParamsName;
		std::vector<std::string> ParamsIdentify;

	public:
		PrototypeAST (const std::string &func_id, 
				const std::string &func_name, 
				const std::vector<std::string> &params_name, 
				const std::vector<std::string> &params_identify)
			: FuncIdentify(func_id), FuncName(func_name), 
			ParamsName(params_name), ParamsIdentify(params_identify){}
		

		// 関数名を取得する
		std::string getName(){return FuncName;}

		// 関数の戻り値の型を取得する
		std::string getIdentify(){return FuncIdentify;}

		// i番目の引数名を取得する
		std::string getParamName(int i){if (i<ParamsName.size()) 
			return ParamsName.at(i); return NULL;}
		
		// i番目の引数の型（int double string）を取得する
		std::string getParamIdentify(int i){if(i<ParamsIdentify.size()) 
			return ParamsIdentify.at(i); return NULL;}

		// 引数の数を取得する
		int getParamNum(){return ParamsName.size();}
};

/**
 * 関数定義を表すAST
 */
class FunctionAST{
	PrototypeAST *Proto;
	FunctionStmtAST *Body;
	
	public:
	FunctionAST(PrototypeAST *proto, FunctionStmtAST *body) : Proto(proto), Body(body){}
	~FunctionAST();

	// 関数名を取得する
	std::string getName(){return Proto->getName();}

	// この関数のプロトタイプ宣言を取得する
	PrototypeAST *getPrototype(){return Proto;}

	// この関数のボディを取得する
	FunctionStmtAST *getBody(){return Body;}
};

/**
 * 関数定義（ボディ）を表すAST
 */
class FunctionStmtAST{
	std::vector<VariableDeclAST*> VariableDecls;
	std::vector<std::string> GlobalVariables;
	std::vector<BaseAST*> StmtLists;

	public:
	FunctionStmtAST(){}
	~FunctionStmtAST();

	// 関数に変数を追加する
	bool addVariableDeclaration(VariableDeclAST *vdecl);

	// 関数にステートメントを追加する
	bool addStatement(BaseAST *stmt){StmtLists.push_back(stmt); return true;}

	bool addGlobalVariables(std::string str){GlobalVariables.push_back(str); return true;}

	// i番目の変数を取得する
	VariableDeclAST *getVariableDecl(int i){if(i<VariableDecls.size()) return VariableDecls.at(i); else return NULL;}

	// i番目のステートメントを取得する
	BaseAST *getStatement(int i){if(i<StmtLists.size()) return StmtLists.at(i); else return NULL;}
	
	bool isGlobalVariable(std::string name){
		for(int i = 0; i < GlobalVariables.size(); i++){
			if(GlobalVariables.at(i) == name)
				return true;
		}
		return false;
	}
};

/**
 * 変数宣言を表すAST
 */
class VariableDeclAST: public BaseAST{
	public:
		typedef enum{ param, local }DeclType;
		typedef enum{ dint, ddouble, string }IdentifyType;
	
	private:
		std::string Name;
		DeclType Type;
		IdentifyType Identify;

	public:
		VariableDeclAST(const std::string &name, const std::string &identify) 
			: BaseAST(VariableDeclID),Name(name){
			if(identify == "int")
				Identify = dint;
			else if(identify == "double")
				Identify = ddouble;
			else if(identify == "string")
				Identify = string;
			else
				fprintf(stderr, "%s is not able to be identified\n", identify.c_str());
		}

		// VariableDeclASTなのでtrueを返す
		static inline bool classof (VariableDeclAST const*){return true;}

		// 渡されたBaseASTクラスがVarialbeDeclASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == VariableDeclID;
		}
		~VariableDeclAST(){}

		// 変数名を取得する
		std::string getName(){return Name;}

		//変数の宣言種別を設定する
		bool setDeclType(DeclType type){Type = type; return true;}

		// 変数の宣下種別を取得する
		DeclType getType(){return Type;}

		// 変数の識別を取得する
		IdentifyType getIdentify(){return Identify;}
};

/**
 * 二項演算を表すAST
 */
class BinaryExprAST : public BaseAST{
	std::string Op;
	BaseAST *LHS, *RHS;
	int Line;
	std::string Width;
	std::string Digit;

	public:
		BinaryExprAST(std::string op, BaseAST *lhs, BaseAST *rhs, int line) 
			: BaseAST(BinaryExprID),Op(op),LHS(lhs),RHS(rhs),Line(line){
			Width = "";
			Digit = "3";				
		}
		~BinaryExprAST(){SAFE_DELETE(LHS);SAFE_DELETE(RHS);}

		// BinaryExprASTなのでtrueを返す
		static inline bool classof(BinaryExprAST const* base){return true;}

		// 渡されたBaseASTがBinaryExprASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == BinaryExprID;
		}
		
		// Widthを設定
		bool setWidth(std::string i){Width = i; return true;}

		// digitを設定
		bool setDigit(std::string i){Digit = i;return true;}

		// Widthを取得
		std::string getWidth(){return Width;}

		// Digitを取得
		std::string getDigit(){return Digit;}

		// 演算子を取得する
		std::string getOp(){return Op;}

		// 左辺値を取得
		BaseAST *getLHS(){return LHS;}

		// 右辺値を取得
		BaseAST *getRHS(){return RHS;}

		int getLine(){return Line;}	
};

/**
 * 関数呼び出しを表すAST
 */
class CallExprAST : public BaseAST{
	std::string Callee;
	std::vector<BaseAST*> Args;
	std::string Width;
	std::string Digit;

	public:
		CallExprAST(const std::string &callee, std::vector<BaseAST*> &args)
			: BaseAST(CallExprID),Callee(callee),Args(args){
			Width = "";
			Digit = "3";
		}
		~CallExprAST();

		// CallExprASTなのでtrueを返す
		static inline bool classof (CallExprAST const*){return true;}

		// 渡されたBaseASTがCallExprASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == CallExprID;
		}

		// widthを設定
		bool setWidth(std::string i){Width = i; return true;}
		
		// // digitを設定
		bool setDigit(std::string i){Digit = i;return true;}
		
		// // Widthを取得
		std::string getWidth(){return Width;}
		
		// // Digitを取得
		std::string getDigit(){return Digit;}

		// 呼び出す関数名を取得する
		std::string getCallee(){return Callee;}

		// i番目の引数を取得する
		BaseAST *getArgs (int i){if(i<Args.size())return Args.at(i);else return NULL;}
};

/**
 * returnを表すAST
 * (returnの変数を扱う？)
 */
class ReturnStmtAST : public BaseAST {
	BaseAST *Expr;
	public:
		ReturnStmtAST(BaseAST *expr) : BaseAST(ReturnStmtID),Expr(expr){}
		~ReturnStmtAST(){SAFE_DELETE(Expr);}

		// ReturnStmtASTなのでtrueを返す
		static inline bool classof(ReturnStmtAST const*){return true;}

		// 渡されたBaseASTがReturnStmtASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == ReturnStmtID;
		}

		// returnで返すExpressionを取得する
		BaseAST *getExpr(){return Expr;}
};

/**
 * ifを表すAST
 */
class IfStatementAST : public BaseAST{
	std::string If;                   // if or else if or else 
	std::vector<ComparisonAST*> Coms; // 比較
	std::vector<std::string> Ops;     // 条件式の結合
	std::vector<int> Depth;

	public:
	IfStatementAST() : BaseAST(IfID){}
	~IfStatementAST();

	// IfStatementASTなのでtrueを返す
	static inline bool classof(IfStatementAST const*){return true;}

	// 渡されたBaseASTがIfStatementASTか判定する
	static inline bool classof(BaseAST const* base){return base->getValueID() == IfID;}
	
	// if else の文字列を設定する
	bool setIf(std::string ifelse){If = ifelse;return true;}

	// ifの条件式を追加する
	bool addComparison(ComparisonAST *com){Coms.push_back(com); return true;}
	
	// opを追加する
	bool addOp(std::string op){Ops.push_back(op);return true;}

	// depthを追加
	bool addDepth(int i){Depth.push_back(i);return true;}

	// i番目のopのdepthを取得
	int getDepth(int i){if(Depth.size() > i) return Depth.at(i);else return 0;}

	// if elseを取得する
	std::string getIf(){return If;}

	// i番目の&& ||を取得する
	std::string getOp(int i){if(i<Ops.size()) return Ops.at(i); else return NULL;}

	// i番目の比較を取得する
	ComparisonAST *getComparison(int i){if(i<Coms.size()) return Coms.at(i); else return NULL;}

	// 比較の個数を取得
	int getComparisonNumber(){return Coms.size();}
	
	// 結合の個数を取得
	int getOpsNumber(){return Ops.size();}
};

/**
 *  ifの終わりを表すAST
 */
class IfEndAST : public BaseAST {
	public:
	IfEndAST() : BaseAST(IfEndID){}
	static inline bool classof(IfEndAST const*){return true;}
	static inline bool classof(BaseAST const* base){
		return base->getValueID() == IfEndID;
	}
};

/**
 * forを表すAST
 */
class ForStatementAST : public BaseAST{
	BinaryExprAST *BinExpr;
	VariableAST *Val;
	BaseAST *EndExpr;

	public:
		ForStatementAST(VariableAST *val, BinaryExprAST *bin_expr, BaseAST *end_expr)
		: BaseAST(ForID), Val(val), BinExpr(bin_expr), EndExpr(end_expr){}
		~ForStatementAST(){}
		
		// ForStatementASTなのでtrueを返す
		static inline bool classof(ForStatementAST const* base){return true;}

		// 渡されたASTがForStatementASTか判定する
		static inline bool classof(BaseAST const* base){return base->getValueID() == ForID;}
		
		// Valを取得
		VariableAST *getVal(){return Val;}

		// BinExprを取得
		BinaryExprAST *getBinExpr(){return BinExpr;}

		// EndNumberを取得
		BaseAST *getEndExpr(){return EndExpr;}
};

/**
 * forEndを表すAST
 */
class ForEndAST : public BaseAST {
	public:
		ForEndAST() : BaseAST(ForEndID){}
		static inline bool classof(ForEndAST const*){return true;}
		static inline bool classof(BaseAST const* base){return base->getValueID() == ForEndID;}
};


/**
 * 条件式を表すAST
 */
class ComparisonAST : public BaseAST{
	std::string Op;
	BaseAST *LHS, *RHS;

	public:
	ComparisonAST(std::string op, BaseAST *lhs, BaseAST *rhs) 
		: BaseAST(ComparisonID), Op(op), LHS(lhs), RHS(rhs){}
	~ComparisonAST(){SAFE_DELETE(LHS);SAFE_DELETE(RHS);}

	// ComparisonASTなのでtrueを返す
	static inline bool classof(ComparisonAST const* base){return true;}

	// 渡されたBaseASTがComparisonASTか判定する
	static inline bool classof(BaseAST const* base){return base->getValueID() == ComparisonID;}

	// < > = を取得する
	std::string getOp(){return Op;}

	// 左辺値を取得する
	BaseAST *getLHS(){return LHS;}

	// 右辺値を取得する
	BaseAST *getRHS(){return RHS;}
};

/**
 * 関数外の変数を宣言するAST
 */
class GlobalVariableAST : public BaseAST{
	std::string Name;
	int Line;

	public:
		GlobalVariableAST(const std::string &name, int line) : BaseAST(GlobalVariableID), Name(name), Line(line){}
		~GlobalVariableAST(){}

		// ComparisonASTなのでtrueを返す
		static inline bool classof(GlobalVariableAST const* base){return true;}
		
		// 渡されたBaseASTがComparisonASTか判定する
		static inline bool classof(BaseAST const* base){return base->getValueID() == GlobalVariableID;}
		std::string getName(){return Name;}
		int getLine(){return Line;}
};

/**
 * 変数参照を表すAST
 */
class VariableAST : public BaseAST{
	//Name
	std::string Name;
	std::string Width;
	std::string Digit;

	public:
		VariableAST(const std::string &name) : 
			BaseAST(VariableID),Name(name){
			Width = "";
			Digit = "3";
		}
		~VariableAST(){}

		// VariableASTなのでtrueを返す
		static inline bool classof(VariableAST const*){return true;}

		// 渡されたBaseAstがVariableASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == VariableID;
		}
		
		// widthを設定
		bool setWidth(std::string i){Width = i; return true;}

		// digitを設定
		bool setDigit(std::string i){Digit = i;return true;}
		
		// Widthを取得
		std::string getWidth(){return Width;}
		
		// Digitを取得
		std::string getDigit(){return Digit;}

		// 変数名を取得
		std::string getName(){return Name;}
};

/*
 * 変数を表すAST
 */
class NumberAST : public BaseAST{
	double Val;
	std::string Width;
	std::string Digit;

	public:
		NumberAST(double val) : BaseAST(NumberID), Val(val){
			Width = "";
			Digit = "3";
		}
		~NumberAST(){}

		// DoubleASTなのでtrueを返す
		static inline bool classof(NumberAST const*){return true;}

		// 渡されたBaseASTがDoubleASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == NumberID;
		}

		// widthを設定
		bool setWidth(std::string i){Width = i; return true;}
		
		// digitを設定
		bool setDigit(std::string i){Digit = i;return true;}
		
		// Widthを取得
		std::string getWidth(){return Width;}
		
		// Digitを取得
		std::string getDigit(){return Digit;}

		// このASTが表現する数値を取得
		double getNumberValue(){ return Val; }
};

/*
 * 文字列を表すAST
 */
class StringAST : public BaseAST{
	std::string Str;

	public:
		StringAST(std::string str) : BaseAST(StringID), Str(str){}
		~StringAST(){}

		// StringASTなのでtrueを返す
		static inline bool classof(StringAST const*){return true;}

		// 渡されたBaseASTがStringASTか判定する
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == StringID;
		}

		// このASTが表現する文字列を取得
		std::string getStringValue(){return Str;}

		// 文字を足す
		bool setAddChar(char c){Str += c; return true;}
		
		// 文字を削除
		bool clear(){Str += ""; return true;}

		// 文字列を足す
		bool setAddString(std::string str){Str += str; return true;}
};

/**
 * breakを表すAST
 */
class BreakAST : public BaseAST{
	int Depth;

	public:
		BreakAST(int depth) : BaseAST(BreakID), Depth(depth){}
		static inline bool classof(BreakAST const*){return true;}
		static inline bool classof(BaseAST const* base){
		        return base->getValueID() == BreakID;
		}

		int getDepth(){return Depth;}
};

/**
 * Continueを表すAST
 */
class ContinueAST : public BaseAST{
	int Depth;
	
	public:
		
		ContinueAST(int depth) : BaseAST(ContinueID), Depth(depth){}
	
		static inline bool classof(ContinueAST const*){return true;}
	
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == ContinueID;
		}
		int getDepth(){return Depth;}
};

/**
 * ";"を表すAST
 */
class NullExprAST : public BaseAST {
	public:
		NullExprAST() : BaseAST(NullExprID){}
		static inline bool classof(NullExprAST const*){return true;}
		static inline bool classof(BaseAST const* base){
			return base->getValueID() == NullExprID;
		}
};

/**
 * "\n"を表すAST
 */
class NewLineAST : public BaseAST{

	public:
		NewLineAST() : BaseAST(NewLineID){}
		~NewLineAST(){}

		static inline bool classof(NewLineAST const*){return true;}
		static inline bool classof(BaseAST const* base)
		{return base->getValueID() == NewLineID;}
};

#endif
