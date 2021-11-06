#include "parser.hpp"
#include "lexer.hpp"

/**
 * コンストラクタ
 */
Parser::Parser(std::string filename){
	Tokens = LexicalAnalysis(filename);
};

/**
 * 構文解析実行
 * @return 解析成功:true 解析失敗:false
 */
bool Parser::doParse(){
	if(!Tokens){
		//fprintf(stderr, "error at lexer\n");
		return false;
	}else{
		return visitTranslationUnit();
	}
};

/**
 * AST取得
 * @return TranslationUnitへの参照
 */
TranslationUnitAST &Parser::getAST(){
	if(TU){
		return *TU;
	}else{ 
		return *(new TranslationUnitAST());
	}
};

/**
 * TranslationUnit用構文解析メソッド
 * @return 解析成功:true 解析失敗:false
 */
bool Parser::visitTranslationUnit(){
	TU = new TranslationUnitAST();

	// printの宣言追加
	std::vector<std::string> param_list;
	std::vector<std::string> param_identify;
	param_list.push_back("i");
	param_identify.push_back("string");
	TU->addPrototype(new PrototypeAST("int", "print", param_list, param_identify));
	PrototypeTable["print"] = 1;
	
	// clear
	param_list.clear();
	param_identify.clear();

	// inputの宣言追加
	param_list.push_back("i");
	param_identify.push_back("string");
	TU->addPrototype(new PrototypeAST("int", "input", param_list, param_identify));
	PrototypeTable["input"] = 1;
	
	//ExternalDecl
	while(true){
		if(!visitExternalDeclaration(TU)){
			SAFE_DELETE(TU);
			return false;
		}
		if (Tokens->getCurType() == TOK_EOF){
			break;
		}
	}
	return true;
}

/**
 * ExternalDeclaration用構文解析クラス
 * @param TranslationUnitAST
 * @return true
 */
bool Parser::visitExternalDeclaration(TranslationUnitAST *tunit){
	int bkup = Tokens->getCurIndex();
	// FunctionDeclaration
	PrototypeAST *proto = visitFunctionDeclaration();
	if (!proto){
		CORRECT = false;
		fprintf(stderr, "%d行目 : 関数を宣言する際は 関数名(引数,引数,,,){処理} としてください.\n", Tokens->getCurLine());
		Tokens->getNextFunction();
		return true;
	}

	tunit->addPrototype(proto);
	if(Tokens->getCurString() == "{" || Tokens->getCurString() == "{{"){
		Tokens->applyTokenIndex(bkup);
		// FunctionDefinition
		FunctionAST *func_def = visitFunctionDefinition();
		if (func_def){
			if(Tokens->getCurString() == "]]")Tokens->getNextToken();
			tunit->addFunction(func_def);
		}else
			Tokens->getNextFunction();

	}else{
		Tokens->getNextFunction();
		SAFE_DELETE(proto);
	}
	return true;
}

/**
 * FunctionDeclaration用構文解析メソッド
 * @return 解析成功:PrototypeAST 解析失敗:NULL
 */
PrototypeAST *Parser::visitFunctionDeclaration(){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	PrototypeAST *proto = visitPrototype();
	if (!proto)
		return NULL;
	
	// 再定義されていない確認
	if(PrototypeTable.find(proto->getName()) != PrototypeTable.end() ||
			(FunctionTable.find(proto->getName()) != FunctionTable.end() &&
			 FunctionTable[proto->getName()] != proto->getParamNum())){
		// エラーメッセージを出してNULLを返す
		CORRECT = false;
		fprintf(stderr, "%d行目 : 関数 %s はすでに作成されています\n",  line, proto->getName().c_str());
		SAFE_DELETE(proto);
		return NULL;
	}

	// prototype
	// （関数名, 引数）のペアをプロトタイプ宣言テーブル（Map）に追加
	PrototypeTable[proto->getName()] = proto->getParamNum();
	return proto;
}

/**
 * FunctionDefinition用構文解析メソッド
 * @return 解析成功:FunctionAST 解析失敗:NULL
 */
FunctionAST *Parser::visitFunctionDefinition(){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	PrototypeAST *proto = visitPrototype();
	
	if(!proto){
		return NULL;
	// ここでプロトタイプ宣言と間違いないか
	// すでに関数定義が行われていないか確認
	}else if(PrototypeTable.find(proto->getName()) != PrototypeTable.end() &&
			PrototypeTable[proto->getName()] != proto->getParamNum() ||
			FunctionTable.find(proto->getName()) != FunctionTable.end()){

		CORRECT = false;
		// エラーメッセージを出してNULLを返す
		fprintf(stderr, "%d行目 : 関数 %s はすでに作成されています\n",  line, proto->getName().c_str());
		SAFE_DELETE(proto);
		return NULL;
	}
	
	VariableTable.clear();
	FunctionStmtAST *func_stmt = visitFunctionStatement(proto);
	if(func_stmt){
		// ここで（関数名, 引数の数）のペアを関数テーブル（Map）に追加
		FunctionTable[proto->getName()] = proto->getParamNum();
		return new FunctionAST(proto, func_stmt);
	}else{
		SAFE_DELETE(proto);
		Tokens->getNextStatement();
		return NULL;
	}
}

/** Prototype用構文解析メソッド
 * @return 解析成功:PrototypeAST 解析失敗:NULL;
 */
