#include "codegen.hpp"

/**
 * コンストラクタ
 */
CodeGen::CodeGen(){
	Builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
	Mod = NULL;
}

/**
 * デストラクタ
 */
CodeGen::~CodeGen(){
	SAFE_DELETE(Builder);
	SAFE_DELETE(Mod);
}

/**
 * コード生成実行
 * @param TranslationUnitAST Module名（入力ファイル名）
 * @return 成功時:true 失敗時:false
 */
bool CodeGen::doCodeGen(TranslationUnitAST &tunit, std::string name, std::string link_file, bool with_jit = false){
	// Module生成に失敗したら終了
	if(!generateTranslationUnit(tunit, name))
		return false;
	
	// LinkFileの指定があったらModuleをリンク
	if(!link_file.empty() && !linkModule(Mod, link_file))
		return false;

	// JITのフラグふぁ立っていたらJIT
	if(with_jit){
		// ExecutionEngine生成
		llvm::ExecutionEngine *EE = llvm::EngineBuilder(Mod).create();
		
		// 実行したいFunctionのポインタを渡す（main関数へのポインタを取得）
		llvm::Function *F;
		if(!(F = Mod->getFunction("main")))
			return false;

		// JIT済みのmain関数のポインタを取得
		int (*fp)() = (int (*)())EE->getPointerToFunction(F);
		fprintf(stderr,"%d\n",fp());
	}
	return true;
}

/**
 * Module取得
 */
llvm::Module &CodeGen::getModule(){
	if(Mod)
		return *Mod;
	else
		return *(new llvm::Module("null", llvm::getGlobalContext()));
}

/**
 *  Module生成メソッド
 *  @param TranslationUnitAST Module名（入力ファイル）
 *  @return 成功時:true 失敗時:false
 */
bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit, std::string name){
	// Moduleを生成
	Mod = new llvm::Module(name, llvm::getGlobalContext());
	
	// printのFunction/////////////////////////////////////////////////
	std::vector<llvm::Type*> printFuncArgs;
	printFuncArgs.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

	llvm::FunctionType *printFuncType = llvm::FunctionType::get(
			llvm::Type::getDoubleTy(llvm::getGlobalContext()),
			printFuncArgs,
			true
	);

	llvm::Function *printFunc = llvm::Function::Create(
			printFuncType,
			llvm::GlobalValue::ExternalLinkage,
			"printf",
			Mod
	);
	printFunc->setCallingConv(llvm::CallingConv::C);
	////////////////////////////////////////////////////////////////////
	
	// scanfのFunction//////////////////////////////////////////////////
	std::vector<llvm::Type*> scanFuncArgs;
	scanFuncArgs.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

	llvm::FunctionType *scanFuncType = llvm::FunctionType::get(
			llvm::Type::getDoubleTy(llvm::getGlobalContext()),
			scanFuncArgs,
			true
	);

	llvm::Function *scanFunc = llvm::Function::Create(
			scanFuncType,
			llvm::GlobalValue::ExternalLinkage,
			"__isoc99_scanf",
			Mod
	);
	scanFunc->setCallingConv(llvm::CallingConv::C);
	////////////////////////////////////////////////////////////////////
	
	// sprintfのfunction ///////////////////////////////////////////////
	std::vector<llvm::Type*> sprintFuncArgs;
	sprintFuncArgs.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
	sprintFuncArgs.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));

	llvm::FunctionType *sprintFuncType = llvm::FunctionType::get(
			llvm::Type::getDoubleTy(llvm::getGlobalContext()),
			sprintFuncArgs,
			true
	);

	llvm::Function *sprintFunc = llvm::Function::Create(
			sprintFuncType,
			llvm::GlobalValue::ExternalLinkage,
			"sprintf",
			Mod
	);
	sprintFunc->setCallingConv(llvm::CallingConv::C);
	////////////////////////////////////////////////////////////////////

	// memsetのfunction ////////////////////////////////////////////////
	std::vector<llvm::Type*> memsetFuncArgs;
	memsetFuncArgs.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
	memsetFuncArgs.push_back(llvm::Type::getInt8Ty(llvm::getGlobalContext()));
	memsetFuncArgs.push_back(llvm::Type::getInt64Ty(llvm::getGlobalContext()));
	memsetFuncArgs.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
	memsetFuncArgs.push_back(llvm::Type::getInt1Ty(llvm::getGlobalContext()));
	llvm::FunctionType *memsetFuncType = llvm::FunctionType::get(
			llvm::Type::getDoubleTy(llvm::getGlobalContext()),
	                memsetFuncArgs,
			false
	);

	llvm::Function *memsetFunc = llvm::Function::Create(
			memsetFuncType,
			llvm::GlobalValue::ExternalLinkage,
			"llvm.memset.p0i8.i64",
			Mod
	);
	memsetFunc->setCallingConv(llvm::CallingConv::C);
	////////////////////////////////////////////////////////////////////


	// function declaration
	for(int i = 0; ; i++){
		PrototypeAST *proto = tunit.getPrototype(i);
		if(!proto){
			break;
		}else if(!generatePrototype(proto, Mod)){
			SAFE_DELETE(Mod);
			return false;
		}
	}

	// function definition
	// main最初
	for(int i = 0; ;i++){
		FunctionAST *func = tunit.getFunction(i);
		if(!func)
			break;
		if(func->getPrototype()->getName() == "main"){
			if(!generateFunctionDefinition(func, Mod)){
				SAFE_DELETE(Mod);
				return false;
			}
		}
	}
	for(int i = 0; ;i++){
		FunctionAST *func = tunit.getFunction(i);
		
		if(!func)
			break;
		if(func->getPrototype()->getName() != "main"){
			if(!generateFunctionDefinition(func, Mod)){
				SAFE_DELETE(Mod);
				return false;
			}
		}
	}
	return true;
}

/**
 * 関数宣言生成メソッド
 * @param PrototypeAST, Module
 * @return 生成したFunctionのポインタ
 */