PrototypeAST *Parser::visitPrototype(){
	// bkup index
	int bkup = Tokens->getCurIndex();

	// parameter_list
	bool is_first_param = true;
	std::string func_name;
	std::string func_identify;
	std::vector<std::string> param_list;
	std::vector<std::string> param_identify_list;

	// 関数名を取得
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		func_name = Tokens->getCurString();
		Tokens->getNextToken();
	}else{
		return NULL;
	}

	// type specifier
	if(func_name == "main")
		func_identify = "int";
	else
		func_identify = "double";
	
	// LEFT PAREN 
	if(Tokens->getCurType() != TOK_SYMBOL || Tokens->getCurString() != "("){
		if(Tokens->getCurString() == "=")
			fprintf(stderr, "%d行目 : もしかして繰り返し構文？.\n", Tokens->getCurLine());
		CORRECT = false;
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	
	for(int i=0;;i++){
		// ,
		if (!is_first_param && Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ","){
			Tokens->getNextToken();
		}
		else if(Tokens->getCurType() == TOK_IDENTIFIER){
			// 変数は全てdouble
			param_identify_list.push_back("double");
			// 引数の変数名に重複がないか確認
			if(std::find(param_list.begin(),param_list.end(), Tokens->getCurString()) != param_list.end()){
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}

			param_list.push_back(Tokens->getCurString());
			Tokens->getNextToken();
		}else{
			break;
		}

		is_first_param = false;
	}

	// RIGHT PAREN
	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")"){
		Tokens->getNextToken();
		return new PrototypeAST(func_identify, func_name, param_list, param_identify_list);
	}else{
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
}

/**
 * FunctionStatement用構文解析メソッド
 * @param 関数名や引数を格納したPrototypeクラスのインスタンス
 * @return 解析成功:FunctionStmtAST 解析失敗:NULL;
 */
FunctionStmtAST *Parser::visitFunctionStatement(PrototypeAST *proto){
	int bkup = Tokens->getCurIndex();
	if(Tokens->getCurString() == "{" || Tokens->getCurString() == "{{")
		Tokens->getNextToken();
	else
		return NULL;
	
	FunctionStmtAST *func_stmt = new FunctionStmtAST();
	
	// 引数をfunc_stmtの変数宣言リストに追加
	for(int i = 0; i < proto->getParamNum(); i++){
		VariableDeclAST *vdecl = new 
			VariableDeclAST(proto->getParamName(i), proto->getParamIdentify(i));
		vdecl->setDeclType(VariableDeclAST::param);
		func_stmt->addVariableDeclaration(vdecl);
		VariableTable.push_back(vdecl->getName());
	}
	
	VariableDeclAST *var_decl;
	BaseAST *stmt = NULL;
	BaseAST *last = NULL;
	// {,}を使うfor ifを格納
	std::vector<std::tuple<std::string, int>> lastStmt;
	std::string popStmt;
	int popLine;
	while(true){
		last = stmt;

		if(Tokens->getCurType() == TOK_EOF || Tokens->getCurString() == "]]"){
			break;
		}
		// 代入式か調べて代入先が未宣言の場合宣言する
		Tokens->getNextToken();
		if(Tokens->getCurString() == "="){
			Tokens->getBackToken();
			if(Tokens->getCurType() != TOK_IDENTIFIER){
				CORRECT = false;
				fprintf(stderr, "%d行目 :  = の左は変数が必要です\n", 
						Tokens->getCurLine());
				Tokens->applyTokenIndex(bkup);
				return NULL;
			}
			// 変数が宣言されていなかったら宣言する
			if(std::find(VariableTable.begin(), VariableTable.end(), 
						Tokens->getCurString()) == VariableTable.end()){
				VariableDeclAST *var_decl = visitVariableDeclaration();
				var_decl->setDeclType(VariableDeclAST::local);
				func_stmt->addVariableDeclaration(var_decl);
				VariableTable.push_back(var_decl->getName());
			}
		}else{
			Tokens->getBackToken();
		}


		int line = Tokens->getCurLine();
		
		// "}"が来るか確認
		if(Tokens->getCurString() == "}" && lastStmt.size() > 0){
			// "}"を処理していく
			while(Tokens->getCurString() == "}" && lastStmt.size() > 0){
				// if for どちらの}か取得
				popStmt = std::get<0>(lastStmt.at(lastStmt.size()-1));
				popLine = std::get<1>(lastStmt.at(lastStmt.size()-1));
				lastStmt.pop_back();
				// }の次のトークンに設定
				Tokens->getNextToken();
				
				line = Tokens->getCurLine();
				
				// elseが来た時に前にifが来ているか調べる
				if((Tokens->getCurType() == TOK_ELSE_IF || 
							Tokens->getCurType() == TOK_ELSE) && 
						(popStmt == "if" || popStmt == "else if")){
					std::string catch_token = Tokens->getCurString();
					int catch_line = Tokens->getCurLine();
					lastStmt.emplace_back(catch_token, catch_line);
					stmt = visitIfStatement(func_stmt);
					// 現在の else if, else をlastStmtへ
					// IfStatementへ
					if(Tokens->getCurType() == TOK_SYMBOL &&
						       	Tokens->getCurString() == "{"){
						Tokens->getNextToken();
					}else{
						CORRECT = false;
						fprintf(stderr, "%d行目 : 条件式またはelseの後に { がありません.\n", popLine);
						Tokens->getNextStatement();
						stmt = NULL;
					}
				}
				else if(popStmt == "if" || popStmt == "else if" || popStmt == "else"){
					stmt = visitIfEndStatement();
					if(Tokens->getCurType() == TOK_EOF){
						CORRECT = false;
						fprintf(stderr, "%d行目 : 条件式の処理の最後に } がありません.\n", popLine);
						Tokens->getNextStatement();
					}
					break;
				}
				else if(popStmt == "for"){
					stmt = visitForEndStatement();
					if(Tokens->getCurType() == TOK_EOF){
						CORRECT = false;
						fprintf(stderr, "%d行目 : 繰り返し構文の処理の最後に } がありません.\n", popLine);
						Tokens->getNextStatement();
					}
					break;
				}
				else
					stmt = NULL;
			}
		
		}
		else if(Tokens->getCurString() == "}" || Tokens->getCurString() == "}}"){
			break;
		}else if(Tokens->getCurType() == TOK_IF && Tokens->getCurString() == "if"){
			// {がくるか確認
			std::string catch_token = Tokens->getCurString();
			int catch_line = Tokens->getCurLine();
			lastStmt.emplace_back(catch_token, catch_line);
			stmt = visitIfStatement(func_stmt);
			if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "{"){
				Tokens->getNextToken();
			}else{
				CORRECT = false;
				fprintf(stderr, "%d行目 : if 条件式 の後に { がありません.\n", line);
				stmt = NULL;
				Tokens->getNextStatement();
			}
		
		}else if(Tokens->getCurType() == TOK_ELSE_IF){
			CORRECT = false;
			if(std::get<0>(lastStmt.at(lastStmt.size()-1)) == "if" || std::get<0>(lastStmt.at(lastStmt.size()-1)) == "else if")
				fprintf(stderr, "%d行目 : 上の条件式が } で閉じられていません.\n", line);
			else
				fprintf(stderr, "%d行目 : elif の前に ? がありません.\n", line);
			
			stmt = visitIfStatement(func_stmt);
			
			if(Tokens->getCurString() != "{")
				fprintf(stderr, "%d行目 : elif の処理を { の後に記述してください.\n", line);
			else
				Tokens->getNextToken();
		}
		else if(Tokens->getCurType() == TOK_ELSE){
			CORRECT = false;
			if(std::get<0>(lastStmt.at(lastStmt.size()-1)) == "if" || std::get<0>(lastStmt.at(lastStmt.size()-1)) == "else if")
				fprintf(stderr, "%d行目 : 上の条件式が } で閉じられていません.\n", line);
			else
				fprintf(stderr, "%d行目 : else の前に ? がありません.\n", line);
			
			stmt = visitIfStatement(func_stmt);
			
			if(Tokens->getCurString() != "{")
				fprintf(stderr, "%d行目 : else の処理を { の後に記述してください.\n", line);
			else
				Tokens->getNextToken();
		}
		else if(Tokens->getCurType() == TOK_FOR){
			std::string catch_token = Tokens->getCurString();
			int catch_line = Tokens->getCurLine();
			lastStmt.emplace_back(catch_token, catch_line);
			stmt = visitForStatement(func_stmt, lastStmt.size());
			// {がくるか確認
			if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "{"){
				Tokens->getNextToken();
			}else{
				CORRECT = false;
				fprintf(stderr, "%d行目 : for 繰り返し数 の後に { がありません.\n", line);
				Tokens->getNextStatement();
			}
		}else if(Tokens->getCurType() == TOK_BREAK){
			stmt = visitBreakStatement();

		}else if(Tokens->getCurType() == TOK_CONTINUE){
			stmt = visitContinueStatement();
			
		}else{
			stmt = visitStatement(func_stmt);
			if(!stmt){
				Tokens->getNextStatement();
				Tokens->getNextToken();
			}
		}
		if(stmt)
			func_stmt->addStatement(stmt);
	}
	
	if(Tokens->getCurString() == "}" || Tokens->getCurString() == "}}" || Tokens->getCurType() == TOK_EOF || Tokens->getCurString() == "]]"){
		while(lastStmt.size() != 0){
			popStmt = std::get<0>(lastStmt.at(lastStmt.size()-1));
                        popLine = std::get<1>(lastStmt.at(lastStmt.size()-1));
			lastStmt.pop_back();
			CORRECT = false;
			fprintf(stderr, "%d行目 : 処理の最後に } がありません.\n", popLine);
		}
		if(Tokens->getCurString() == "}}"){}
		else if (Tokens->getCurString() != "}"){
			CORRECT = false;
			fprintf(stderr, "関数 %s : 関数を閉じる } が足りません.\n", proto->getName().c_str());
		}
		
		if(!last || !llvm::isa<ReturnStmtAST>(last))
			func_stmt->addStatement(new ReturnStmtAST(new NumberAST(0.0)));
		Tokens->getNextToken();
		return func_stmt;
	}else{
		SAFE_DELETE(func_stmt);
		SAFE_DELETE(stmt);
		return NULL;
	}
}

/**
 * AssignmentExpression用構文解析メソッド
 * @return 解析成功:AST 解析失敗:NULL
 * 代入文の解析
 */