llvm::Function *CodeGen::generatePrototype(PrototypeAST *proto, llvm::Module *mod){
	// already declared?
	llvm::Function *func = mod->getFunction(proto->getName());
	if(func){
		if(func->arg_size() == proto->getParamNum() && func->empty()){
			return func;
		}else{
			fprintf(stderr, "error::function %s is redefined", proto->getName().c_str());
			return NULL;
		}
	}
	// create arg_types
	std::vector<llvm::Type*> arg_types;
	for(int i=0; i < proto->getParamNum(); i++){
		if(proto->getParamIdentify(i) == "int")
			arg_types.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
		else if(proto->getParamIdentify(i) == "double")
			arg_types.push_back(llvm::Type::getDoubleTy(llvm::getGlobalContext()));
		else if(proto->getParamIdentify(i) == "string")
			arg_types.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()));
		else
			return NULL;
	}	
	// create func type
	llvm::FunctionType *func_type;
	if(proto->getIdentify() == "double")
		func_type = llvm::FunctionType::get(
				llvm::Type::getDoubleTy(llvm::getGlobalContext()),arg_types,false);
	else if(proto->getIdentify() == "int")
		func_type = llvm::FunctionType::get(
				llvm::Type::getInt32Ty(llvm::getGlobalContext()),arg_types,false);
	// create function
	func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, proto->getName(), mod);
	// set names
	llvm::Function::arg_iterator arg_iter = func->arg_begin();
	for(int i = 0; i < proto->getParamNum(); i++){
		arg_iter->setName(proto->getParamName(i).append("_arg"));
		arg_iter++;
	}
	return func;
}

/**
 * 関数定義生成メソッド
 * @param FunctionAST Module
 * @return 生成したFunctionのポインタ
 */
llvm::Function *CodeGen::generateFunctionDefinition(FunctionAST *func_ast, llvm::Module *mod){
	llvm::Function *func = generatePrototype(func_ast->getPrototype(), mod);
	if(!func){ return NULL; }
	CurFunc = func;
	//FuncName = func_ast->getPrototype()->getName();
	llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", func);
	Builder->SetInsertPoint(bblock);
	// Functionのボディを生成
	llvm::Value *f = generateFunctionStatement(func_ast->getBody(), func);
	if(!f)
		return NULL;
	return func;
}

/**
 * 関数生成メソッド
 * 変数宣言、ステートメントの順に生成
 * @param FunctionStmtAST
 * @return 最後に生成した
 */