BaseAST *Parser::visitAssignmentExpression(FunctionStmtAST *func_stmt){
	int bkup = Tokens->getCurIndex();

	BaseAST *lhs;
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		// 変数が宣言されているか確認
		if(std::find(VariableTable.begin(), VariableTable.end(), Tokens->getCurString()) 
				!= VariableTable.end()){
			lhs = new VariableAST(Tokens->getCurString());
			Tokens->getNextToken();
			BaseAST *rhs;
			if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "="){
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt))
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				else{
					SAFE_DELETE(lhs);
					Tokens->getBackToken();
					CORRECT = false;
					fprintf(stderr, "%d行目 : = の右辺を確認してください.(上のエラーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "+="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt)){
					rhs = new BinaryExprAST("+", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : += の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "-="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL,func_stmt)){
					rhs = new BinaryExprAST("-", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : -= の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "*="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt)){
					rhs = new BinaryExprAST("*", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : *= の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "/="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt)){
					rhs = new BinaryExprAST("/", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : /= の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "%="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt)){
					rhs = new BinaryExprAST("%", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : %%= の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "//="){
				Tokens->getBackToken();
				BaseAST *mhs = new VariableAST(Tokens->getCurString());
				Tokens->getNextToken();
				Tokens->getNextToken();
				if(rhs = visitAdditiveExpression(NULL, func_stmt)){
					rhs = new BinaryExprAST("//", mhs, rhs, Tokens->getToken().getLine());
					return new BinaryExprAST("=", lhs, rhs, Tokens->getToken().getLine());
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : //= の右辺を確認してください.(上のエラ>ーを修正することで直る場合があります)\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
			else{
				// 変数と関数が同じ場合
				if(Tokens->getCurString() == "("){
					Tokens->getBackToken();
				}else{
					SAFE_DELETE(lhs);
					CORRECT = false;
					fprintf(stderr, "%d行目 : 式は変数に代入されていません.\n", Tokens->getCurLine());
					Tokens->getNextStatement();
					return NULL;
				}
			}
		}else{
			Tokens->applyTokenIndex(bkup);
		}
	}

	BaseAST *add_expr = visitAdditiveExpression(NULL, func_stmt);
	if(add_expr){
		return add_expr;
	}
	return NULL;
}

/**
 * PrimaryExpression用構文解析メソッド
 * @return 解析成功時: AST失敗時:NULL
 */
BaseAST *Parser::visitPrimaryExpression(FunctionStmtAST *func_stmt){
	// recored index
	int bkup = Tokens->getCurIndex();
	// 変数が宣言されていることを確認
	// VARIABLE_IDENTIFIER
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		Tokens->getNextToken();
		if(Tokens->getCurString() == "("){
			Tokens->getBackToken();
			return NULL;
		}
		Tokens->getBackToken();
		if(std::find(VariableTable.begin(), VariableTable.end(),
				   Tokens->getCurString()) != VariableTable.end()){
			std::string var_name = Tokens->getCurString();
			Tokens->getNextToken();
			return new VariableAST(var_name);
		}
		return NULL;
	
	//数字
	}else if(Tokens->getCurType() == TOK_DIGIT){
		double val = Tokens->getCurNumVal();
		Tokens->getNextToken();
		return new NumberAST(val);
	
	// string
	}else if(Tokens->getCurType() == TOK_STR){
		std::string str = Tokens->getCurStrVal();
		Tokens->getNextToken();
		return new StringAST(str);

	// integer(-)
	}else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "-"){
		BaseAST *lhs = new NumberAST(-1);
		Tokens->getNextToken();
		BaseAST *rhs = visitPostfixExpression(func_stmt);
		return new BinaryExprAST("*", lhs, rhs, Tokens->getCurLine());
	// (
	}else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "("){
		int line = Tokens->getCurLine();
		Tokens->getNextToken();
		BaseAST *lhs = visitAdditiveExpression(NULL, func_stmt);
		if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")")
			Tokens->getNextToken();
		else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : ( を使った式が ) で閉じられていません.\n", line);
			Tokens->getNextStatement();
			return NULL;
		}
		return lhs;
	}

	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() != ";" && Tokens->getCurString() != "{" && Tokens->getCurString() != "}"){
		CORRECT = false;
		fprintf(stderr, "%d行目 : %s が予期せぬところに書かれています.\n", Tokens->getCurLine(), Tokens->getCurString().c_str());
		Tokens->getNextStatement();
	}
	
	return NULL;
}

/**
 * PostfixExpression用構文解析メソッド
 * @return 解析成功:AST 解析失敗:NULL
 */
BaseAST *Parser::visitPostfixExpression(FunctionStmtAST *func_stmt){
	// get index
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	BaseAST *prim_expr = visitPrimaryExpression(func_stmt);
	if(prim_expr)
		return prim_expr;
	
	
	// FUNCTION_IDENTIFIER
	if(Tokens->getCurType() == TOK_IDENTIFIER){
		int param_num;
		// プロトタイプ宣言されているか確認し、引数の数をテーブルから取得
		if(PrototypeTable.find(Tokens->getCurString()) != PrototypeTable.end() ){
			param_num = PrototypeTable[Tokens->getCurString()];

		//関数定義済みであるか確認し、引数の数をテーブルから取得
		}else if(FunctionTable.find(Tokens->getCurString()) != FunctionTable.end()){
			param_num = FunctionTable[Tokens->getCurString()];
		}
		else{
			int bfr = Tokens->getCurIndex();
			if(Tokens->getNextToken() && Tokens->getCurString() == "(" && Tokens->getNextToken() && Tokens->getCurString() == ")"){
				Tokens->applyTokenIndex(bfr);
				CORRECT = false;
				fprintf(stderr, "%d行目 : 関数 %s は宣言されていません.\n", line, Tokens->getCurString().c_str());
			}else{
				Tokens->applyTokenIndex(bfr);
				CORRECT = false;
				fprintf(stderr, "%d行目 : 変数 %s は宣言されていません.\n", line, Tokens->getCurString().c_str());
			}
			Tokens->getNextStatement();
			return NULL;
		}

		// 関数名取得
		std::string Callee = Tokens->getCurString();
		Tokens->getNextToken();
		
		// LEFT PAREN
		if(Tokens->getCurType() != TOK_SYMBOL || Tokens->getCurString() != "("){
			Tokens->getNextStatement();
			return NULL;
		}

		Tokens->getNextToken();
		std::vector<BaseAST*> args;
		BaseAST *assign_expr = NULL;
		// 引数を解析 print と その他
		if(Callee == "print"){
			while(Tokens->getCurString() != ")"){
				VariableAST *arg1 = NULL;
				NumberAST *arg2 = NULL;
				BinaryExprAST *arg3 = NULL;
				CallExprAST *arg4 = NULL;
				if(Tokens->getCurString() == ","){
					assign_expr = new NewLineAST();
					Tokens->getNextToken();
				}
				else if(!(assign_expr = visitAdditiveExpression(NULL, func_stmt))){
					CORRECT = false;
					fprintf(stderr, "%d行目 : printの引数を確認してください\n", line);
					Tokens->getNextStatement();
					break;
				}
				
				if(llvm::isa<VariableAST>(assign_expr))
					arg1 = llvm::dyn_cast<VariableAST>(assign_expr);
				else if(llvm::isa<NumberAST>(assign_expr))
					arg2 = llvm::dyn_cast<NumberAST>(assign_expr);
				else if(llvm::isa<BinaryExprAST>(assign_expr))
					arg3 = llvm::dyn_cast<BinaryExprAST>(assign_expr);
				else if(llvm::isa<CallExprAST>(assign_expr))
					arg4 = llvm::dyn_cast<CallExprAST>(assign_expr);

				if(arg1 || arg2 || arg3 || arg4){
					if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "\\"){
						Tokens->getNextToken();
						if(Tokens->getCurType() == TOK_DIGIT){
							if(arg1)
								arg1->setWidth(Tokens->getCurString());
							else if(arg2)
								arg2->setWidth(Tokens->getCurString());
							else if(arg3)
								arg3->setWidth(Tokens->getCurString());
							else if(arg4)
								arg4->setWidth(Tokens->getCurString());
							Tokens->getNextToken();
						}
						if(!(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "\\")){
							CORRECT = false;
							fprintf(stderr, "%d行目 : printの引数を確認してください\n", line);
							return NULL;
						}
						Tokens->getNextToken();
						if(Tokens->getCurString() == ","){
							if(arg1)
								arg1->setDigit("-1");
							else if(arg2)
								arg2->setDigit("-1");
							else if(arg3)
								arg3->setDigit("-1");
							else if(arg4)
								arg4->setDigit("-1");
						}
						else{
							std::string s = "";
							if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "-"){
								s += Tokens->getCurString();
								Tokens->getNextToken();
							}
							if(Tokens->getCurType() == TOK_DIGIT){
								if(arg1)
									arg1->setDigit(s+Tokens->getCurString());
								else if(arg2)
									arg2->setDigit(s+Tokens->getCurString());
								else if(arg3)
									arg3->setDigit(s+Tokens->getCurString());
								else if(arg4)
									arg4->setDigit(s+Tokens->getCurString());
								Tokens->getNextToken();
							}
						}
					}

					if(arg1)
						assign_expr = arg1;
					else if(arg2)
						assign_expr = arg2;
					else if(arg3)
						assign_expr = arg3;
					else if(arg4)
						assign_expr = arg4;
				}
				args.push_back(assign_expr);
				if (Tokens->getCurString() == ";" || Tokens->getCurString() == "}" || Tokens->getCurString() == "{"){
					CORRECT = false;
					fprintf(stderr, "%d行目 : printの末尾にカッコがありません\n", line);
					Tokens->getNextStatement();
					return NULL;
				}
				else if(Tokens->getCurString() == ","){
					Tokens->getNextToken();
					if(Tokens->getCurString() == ")")
						args.push_back(new NewLineAST());
				}
			}
			if(args.size() == 0)
				args.push_back(new NewLineAST());
		}
		else if(Callee == "input"){
			bool is_first = true;
			while(is_first || Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ","){
				if(!is_first) Tokens->getNextToken();
				
				// 変数が宣言されていなかったら宣言する
				if(Tokens->getCurType() == TOK_IDENTIFIER){
					if(std::find(VariableTable.begin(), VariableTable.end(), Tokens->getCurString()) == VariableTable.end()){
						VariableDeclAST *var_decl = visitVariableDeclaration();
						var_decl->setDeclType(VariableDeclAST::local);
						func_stmt->addVariableDeclaration(var_decl);
						VariableTable.push_back(var_decl->getName());
					}
					args.push_back(new VariableAST(Tokens->getCurString()));
					Tokens->getNextToken();
				}
				else{
					CORRECT = false;
					fprintf(stderr, "%d行目 : input の引数の %s に入力は代入できません.\n", line, Tokens->getCurString().c_str());
					Tokens->getNextStatement();
					return NULL;
				}
				
				is_first = false;
			}
		}
		else{
			if(Tokens->getCurString() != ")" && (assign_expr = visitAdditiveExpression(NULL, func_stmt))){
				args.push_back(assign_expr);
				// ","が続く限り繰り返し
				while(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ","){
					Tokens->getNextToken();
					// IDENTIFIER
					if(Tokens->getCurString() != ")" && (assign_expr = visitAdditiveExpression(NULL, func_stmt)))
						args.push_back(assign_expr);
					else
						break;
				}
			}
		}
		// 引数の数を確認(変数の数に指定のないprint関数は除く)
		if(Callee != "print" && Callee != "input" && args.size() != param_num){
			for(int i=0;i<args.size();i++){
				SAFE_DELETE(args[i]);
			}
			CORRECT = false;
			fprintf(stderr, "%d行目 : 関数 %s の引数の数が合いません.\n", line, Callee.c_str());
			Tokens->getNextStatement();
			return NULL;
		}
		
		// Right PaLen
		if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")"){
			Tokens->getNextToken();
			return new CallExprAST(Callee,args);
		}else{
			for(int i=0;i<args.size();i++){
				SAFE_DELETE(args[i]);
			}
			CORRECT = false;
			fprintf(stderr, "%d行目 : 関数 %s の呼び出しに失敗しました.\n", line, Callee.c_str());
			Tokens->getNextStatement();
			return NULL;
		}
	}else{
		return NULL;
	}
}