llvm::Value *CodeGen::generateFunctionStatement(FunctionStmtAST *func_stmt, llvm::Function *func){
	// inser variable decls
	VariableDeclAST *vdecl;
	llvm::Value *v = NULL;
	for(int i = 0; ;i++){
		// 最後まで見たら終了
		if(!func_stmt->getVariableDecl(i))
			break;

		//create alloca
		vdecl = llvm::dyn_cast<VariableDeclAST>(func_stmt->getVariableDecl(i));
		v = generateVariableDeclaration(vdecl);
	}
	// ifに対応する変数など
	BaseAST *stmt;
	llvm::BasicBlock *bblock;// Blockの生成と判定式が正の時格納
	llvm::BasicBlock *bend;  // ifの最後
	llvm::BasicBlock *bcur;  // ポイント設定
	llvm::BasicBlock *bbfr;  // 1つ前のBlock
	llvm::BasicBlock *bago;  // 2つ前のBlock
	std::vector<llvm::BasicBlock*> bor;  // True or の次にどこに行くか
	std::vector<llvm::BasicBlock*> band; // False and の次にどこに行くか
	llvm::BasicBlock *bfr;
	llvm::Value *fcmp;
	IfStatementAST *ifs;
	ComparisonAST *com;
	std::string ifStr;
	static llvm::IRBuilderBase::InsertPoint ip;
	// 1:判定方法if, else if, else  2:brを生成するBlock 
	// 3:条件式を満たした際に分岐するBlock 4:比較結果
	std::vector<
		std::tuple<std::string, llvm::BasicBlock*, llvm::BasicBlock*, llvm::Value*, int>
		> blocks;
	int depth;
	std::vector<
		std::tuple<llvm::BasicBlock*, llvm::BasicBlock*, llvm::Value*, ForStatementAST*> 
		> forVals;
	std::map<ForStatementAST*, std::vector<std::tuple<llvm::BasicBlock*, llvm::BasicBlock::iterator>>> breakVals;
	std::map<ForStatementAST*, std::vector<std::tuple<llvm::BasicBlock*, llvm::BasicBlock::iterator>>> continueVals;
	for(int i = 0; ;i++){
		// 最後まで見たら終了
		stmt = func_stmt->getStatement(i);
		if(!stmt)
			break;
		else if(llvm::isa<IfStatementAST>(stmt)){
			if(llvm::dyn_cast<IfStatementAST>(stmt)->getIf() == "if"){
				// 条件式の生成 （まだand or出来ない）
				ifs = llvm::dyn_cast<IfStatementAST>(stmt);
				for(int j = 0; j < ifs->getComparisonNumber(); j++){
					com = ifs->getComparison(j);
					fcmp = generateComparison(com->getLHS(), com->getRHS(), com->getOp(), func_stmt);
					
					// もしand orがあったらBlockとbrを設定する
					if(j < ifs->getOpsNumber()){
						if(ifs->getOp(j) == "or"){
							bblock = llvm::BasicBlock::Create(
									llvm::getGlobalContext(), 
									"or.lhs.false", func);
							
							// lhs or rhs のlhsがtrueならif.thenへ
							// lhs or rhs のlhsがfalseならrhsへ
							blocks.emplace_back("or", 
									Builder->GetInsertBlock(), 
									bblock, fcmp,ifs->getDepth(j));
							
							Builder->SetInsertPoint(bblock);
						}
						else if(ifs->getOp(j) == "and"){
							bblock = llvm::BasicBlock::Create(
									llvm::getGlobalContext(), 
									"and.lhs.true", func);
							
							// lhs and rhsのlhsがtrueならrhsへ
							// lhs and rhsのlhsがfalseならorのrhsへ
							blocks.emplace_back("and", 
									Builder->GetInsertBlock(), 
									bblock, fcmp,ifs->getDepth(j));
							
							Builder->SetInsertPoint(bblock);
						}
					}
				}
				// 条件式を満たした先のBlockを生成
				bblock = llvm::BasicBlock::Create(
						llvm::getGlobalContext(), "if.then", func);
				
				// br情報　
                                blocks.emplace_back("to if", Builder->GetInsertBlock(), bblock, fcmp,
						ifs->getDepth(ifs->getOpsNumber()));
				
				// 条件式が正の場合のBlockにポイントを合わせる
				Builder->SetInsertPoint(bblock);
			}
			else if(llvm::dyn_cast<IfStatementAST>(stmt)->getIf() == "else if"){
				// br情報　if か else if の内部にいる
				blocks.emplace_back("to end", Builder->GetInsertBlock(), bblock, fcmp,0);
				
				// 条件式を評価するBlockを生成
				bblock = llvm::BasicBlock::Create(
						llvm::getGlobalContext(), "if.else", func);

				// if.elseにポイントを合わせる
				Builder->SetInsertPoint(bblock);

				// ifの条件分岐からif.elseに分岐する
				blocks.emplace_back("come if else", 
						Builder->GetInsertBlock(), bblock, fcmp, 0);
				
				// 条件式を生成　（まだand or出来ない）
				ifs = llvm::dyn_cast<IfStatementAST>(stmt);
				for(int j = 0; j < ifs->getComparisonNumber(); j++){
					com = ifs->getComparison(j);
					fcmp = generateComparison(com->getLHS(), com->getRHS(), com->getOp(), func_stmt);
					
					// もしand orがあったらBlockとbrを設定する
					if(j < ifs->getOpsNumber()){
						if(ifs->getOp(j) == "or"){
							bblock = llvm::BasicBlock::Create(
									llvm::getGlobalContext(),
								       	"or.lhs.false",	func);
					
							// lhs or rhs のlhsがtrueならif.thenへ
							// lhs or rhs のlhsがfalseならrhsへ
							blocks.emplace_back("or", 
									Builder->GetInsertBlock(), 
									bblock, fcmp, 
									ifs->getDepth(j));
					
							Builder->SetInsertPoint(bblock);
						}
						else if(ifs->getOp(j) == "and"){
							bblock = llvm::BasicBlock::Create(
									llvm::getGlobalContext(), 
									"and.lhs.true", func);
					
							// lhs and rhsのlhsがtrueならrhsへ
							// lhs and rhsのlhsがfalseならorのrhsへ
							blocks.emplace_back("and", 
									Builder->GetInsertBlock(), 
									bblock, fcmp,
									ifs->getDepth(j));
					
							Builder->SetInsertPoint(bblock);
						}
					}
				}
				
				// 条件式が正を満たした先のBlockを生成
				bblock = llvm::BasicBlock::Create(
						llvm::getGlobalContext(), "if.then", func);
				
				// br情報
				blocks.emplace_back("to else if", Builder->GetInsertBlock(), bblock, fcmp, ifs->getDepth(ifs->getOpsNumber()));

				// 条件が正の場合のBlockにポイントを合わせる
				Builder->SetInsertPoint(bblock);
			}
			else if(llvm::dyn_cast<IfStatementAST>(stmt)->getIf() == "else"){
				// br情報　ifかelse ifにポイントが合っている
				blocks.emplace_back("to end", Builder->GetInsertBlock(), bblock, fcmp, 0);
				
				// elseのBlockを生成
				bblock = llvm::BasicBlock::Create(
						llvm::getGlobalContext(), "if.else", func);

				// elseのBlockにポイントを合わせる
				Builder->SetInsertPoint(bblock);
				
				blocks.emplace_back("come else", 
						Builder->GetInsertBlock(), bblock, fcmp, 0);
			}
		}
		else if(llvm::isa<IfEndAST>(stmt)){
			// 現在のポイントからif.endにbrする
			bend = llvm::BasicBlock::Create(llvm::getGlobalContext(), "if.end", func);
			Builder->CreateBr(bend);

			// brを生成
			bbfr = bend;
			bago = NULL;
			bcur = Builder->GetInsertBlock();
			bfr = bend;
			//bor = Builder->GetInsertBlock();
			//band = bend;
			bor.clear();
			bor.push_back(bend);
			band.clear();
			band.push_back(bend);
			while(true){
				ifStr = std::get<0>(blocks.at(blocks.size()-1));
				bblock = std::get<2>(blocks.at(blocks.size()-1));
				fcmp = std::get<3>(blocks.at(blocks.size()-1));
				depth = std::get<4>(blocks.at(blocks.size()-1));
				if(ifStr == "or"){
					//band = bfr;	
					while(depth >= band.size())
						band.push_back(band.at(band.size()-1));
					
					for(int j = depth; band.size() > j; j++)
						band.at(j) = bfr;
					
					Builder->SetInsertPoint(
							std::get<1>(blocks.at(blocks.size()-1)));

					while(bor.size() <= depth)
						bor.push_back(bor.at(bor.size()-1));
					
					Builder->CreateCondBr(fcmp, bor.at(depth), bblock);
				}
				else if(ifStr == "and"){
					while(depth+1 >= bor.size())
						bor.push_back(bor.at(bor.size()-1));
					
					for(int j = depth+1; bor.size() > j; j++)
						bor.at(j) = bfr;

					Builder->SetInsertPoint(
							std::get<1>(blocks.at(blocks.size()-1)));
					
					while(band.size() <= depth)
						band.push_back(band.at(band.size()-1));

					Builder->CreateCondBr(fcmp, bblock, band.at(depth));
				}
				else{
					bago = bbfr;
					bbfr = bcur;
					bcur = std::get<1>(blocks.at(blocks.size()-1));
					if(ifStr == "to if" || ifStr == "to else if"){
						Builder->SetInsertPoint(bcur);
						Builder->CreateCondBr(fcmp, bblock, bago);
						//bor = bblock;
						bor.clear();
						bor.push_back(bblock);
					}
					else if(ifStr == "come if else" || ifStr == "come else"){
						//band = bcur;
						band.clear();
						band.push_back(bcur);
					}
					else if(ifStr == "to end"){
						Builder->SetInsertPoint(bcur);
						Builder->CreateBr(bend);
					}
				}
				bfr = std::get<1>(blocks.at(blocks.size()-1));
				blocks.pop_back();
				if(ifStr == "to if")
					break;
			}
			while(blocks.size() > 0 && (std::get<0>(blocks.at(blocks.size()-1)) == "or" 
						|| std::get<0>(blocks.at(blocks.size()-1)) == "and")){
				ifStr = std::get<0>(blocks.at(blocks.size()-1));
				bblock = std::get<2>(blocks.at(blocks.size()-1));
				fcmp = std::get<3>(blocks.at(blocks.size()-1));
				depth = std::get<4>(blocks.at(blocks.size()-1));
				if(ifStr == "or"){
					//band = bfr;
					while(depth >= band.size())
						band.push_back(band.at(band.size()-1));

					for(int j = depth; band.size() > j; j++)
						band.at(j) = bfr;
					
					Builder->SetInsertPoint(
							std::get<1>(blocks.at(blocks.size()-1)));
					
					while(bor.size() <= depth)
						bor.push_back(bor.at(bor.size()-1));
					
					Builder->CreateCondBr(fcmp, bor.at(depth), bblock);
				}
				else if(ifStr == "and"){
					while(depth+1 >= bor.size())
						bor.push_back(bor.at(bor.size()-1));

					for(int j = depth+1; bor.size() > j; j++)
						bor.at(j) = bfr;
					
					Builder->SetInsertPoint(
							std::get<1>(blocks.at(blocks.size()-1)));
					
					while(band.size() <= depth)
						band.push_back(band.at(band.size()-1));

					Builder->CreateCondBr(fcmp, bblock, band.at(depth));
				}
				bfr = std::get<1>(blocks.at(blocks.size()-1));
				blocks.pop_back();
			}

			Builder->SetInsertPoint(bend);
		}
		else if(llvm::isa<ForStatementAST>(stmt)){
			ForStatementAST *for_expr = llvm::dyn_cast<ForStatementAST>(stmt);
			llvm::BasicBlock *bcond = llvm::BasicBlock::Create(
					llvm::getGlobalContext(), "for.cond", CurFunc);
			llvm::BasicBlock *bbody = llvm::BasicBlock::Create(
					llvm::getGlobalContext(), "for.body", CurFunc);
			llvm::Value *fcmp = generateForStatement(for_expr, bcond, bbody, func_stmt);
			forVals.emplace_back(bcond, bbody, fcmp, for_expr);
		}
		else if(llvm::isa<ForEndAST>(stmt)){
			llvm::BasicBlock *bcond = std::get<0>(forVals.at(forVals.size()-1));
			llvm::BasicBlock *bbody = std::get<1>(forVals.at(forVals.size()-1));
			llvm::Value *fcmp = std::get<2>(forVals.at(forVals.size()-1));
			ForStatementAST *for_expr = std::get<3>(forVals.at(forVals.size()-1));
			llvm::BasicBlock *binc = llvm::BasicBlock::Create(llvm::getGlobalContext(), "for.inc", CurFunc);
			llvm::BasicBlock *bend = llvm::BasicBlock::Create(llvm::getGlobalContext(), "for.end", CurFunc);
			generateForEndStatement(bcond, bbody, binc, bend, fcmp, for_expr, func_stmt);
			llvm::BasicBlock *temp = Builder->GetInsertBlock();
			forVals.pop_back();

			if(breakVals.find(for_expr) != breakVals.end()){
				for(int i = 0; i < breakVals[for_expr].size(); i++){
					Builder->SetInsertPoint(std::get<0>(breakVals[for_expr].at(i)),	++std::get<1>(breakVals[for_expr].at(i)));
					Builder->CreateBr(bend);
				}
				breakVals[for_expr].clear();
			}

			if(continueVals.find(for_expr) != continueVals.end()){
				for(int i = 0; i < continueVals[for_expr].size(); i++){
					Builder->SetInsertPoint(std::get<0>(continueVals[for_expr].at(i)), ++std::get<1>(continueVals[for_expr].at(i)));
					Builder->CreateBr(binc);
				}
				continueVals[for_expr].clear();
			}
			Builder->SetInsertPoint(temp);
		}
		else if(llvm::isa<BreakAST>(stmt)){
			BreakAST *break_expr = llvm::dyn_cast<BreakAST>(stmt);
			llvm::BasicBlock *bcur = Builder->GetInsertBlock();
			ForStatementAST *for_expr;
			if(break_expr->getDepth() <= 0 || forVals.size() < break_expr->getDepth()){
				fprintf(stderr, "break文のカッコの中の数を確認してください\n");
				return NULL;
			}
			else
				for_expr = std::get<3>(forVals.at(forVals.size()-break_expr->getDepth()));
			if(breakVals.find(for_expr) == breakVals.end()){
				std::vector<std::tuple<llvm::BasicBlock*, llvm::BasicBlock::iterator>> temp;
				breakVals[for_expr] = temp;
			}
			
			if(bcur->empty())
				breakVals[for_expr].emplace_back(bcur, bcur->begin());
			else{
				llvm::BasicBlock::iterator iter = bcur->begin();
				for(int i = 0; i < bcur->size()-1; i++)
					iter++;
				
				breakVals[for_expr].emplace_back(bcur, iter);
			}
		}
		else if(llvm::isa<ContinueAST>(stmt)){
			ContinueAST *continue_expr = llvm::dyn_cast<ContinueAST>(stmt);
			llvm::BasicBlock *bcur = Builder->GetInsertBlock();
			ForStatementAST *for_expr;
			if(continue_expr->getDepth() <= 0 || forVals.size() < continue_expr->getDepth()){
				fprintf(stderr, "continue文のカッコの中の数を確認してください.\n");
				return NULL;
			}
			else
				for_expr = std::get<3>(forVals.at(forVals.size()-continue_expr->getDepth()));

			if(continueVals.find(for_expr) == continueVals.end()){
				std::vector<std::tuple<llvm::BasicBlock*, llvm::BasicBlock::iterator>> temp;
				continueVals[for_expr] = temp;
			}

			if(bcur->empty())
				continueVals[for_expr].emplace_back(bcur, bcur->begin());
			else{
				llvm::BasicBlock::iterator iter = bcur->begin();
				for(int i = 0; i < bcur->size()-1; i++)
					iter++;

				continueVals[for_expr].emplace_back(bcur, iter);
			}
			
		}
		else if(llvm::isa<GlobalVariableAST>(stmt)){
			GlobalVariableAST *gVar = llvm::dyn_cast<GlobalVariableAST>(stmt);
			llvm::Value *check = Mod->getNamedGlobal(gVar->getName());
			if(!check){
				fprintf(stderr, "%d行目 : global で宣言された変数 %s はありません.\n", gVar->getLine(), gVar->getName().c_str());
				CORRECT = false;
			}
		}else if(!llvm::isa<NullExprAST>(stmt))
			v = generateStatement(stmt, func_stmt);
	}
	return v;
}

/**
 * 変数宣言(alloca命令)生成メソッド
 * @param VariableDeclAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateVariableDeclaration(VariableDeclAST *vdecl){
	if(CurFunc->getName().str() == "main"){
		Mod->getOrInsertGlobal(vdecl->getName(), llvm::Type::getDoubleTy(llvm::getGlobalContext()));
		llvm::GlobalVariable *gvar = Mod->getNamedGlobal(vdecl->getName());
		gvar->setLinkage(llvm::GlobalValue::CommonLinkage);
		gvar->setInitializer(llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 0));
		return gvar;
	}
	else{
		// create alloca
		llvm::AllocaInst *alloca = NULL;
		
		if(vdecl->getIdentify() == VariableDeclAST::dint)
			alloca = Builder->CreateAlloca(llvm::Type::getInt32Ty(
				llvm::getGlobalContext()), 0, vdecl->getName());
		if(vdecl->getIdentify() == VariableDeclAST::ddouble)
			alloca = Builder->CreateAlloca(llvm::Type::getDoubleTy(
				llvm::getGlobalContext()), 0, vdecl->getName());

		// if args alloca
		if(vdecl->getType() == VariableDeclAST::param){
			// store args
			llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
			Builder->CreateStore(vs_table.lookup(vdecl->getName().append("_arg")), alloca);
		}
		return alloca;
	}
}

/**
 * ステートメント生成メソッド
 * 実際にはASTの種類を確認して各種生成メソッドを呼び出し
 * @param BaseAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateStatement(BaseAST *stmt, FunctionStmtAST *func_stmt){
	if(llvm::isa<BinaryExprAST>(stmt)){
		return generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(stmt), func_stmt);
	}else if(llvm::isa<CallExprAST>(stmt)){
		return generateCallExpression(llvm::dyn_cast<CallExprAST>(stmt), func_stmt);
	}else if(llvm::isa<ReturnStmtAST>(stmt)){
		return generateReturnStatement(llvm::dyn_cast<ReturnStmtAST>(stmt), func_stmt);
	}else{
		return NULL;
	}
}

/**
 * 二項演算生成メソッド
 * @param JumpStmtAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateBinaryExpression(BinaryExprAST *bin_expr, FunctionStmtAST *func_stmt){
	BaseAST *lhs = bin_expr->getLHS();
	BaseAST *rhs = bin_expr->getRHS();
	
	llvm::Value *lhs_v;
	llvm::Value *rhs_v;

	// = の場合に代入先に指定するValueを格納する
	llvm::Value *assigned_v;
	VariableAST *lhs_var;
	
	// assignment
	if(bin_expr->getOp() == "="){
		// lhs is variable
		lhs_var = llvm::dyn_cast<VariableAST>(lhs);
		if(CurFunc->getName().str() == "main" || func_stmt->isGlobalVariable(lhs_var->getName()))
			assigned_v = Mod->getNamedGlobal(lhs_var->getName());
		else{
			llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
			assigned_v = vs_table.lookup(lhs_var->getName());
		}
		lhs_v = generateVariable(lhs_var, func_stmt);

	// other operand
	}else{
		// lhs = ?
		// Binary?
		if(llvm::isa<BinaryExprAST>(lhs)){
			lhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(lhs), func_stmt);
		
		// CallExpr?
		}else if(llvm::isa<CallExprAST>(lhs)){
			lhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(lhs), func_stmt);
			
		// Variable?
		}else if(llvm::isa<VariableAST>(lhs)){
			lhs_v = generateVariable(llvm::dyn_cast<VariableAST>(lhs), func_stmt);

		// Number?
		}else if(llvm::isa<NumberAST>(lhs)){
			NumberAST *num = llvm::dyn_cast<NumberAST>(lhs);
			lhs_v = generateNumber(num->getNumberValue());
		}
	}

	// create rhs value
	if(llvm::isa<BinaryExprAST>(rhs)){
		rhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(rhs), func_stmt);
	
	// CallExpr?
	}else if(llvm::isa<CallExprAST>(rhs)){
		rhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(rhs), func_stmt);
	
	// Variable?
	}else if(llvm::isa<VariableAST>(rhs)){
		rhs_v = generateVariable(llvm::dyn_cast<VariableAST>(rhs), func_stmt);
	
	// Number?
	}else if(llvm::isa<NumberAST>(rhs)){
		NumberAST *num = llvm::dyn_cast<NumberAST>(rhs);
		rhs_v = generateNumber(num->getNumberValue());
	}
	
	// コード生成
	if(bin_expr->getOp() == "="){
		// store
		return Builder->CreateStore(rhs_v, assigned_v);
	
	}
	
	if(bin_expr->getOp() == "+"){
		// add 
		return Builder->CreateFAdd(lhs_v, rhs_v, "add_tmp");
		
	}else if(bin_expr->getOp() == "-"){
		// sub
		return Builder->CreateFSub(lhs_v, rhs_v, "sub_tmp");
		
	}else if(bin_expr->getOp() == "*"){
		// mul
		return Builder->CreateFMul(lhs_v, rhs_v, "mul_tmp");
		
	}else if(bin_expr->getOp() == "/"){
		generateDenominatorCheck("割り算", rhs, bin_expr->getLine(), func_stmt);
		
		// div
		return Builder->CreateFDiv(lhs_v, rhs_v,"div_tmp");
	}
	else if(bin_expr->getOp() == "//"){
		generateDenominatorCheck("割り切り算", rhs, bin_expr->getLine(), func_stmt);
		
		// div
		llvm::Value *div_tmp = Builder->CreateFDiv(lhs_v, rhs_v, "div_tmp");
		div_tmp = Builder->CreateCast(llvm::Instruction::FPToSI, div_tmp,
				llvm::Type::getInt32Ty(llvm::getGlobalContext()));
		div_tmp = Builder->CreateCast(llvm::Instruction::SIToFP, div_tmp,
				llvm::Type::getDoubleTy(llvm::getGlobalContext()));
		return div_tmp;
	}
	else if(bin_expr->getOp() == "%"){
		generateDenominatorCheck("余り演算", rhs, bin_expr->getLine(), func_stmt);
		
		// rem
		return Builder->CreateFRem(lhs_v, rhs_v, "rem_tmp");
	}
	return NULL;
}


/**
 * 関数呼び出し（Call命令）生成メソッド
 * @param CallExprAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateCallExpression(CallExprAST *call_expr, FunctionStmtAST *func_stmt){
	std::vector<llvm::Value*> arg_vec;
	BaseAST *arg;
	llvm::Value *arg_v;
	llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
	std::string Str = "";
	llvm::Value *val;
	if(call_expr->getCallee() == "print" || call_expr->getCallee() == "input")
		arg_vec.push_back(generateString(""));
	
	if(call_expr->getCallee() == "input"){
		VariableAST *var;
		llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
		for(int i = 0; ; i++){
			if(!(arg = call_expr->getArgs(i)))
				break;

			if(!llvm::isa<VariableAST>(arg)){
				fprintf(stderr, "inputのカッコ内の変数を確認してください.\n");
				return NULL;
			}
			var = llvm::dyn_cast<VariableAST>(arg);
			if(CurFunc->getName().str() == "main" || func_stmt->isGlobalVariable(var->getName()))
					arg_vec.push_back(Mod->getNamedGlobal(var->getName()));
			else
				arg_vec.push_back(vs_table.lookup(var->getName()));
		}

		Str = "%lf";
		for(int i = 0; i < arg_vec.size()-2; i++)
			Str += " %lf";
		arg_vec.at(0) = generateString(Str);
		return Builder->CreateCall(Mod->getFunction("__isoc99_scanf"), arg_vec, "call_temp");
	}

	for(int i = 0; ;i++){
		if(!(arg = call_expr->getArgs(i)))
			break;
		
		// isCall
		if(llvm::isa<CallExprAST>(arg)){
			arg_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(arg), func_stmt);
		
			// isBinaryExpr
		}else if(llvm::isa<BinaryExprAST>(arg)){
			BinaryExprAST *bin_expr = llvm::dyn_cast<BinaryExprAST>(arg);
			arg_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(arg), func_stmt);
			if(bin_expr->getOp() == "="){
				VariableAST *var = llvm::dyn_cast<VariableAST>(bin_expr->getLHS());
				arg_v = generateVariable(var, func_stmt);
			}
		}
		// isVar
		else if(llvm::isa<VariableAST>(arg)){
			arg_v = generateVariable(llvm::dyn_cast<VariableAST>(arg), func_stmt);
		
		// isNumber
		}else if(llvm::isa<NumberAST>(arg)){
			NumberAST *num = llvm::dyn_cast<NumberAST>(arg);
			arg_v = generateNumber(num->getNumberValue());
	
		// string
		}else if(llvm::isa<StringAST>(arg)){
			StringAST *str = llvm::dyn_cast<StringAST>(arg);
			arg_v = generateString(str->getStringValue());
		
		// NewLine
		}else if(llvm::isa<NewLineAST>(arg)){}

		if(call_expr->getCallee() == "print"){
			llvm::Value *print_string;
			std::vector<llvm::Value*> print_vec;
			std::vector<llvm::Value*> indices;
			std::vector<llvm::Value*> memset_vec;
			indices.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0));
			indices.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0));
			llvm::AllocaInst *print_str = Builder->CreateAlloca(llvm::ArrayType::get(llvm::Type::getInt8Ty(llvm::getGlobalContext()), 1));
			memset_vec.push_back(Builder->CreatePointerCast(print_str, llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), "print_str_temp"));
			memset_vec.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(llvm::getGlobalContext()), 0));
			memset_vec.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 1));
			memset_vec.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 1));
			memset_vec.push_back(llvm::ConstantInt::get(llvm::Type::getInt1Ty(llvm::getGlobalContext()), false));
			Builder->CreateCall(Mod->getFunction("llvm.memset.p0i8.i64"), memset_vec, "call_temp");
			if(llvm::isa<StringAST>(arg)){
				print_vec.push_back(generateString("%s "));
				print_vec.push_back(arg_v);
				val = Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");
			}else if(llvm::isa<NewLineAST>(arg)){
				print_vec.push_back(generateString("\n"));
				val = Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");
			}else if(arg_v->getType()->isDoubleTy()){
				std::string width;
				std::string digit;
				
				if(llvm::isa<VariableAST>(arg)){
					width = llvm::dyn_cast<VariableAST>(arg)->getWidth();
					digit = llvm::dyn_cast<VariableAST>(arg)->getDigit();
				}
				else if(llvm::isa<NumberAST>(arg)){
					width = llvm::dyn_cast<NumberAST>(arg)->getWidth();
					digit = llvm::dyn_cast<NumberAST>(arg)->getDigit();
				}
				else if(llvm::isa<BinaryExprAST>(arg)){
					width = llvm::dyn_cast<BinaryExprAST>(arg)->getWidth();
					digit = llvm::dyn_cast<BinaryExprAST>(arg)->getDigit();
				}
				else if(llvm::isa<CallExprAST>(arg)){
					width = llvm::dyn_cast<CallExprAST>(arg)->getWidth();
					digit = llvm::dyn_cast<CallExprAST>(arg)->getDigit();
				}
				else{
					fprintf(stderr, "printに予期しない引数の型があります\n");
					return NULL;
				}
				
				if(digit == "-1"){
					val = Builder->CreateInBoundsGEP(print_str,indices, "print_array");
					print_vec.push_back(val);
					print_vec.push_back(generateString("%%%s.0f "));
					print_vec.push_back(generateString(width));
					Builder->CreateCall(Mod->getFunction("sprintf"), print_vec, "call_temp");
					print_vec.clear();
					print_vec.push_back(Builder->CreateInBoundsGEP(print_str, indices, "print_array"));
					print_vec.push_back(arg_v);
					val = Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");
				}
				else if(digit == "-2"){
					llvm::Value *mod = Builder->CreateFRem(arg_v, generateNumber(1), "rem_temp");
					llvm::Value *fcmp = Builder->CreateFCmpOEQ(mod, generateNumber(0), "cmp");
					llvm::BasicBlock *integer = llvm::BasicBlock::Create(llvm::getGlobalContext(), "int_arg", CurFunc);
					llvm::BasicBlock *decimal = llvm::BasicBlock::Create(llvm::getGlobalContext(), "dec_arg", CurFunc);
					llvm::BasicBlock *end = llvm::BasicBlock::Create(llvm::getGlobalContext(), "end_arg", CurFunc);
					Builder->CreateCondBr(fcmp, integer, decimal);
					Builder->SetInsertPoint(integer);
					
					
					// int だったら
					val = Builder->CreateInBoundsGEP(print_str,indices, "print_array");
					print_vec.push_back(val);
					print_vec.push_back(generateString("%%%s.0f "));
					print_vec.push_back(generateString(width));
					Builder->CreateCall(Mod->getFunction("sprintf"), print_vec, "call_temp");
					print_vec.clear();
					print_vec.push_back(Builder->CreateInBoundsGEP(print_str, indices, "print_array"));
					print_vec.push_back(arg_v);
					Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");					

					Builder->CreateBr(end);
					
					// print_str.clear()
					print_vec.clear();

					Builder->SetInsertPoint(decimal);

					// decだったら
					val = Builder->CreateInBoundsGEP(print_str,indices, "print_array");
					print_vec.push_back(val);
					print_vec.push_back(generateString("%%.5f "));
					Builder->CreateCall(Mod->getFunction("sprintf"), print_vec, "call_temp");
					print_vec.clear();

					print_vec.push_back(Builder->CreateInBoundsGEP(print_str, indices, "print_array"));
					print_vec.push_back(arg_v);
					val = Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");
					Builder->CreateBr(end);

					Builder->SetInsertPoint(end);
				}else{
					val = Builder->CreateInBoundsGEP(print_str,indices, "print_array");
					print_vec.push_back(val);
					print_vec.push_back(generateString("%%%s.%sf "));
					print_vec.push_back(generateString(width));
					print_vec.push_back(generateString(digit));
					Builder->CreateCall(Mod->getFunction("sprintf"), print_vec, "call_temp");
					print_vec.clear();
					print_vec.push_back(Builder->CreateInBoundsGEP(print_str, indices, "print_array"));
					print_vec.push_back(arg_v);
					val = Builder->CreateCall(Mod->getFunction("printf"), print_vec, "call_temp");
				}
			}
		}
		else {
			arg_vec.push_back(arg_v);
		}

	}
	if(call_expr->getCallee() == "print"){
		return val;
	}
	else
		return Builder->CreateCall(Mod->getFunction(call_expr->getCallee()), arg_vec, "call_temp");
}

/**
 * ジャンプ（今回はreturn 命令のみ）生成メソッド
 * @param JumpStmtAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateReturnStatement(ReturnStmtAST *jump_stmt, FunctionStmtAST *func_stmt){
	BaseAST *expr = jump_stmt->getExpr();
	llvm::Value *ret_v;
	
	if(llvm::isa<BinaryExprAST>(expr)){
		ret_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(expr), func_stmt);
	}if(llvm::isa<CallExprAST>(expr)){
		CallExprAST *call_expr = llvm::dyn_cast<CallExprAST>(expr);
		ret_v = generateCallExpression(call_expr, func_stmt);
	}else if(llvm::isa<VariableAST>(expr)){
		VariableAST *var = llvm::dyn_cast<VariableAST>(expr);
		ret_v = generateVariable(var, func_stmt);
	}else if(llvm::isa<NumberAST>(expr)){
		NumberAST *num = llvm::dyn_cast<NumberAST>(expr);
		ret_v = generateNumber(num->getNumberValue());
	}
	if(!ret_v)
		return NULL;
	else{
		if(CurFunc->getReturnType()->isIntegerTy() && ret_v->getType()->isDoubleTy())
			ret_v = Builder->CreateCast(llvm::Instruction::FPToSI, ret_v,
					llvm::Type::getInt32Ty(llvm::getGlobalContext()), "int_tmp");
		else if(CurFunc->getReturnType()->isDoubleTy() && ret_v->getType()->isIntegerTy())
			ret_v = Builder->CreateCast(llvm::Instruction::SIToFP, ret_v,
					llvm::Type::getDoubleTy(llvm::getGlobalContext()), "double_tmp");
		Builder->CreateRet(ret_v);
		return ret_v;
	}
}

/**
 * 変数参照（load命令）生成メソッド
 * @param VariableAST
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateVariable(VariableAST *var, FunctionStmtAST *func_stmt){
	llvm::Value *value;
	if(CurFunc->getName().str() == "main" || func_stmt->isGlobalVariable(var->getName())){
		value = Mod->getNamedGlobal(var->getName());
	}else{
		llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
		value = vs_table.lookup(var->getName());
	}
	return Builder->CreateLoad(value, "var_temp");
}

/**
 * 定数生成メソッド
 * @param 生成する定数の値
 * @return 生成したValueのポインタ
 */