/**
 * AdditiveExpression用構文解析メソッド
 * @param lhs（左辺）。初回呼び出しはNULL
 * @return 解析成功:AST 解析失敗:NULL
 */
BaseAST *Parser::visitAdditiveExpression(BaseAST *lhs, FunctionStmtAST *func_stmt){
	// bkup index
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	if(!lhs)
		lhs = visitMultiplicativeExpression(NULL, func_stmt);
	if(!lhs)
		return NULL;
	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")")
		return lhs;

	BaseAST *rhs;
	// +
	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "+"){
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL, func_stmt);
		if(rhs){
			return visitAdditiveExpression(
					new BinaryExprAST("+", lhs, rhs, line), func_stmt);
		}else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : 式を確認してください.\n", Tokens->getCurLine());
			SAFE_DELETE(lhs);
			Tokens->getNextStatement();
			return NULL;
		}
	
	// -
	}else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "-"){
		Tokens->getNextToken();
		rhs = visitMultiplicativeExpression(NULL, func_stmt);
		if(rhs){
			return visitAdditiveExpression(new BinaryExprAST("-", lhs, rhs, line),func_stmt);
		}else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : 式を確認してください.\n", Tokens->getCurLine());
			SAFE_DELETE(lhs);
			Tokens->getNextStatement();
			return NULL;
		}
	}
	return lhs;
}

/**
 * ExpressionStatement用構文解析メソッド
 * @return 解析成功:AST 解析失敗:NULL
 */
BaseAST *Parser::visitExpressionStatement(FunctionStmtAST *func_stmt){
	BaseAST *assign_expr;

	// NULL Expression
	if(Tokens->getCurString() == ";"){
		Tokens->getNextToken();
		return new NullExprAST();
	}else if(assign_expr = visitAssignmentExpression(func_stmt)){
		if(Tokens->getCurString() == ";"){
			Tokens->getNextToken();
			return assign_expr;
		}
		else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : %s が予期せぬところにあります.\n", Tokens->getCurLine(), Tokens->getCurString().c_str());
			Tokens->getNextStatement();
		}
	}
	return NULL;
}

/**
 * Statement用構文解析メソッド
 * @return 解析成功:AST 解析失敗:NULL
 */
BaseAST *Parser::visitStatement(FunctionStmtAST *func_stmt){
	BaseAST *stmt = NULL;
	if(stmt = visitReturnStatement(func_stmt)){
		return stmt;
	}else if(stmt = visitGlobalStatement(func_stmt)){
		return stmt;
	}else if(stmt = visitExpressionStatement(func_stmt)){
		return stmt;
	}else{
		return NULL;
	}
}

/**
 * VariableDeclaration用解析メソッド
 * @return 解析成功:VariableeclAST 解析失敗:NULL
 */
VariableDeclAST *Parser::visitVariableDeclaration(){
	std::string name;
	std::string identify;

	identify = "double";

	if(Tokens->getCurType() == TOK_IDENTIFIER){
		name = Tokens->getCurString();
	}else{
		Tokens->ungetToken(1);
		return NULL;
	}
	
	return new VariableDeclAST(name, identify);
}

/**
 * GlobalStatement用解析メソッド
 * @return 解析成功:GlobalVariableAST 解析失敗:NULL
 */