llvm::Value *CodeGen::generateNumber(double value){
	return llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), value);
}

/**
 * 文字列生成メソッド
 * @param 生成する文字列
 * @return 生成したValueポインタ
 */
llvm::Value *CodeGen::generateString(std::string str){
	return Builder->CreateGlobalStringPtr(str, ".str");
}

/**
 * Module結合用メソッド
 */
bool CodeGen::linkModule(llvm::Module *dest, std::string file_name){
	llvm::SMDiagnostic err;
	// Moduleの読み込み
	llvm::Module *link_mod = llvm::ParseIRFile(file_name, err, llvm::getGlobalContext());
	if(!link_mod)
		return false;

	// Moduleの結合
	std::string err_msg;
	if(llvm::Linker::LinkModules(dest, link_mod, llvm::Linker::DestroySource, &err_msg))
		return false;

	SAFE_DELETE(link_mod);

	return true;
}





//////////////////////////////////////////////////////
//比較を生成
//////////////////////////////////////////////////////
llvm::Value *CodeGen::generateComparison(BaseAST *lhs, BaseAST *rhs, std::string op, FunctionStmtAST *func_stmt){
	llvm::Value *lhs_v;
	llvm::Value *rhs_v;
	
	// 左辺値取得
	if(llvm::isa<BinaryExprAST>(lhs)){
		lhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(lhs), func_stmt);
	}else if(llvm::isa<CallExprAST>(lhs)){
		lhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(lhs), func_stmt);
	}else if(llvm::isa<VariableAST>(lhs)){
		lhs_v = generateVariable(llvm::dyn_cast<VariableAST>(lhs), func_stmt);
	}else if(llvm::isa<NumberAST>(lhs)){
		lhs_v = generateNumber(llvm::dyn_cast<NumberAST>(lhs)->getNumberValue());
	}else{
		fprintf(stderr, "タイプ%dの左辺値が取得できません\n", lhs->getValueID());
		return NULL;
	}

	// 右辺値取得
	if(llvm::isa<BinaryExprAST>(rhs)){
		rhs_v = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(rhs), func_stmt);
	}else if(llvm::isa<CallExprAST>(rhs)){
		rhs_v = generateCallExpression(llvm::dyn_cast<CallExprAST>(rhs), func_stmt);
	}else if(llvm::isa<VariableAST>(rhs)){
		rhs_v = generateVariable(llvm::dyn_cast<VariableAST>(rhs), func_stmt);
	}else if(llvm::isa<NumberAST>(rhs)){
		rhs_v = generateNumber(llvm::dyn_cast<NumberAST>(rhs)->getNumberValue());
	}else{
		fprintf(stderr, "右辺値が取得できません\n");
		return NULL;
	}
	
	// コード生成
	if(op == "==")
		return Builder->CreateFCmpOEQ(lhs_v, rhs_v, "cmp");
	else if(op == ">=")
		return Builder->CreateFCmpOGE(lhs_v, rhs_v, "cmp");
	else if(op == ">")
		return Builder->CreateFCmpOGT(lhs_v, rhs_v, "cmp");
	else if(op == "<=")
		return Builder->CreateFCmpOLE(lhs_v, rhs_v, "cmp");
	else if(op == "<")
		return Builder->CreateFCmpOLT(lhs_v, rhs_v, "cmp");
	else if(op == "!=")
		return Builder->CreateFCmpONE(lhs_v, rhs_v, "cmp");
	else
		return NULL;
}

// forを表すllvmirを生成
llvm::Value *CodeGen::generateForStatement(ForStatementAST *for_expr, llvm::BasicBlock *bcond, llvm::BasicBlock *bbody, FunctionStmtAST *func_stmt){
	// 繰り返し変数の設定
	if(!llvm::isa<BinaryExprAST>(for_expr->getBinExpr()) && for_expr->getBinExpr()->getOp() == "="){
		fprintf(stderr, "for 繰り返し数 である必要があります。\n");
		SAFE_DELETE(bcond);
		SAFE_DELETE(bbody);
		return NULL;
	}
	generateBinaryExpression(for_expr->getBinExpr(), func_stmt);
	
	// to for.condへ
	Builder->CreateBr(bcond);
	Builder->SetInsertPoint(bcond);

	// for cond
	llvm::Value *roop_variable = generateVariable(for_expr->getVal(), func_stmt);
	llvm::Value *end_val;
       	if(llvm::isa<BinaryExprAST>(for_expr->getEndExpr()))
		end_val = generateBinaryExpression(llvm::dyn_cast<BinaryExprAST>(for_expr->getEndExpr()), func_stmt);	
	else if(llvm::isa<VariableAST>(for_expr->getEndExpr()))
		end_val = generateVariable(llvm::dyn_cast<VariableAST>(for_expr->getEndExpr()), func_stmt);
	else if(llvm::isa<NumberAST>(for_expr->getEndExpr()))
		end_val = generateNumber(llvm::dyn_cast<NumberAST>(for_expr->getEndExpr())->getNumberValue());
	if(!end_val){
		fprintf(stderr, "for 繰り返し数  である必要があります\n");
		return NULL;
	}
	llvm::Value *fcmp = Builder->CreateFCmpOLE(roop_variable, end_val, "cmp");
	
	// for.bodyにbr for.incに設定
	Builder->SetInsertPoint(bbody);
	return fcmp;
}