BaseAST *Parser::visitGlobalStatement(FunctionStmtAST *func_stmt){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	std::string Name;
	
	if(Tokens->getCurType() == TOK_GLOBAL)
		Tokens->getNextToken();
	else
		return NULL;
	
	if(Tokens->getCurType() == TOK_IDENTIFIER)
		Name = Tokens->getCurString();
	else{
		CORRECT = false;
		fprintf(stderr, "global の後は変数名である必要があります.\n");
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	if(std::find(VariableTable.begin(), VariableTable.end(), Name) == VariableTable.end())
		VariableTable.push_back(Name);
	else{
		CORRECT = false;
		fprintf(stderr, "global の後の変数が関数内の変数と被っています.\n");
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}
	Tokens->getNextToken();
	if(Tokens->getCurString() == ";")
		Tokens->getNextToken();
	else{
		CORRECT = false;
		fprintf(stderr, "global 変数名 の後は ; である必要があります.\n");
		Tokens->applyTokenIndex(bkup);
		return NULL;
	}

	func_stmt->addGlobalVariables(Name);
	return new GlobalVariableAST(Name, line);
}

/**
 * MultiplicativeExpression用解析メソッド
 * @return 解析成功:AST 解析失敗:NULL
 */
BaseAST *Parser::visitMultiplicativeExpression(BaseAST *lhs, FunctionStmtAST *func_stmt){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	if(!lhs){
		lhs = visitPostfixExpression(func_stmt);
	}
	BaseAST *rhs;
	if(!lhs){
		return NULL;
	}

	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "*"){
		Tokens->getNextToken();
		rhs = visitPostfixExpression(func_stmt);
		if(rhs){
			return visitMultiplicativeExpression(new BinaryExprAST("*", lhs, rhs, line), func_stmt);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "/"){
		Tokens->getNextToken();
		rhs = visitPostfixExpression(func_stmt);
		if(rhs){
			return visitMultiplicativeExpression(new BinaryExprAST("/", lhs, rhs, line), func_stmt);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
	else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "//"){
		Tokens->getNextToken();
		rhs = visitPostfixExpression(func_stmt);
		if(rhs){
			return visitMultiplicativeExpression(new BinaryExprAST("//", lhs, rhs, line), func_stmt);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
	else if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "%"){
		Tokens->getNextToken();
		rhs = visitPostfixExpression(func_stmt);
		if(rhs){
			return visitMultiplicativeExpression(new BinaryExprAST("%", lhs, rhs, line), func_stmt);
		}else{
			SAFE_DELETE(lhs);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}
	return lhs;
}

/**
 * ReturnStatement用解析メソッド
 * @return 解析成功:ReturnStmtAST 解析失敗:NULL
 */
BaseAST *Parser::visitReturnStatement(FunctionStmtAST *func_stmt){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	BaseAST *expr;

	if(Tokens->getCurType() == TOK_RETURN){
		Tokens->getNextToken();
		expr = visitAdditiveExpression(NULL, func_stmt);
		if(!expr || llvm::isa<NullExprAST>(expr)){
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}

		if(Tokens->getCurString() == ";"){
			Tokens->getNextToken();
			return new ReturnStmtAST(expr);
		}else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : ; がありません.\n", line);
			Tokens->applyTokenIndex(bkup);
			return NULL;
		}
	}else{
		return NULL;
	}
}

/**
 * IfStatement用解析メソッド
 * if(式 and or 式){}の形
 * @return 解析成功：IfStmtAST 解析失敗：NULL
 */
BaseAST *Parser::visitIfStatement(FunctionStmtAST *func_stmt){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	std::string name = Tokens->getCurString();
	IfStatementAST *if_expr = new IfStatementAST();
	if_expr->setIf(name);
	Tokens->getNextToken();
	
	int depth = 0;

	if(name == "else")
		return if_expr;
		
	// 条件式を見ていく
	bool is_first = true;
	BaseAST *lhs;
	std::string op;
	BaseAST *rhs;
	ComparisonAST *com;
	while(Tokens->getCurString() != "{"){
		if(!is_first){
			if(Tokens->getCurType() == TOK_AND || Tokens->getCurType() == TOK_OR){
				if_expr->addOp(Tokens->getCurString());
				Tokens->getNextToken();
				if_expr->addDepth(depth);
			}
			else{
				CORRECT = false;
				fprintf(stderr, "%d行目 : 条件式の結合が %s でされています.\n", line, Tokens->getCurString().c_str());
				Tokens->getNextStatement();
				return NULL;
			}
		}
			
		while(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "["){
			depth += 1;
			Tokens->getNextToken();
		}

		lhs = visitAdditiveExpression(NULL, func_stmt);
		if(!lhs || llvm::isa<NullExprAST>(lhs)){
			CORRECT = false;
			fprintf(stderr, "%d行目 : 条件式を確認してください.\n", line);
			Tokens->getNextToken();
			return NULL;
		}

		// 比較方法を取得
		if(Tokens->getCurType() == TOK_SYMBOL && 
				(Tokens->getCurString() == "==" ||
				 Tokens->getCurString() == "!=" ||
				 Tokens->getCurString() == "<"  ||
				 Tokens->getCurString() == ">"  ||
				 Tokens->getCurString() == ">=" ||
				 Tokens->getCurString() == "<=")){
			op = Tokens->getCurString();
			Tokens->getNextToken();
		}else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : 条件式内で %s を条件としています.\n", line, Tokens->getCurString().c_str());
			Tokens->getNextStatement();
			return NULL;
		}

		// 条件式の右辺を取得
		rhs = visitAdditiveExpression(NULL, func_stmt);
		if(!rhs || llvm::isa<NullExprAST>(rhs)){
			CORRECT = false;
			fprintf(stderr, "%d行目 : 条件式内を確認してください.\n", line);
			Tokens->getNextStatement();
			return NULL;
		}
				
		// 条件式追加
		com = new ComparisonAST(op, lhs, rhs);
		if(!com){
			Tokens->getNextStatement();
			return NULL;
		}
		
		// )を確認する
		//Tokens->getBackToken();
		while(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "]"){
			depth -= 1;
			Tokens->getNextToken();
		}
			
		if_expr->addComparison(com);
		is_first = false;
	}
	
	if(depth != 0){
		CORRECT = false;
		fprintf(stderr, "%d行目 : 真偽値の () を確認してください.\n", line);
		SAFE_DELETE(if_expr);
		return NULL;
	}

	return if_expr;
}

/**
 ** IfEndStatement用解析メソッド
 ** @return 解析成功：IfEndAST 解析失敗：NULL
 */
BaseAST *Parser::visitIfEndStatement(){
	return new IfEndAST();
}

/**
 * ForStatement用解析メソッド
 * @return 解析成功:ForStatementAST 解析失敗:NULL
 */
ForStatementAST *Parser::visitForStatement(FunctionStmtAST *func_stmt, int id){
	int bkup = Tokens->getCurIndex();
		
	// エラー検出のためforを表すTokenのつぎへ
	while(Tokens->getCurType() == TOK_FOR)
		Tokens->getNextToken();
	// 変数名取得
	std::string val_name;
	BaseAST *start_expr = new NumberAST(1);
	BaseAST *end_expr = new NumberAST(0);
	VariableAST *val;
	BinaryExprAST *bin_expr;
	
	// 繰り返し回数のカウント
	std::string count_id = "count__________count" + std::to_string(id);
	if (std::find(VariableTable.begin(), VariableTable.end(), count_id) == VariableTable.end()){
		VariableDeclAST *var_decl = new VariableDeclAST(count_id, "double");
		var_decl->setDeclType(VariableDeclAST::local);
		func_stmt->addVariableDeclaration(var_decl);
		VariableTable.push_back(var_decl->getName());
	}
	val = new VariableAST(count_id);

	// 繰り返しの終わりの値を取得
	end_expr = visitAdditiveExpression(NULL, func_stmt);
	if(!end_expr || llvm::isa<NullExprAST>(end_expr)){
		if(!end_expr){
			CORRECT = false;
			fprintf(stderr, "%d行目 : for 繰り返し回数 でなければいけません\n", Tokens->getCurLine());
		}
		bin_expr = new BinaryExprAST("=", val, start_expr, Tokens->getCurLine());
		return new ForStatementAST(val, bin_expr, new NumberAST(0));
	}
	bin_expr = new BinaryExprAST("=", val, start_expr, Tokens->getCurLine());
	return new ForStatementAST(val, bin_expr, end_expr);
}

/**
 * ForEndStatement用解析メソッド
 * @return ForEndAST
 */
BaseAST *Parser::visitForEndStatement(){
	return new ForEndAST();
}

/**
 * BreakStatement用解析メソッド
 * @return BreakAST or NULL
 */
BaseAST *Parser::visitBreakStatement(){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();
	int to = 1;
	Tokens->getNextToken();
	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "("){
		Tokens->getNextToken();
		if(Tokens->getCurType() == TOK_DIGIT){
			to = std::stoi(Tokens->getCurString());
			if(to <= 0){
				CORRECT = false;
				fprintf(stderr, "%d行目 : break( の後の数字は1以上です\n", line);
				Tokens->getNextStatement();
				return NULL;
			}
			Tokens->getNextToken();
		}
		else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : break( の次は数字でなければいけません", Tokens->getCurLine());
			Tokens->getNextStatement();
			return NULL;
		}
		if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")")
			Tokens->getNextToken();
		else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : break(数字　の次は ) でなければいけません", Tokens->getCurLine());
			Tokens->getNextStatement();
			return NULL;
		}
	}

	if(Tokens->getCurString() == ";")
		Tokens->getNextToken();
	else{
		CORRECT = false;
		fprintf(stderr, "%d行目 : break() と異なります.\n", line);
		Tokens->getNextStatement();
		return NULL;
	}
	return new BreakAST(to);
}

/**
 * ContinueStatement用解析メソッド
 * @return ContinueAST or NULL
 */
BaseAST *Parser::visitContinueStatement(){
	int bkup = Tokens->getCurIndex();
	int line = Tokens->getCurLine();

	int to = 1;
	Tokens->getNextToken();
	if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == "("){
		Tokens->getNextToken();
		if(Tokens->getCurType() == TOK_DIGIT){
			to = std::stoi(Tokens->getCurString());
			if(to <= 0){
				CORRECT = false;
				fprintf(stderr, "%d行目 : continue( の後の数字は1以上です\n", line);
				Tokens->getNextStatement();
				return NULL;
			}
			Tokens->getNextToken();
		}
		else{
			CORRECT = false;
			fprintf(stderr, "%d行目 : continue( の次は数字でなければいけません", line);
			Tokens->getNextStatement();
			return NULL;
		}
		if(Tokens->getCurType() == TOK_SYMBOL && Tokens->getCurString() == ")")
			Tokens->getNextToken();
		else{
			fprintf(stderr, "%d行目 : continue(数字　の次は ) でなければいけません", line);
			Tokens->getNextStatement();
			return NULL;
		}
	}

	if(Tokens->getCurString() == ";")
		Tokens->getNextToken();
	else{
		CORRECT = false;
		fprintf(stderr, "%d行目 : continue() の次は ; が来る必要があります.\n", line);
		Tokens->getNextStatement();
		return NULL;
	}
	return new ContinueAST(to);
}