// forEndを表すllvmirを生成
llvm::BasicBlock *CodeGen::generateForEndStatement(llvm::BasicBlock *bcond, llvm::BasicBlock *bbody, llvm::BasicBlock *binc, llvm::BasicBlock *bend, llvm::Value *fcmp, ForStatementAST *for_expr, FunctionStmtAST *func_stmt){
	// for.bodyからfor.incへ
	Builder->CreateBr(binc);
	Builder->SetInsertPoint(binc);

	// for.incの生成
	llvm::Value *roop_var = generateVariable(for_expr->getVal(), func_stmt);
	llvm::Value *temp_var = Builder->CreateFAdd(roop_var, generateNumber(1.0), "add_tmp");
	if(CurFunc->getName().str() == "main" || func_stmt->isGlobalVariable(for_expr->getVal()->getName())){
		roop_var = Mod->getNamedGlobal(for_expr->getVal()->getName());
	}else{
		llvm::ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
		roop_var = vs_table.lookup(for_expr->getVal()->getName());
	}
	Builder->CreateStore(temp_var, roop_var);
	
	Builder->CreateBr(bcond);

	// for.condの最後にbrを生成
	Builder->SetInsertPoint(bcond);
	Builder->CreateCondBr(fcmp, bbody, bend);

	// for.endにpointを設定
	Builder->SetInsertPoint(bend);

	return bend;
}

/**
 * 割り算の分母確認
 */
bool CodeGen::generateDenominatorCheck(std::string op, BaseAST *rhs, int line, FunctionStmtAST *func_stmt){
	llvm::Value *fcmp = generateComparison(rhs, new NumberAST(0), "==", func_stmt);
	llvm::BasicBlock *zero = llvm::BasicBlock::Create(llvm::getGlobalContext(), "denominator_zero", CurFunc);
	llvm::BasicBlock *not_zero = llvm::BasicBlock::Create(llvm::getGlobalContext(), "not_denominator_zero", CurFunc);
	Builder->CreateCondBr(fcmp, zero, not_zero);
	Builder->SetInsertPoint(zero);
	std::vector<llvm::Value*> arg_vec;
	std::string error_denominator_zero = std::to_string(line) + "行目 : " + op + "の分母が 0 です.\n";
	arg_vec.push_back(generateString(error_denominator_zero));
	Builder->CreateCall(Mod->getFunction("printf"), arg_vec, "call_temp");
	Builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0));
	Builder->SetInsertPoint(not_zero);
	return true;
}
