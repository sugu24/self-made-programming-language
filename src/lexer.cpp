#include "lexer.hpp"

/**
 * トークンの切り出し関数
 * @ param 字句解析対象ファイル名
 * @ return 切り出したトークンを格納したTokenStream
 */
TokenStream *LexicalAnalysis(std::string input_filename){
	TokenStream *tokens = new TokenStream();
	std::ifstream ifs;
	std::string cur_line;
	std::string token_str;
	int line_num = 1;
	bool iscomment = false;
	
	ifs.open(input_filename.c_str(), std::ios::in);
	if (!ifs){
		fprintf(stderr, "file is not found\n");
		return NULL;
	}
	
	while (ifs && getline(ifs,cur_line)){
		char next_char;
		std::string line;
		Token *next_token;
		int index = 0;
		int length = cur_line.length();
		std::vector<Token*> line_tokens;
		cur_line += ":";

		while(index < length){
			next_char = cur_line.at(index++);
			if(next_char == ':' && index == length)
				break;
			next_token = NULL;
			// コメント読み飛ばし
			if(iscomment){
				if (length > index && next_char == '#' && cur_line.at(index) == '#'){
					index++;
					iscomment = false;
				}
				continue;
			}

			// EOF
			if (next_char == EOF){
				token_str = EOF;
				next_token = new Token(token_str, TOK_EOF, line_num);
			}else if (isspace(next_char)){
				continue;
			}else if(next_char == '\"'){
				next_char = cur_line.at(index++);
				while(index != length && next_char != '\"'){
					token_str += next_char;
					next_char = cur_line.at(index++);
				}
				if(next_char == '\"')
					next_token = new Token(token_str, TOK_STR, line_num);
				else{
					printf("文字列を閉じる\"がありません");
					return NULL;
				}
			}else if (isalpha(next_char)){
				token_str += next_char;
				next_char = cur_line.at(index++);
				while (isalnum(next_char) || next_char == '_'){
					token_str += next_char;
					next_char = cur_line.at(index++);
					if (index == length){
						break;
					}
				}
				index--;

				if (token_str == "return")
					next_token = new Token(token_str,TOK_RETURN,line_num);
				else if(token_str == "and")
					next_token = new Token(token_str, TOK_AND, line_num);
				else if(token_str == "or")
					next_token = new Token(token_str, TOK_OR, line_num);
				else if(token_str == "global")
					next_token = new Token(token_str, TOK_GLOBAL, line_num);
				else if(token_str == "break")
					next_token = new Token("break", TOK_BREAK, line_num);
				else if(token_str == "continue")
					next_token = new Token("continue", TOK_CONTINUE, line_num);
				else
					next_token = new Token(token_str,TOK_IDENTIFIER,line_num);

			// 数字
			}else if (isdigit(next_char)){
				if (next_char == '0'){
					token_str += next_char;
					next_char = cur_line.at(index++);
					if(next_char == '.'){
						char buff = next_char;
						next_char = cur_line.at(index++);
						if(isdigit(next_char)){
							token_str += buff;
							while(isdigit(next_char)){
								token_str += next_char;
								next_char = cur_line.at(index++);
							}
						}
						else
							index--;
					}
					index--;
					next_token = new Token(token_str, TOK_DIGIT, line_num);
				}else{
					token_str += next_char;
					next_char = cur_line.at(index++);
					while (isdigit(next_char)){
						token_str += next_char;
						next_char = cur_line.at(index++);
					}
					if(next_char == '.'){
						char buff = next_char;
						next_char = cur_line.at(index++);
						if(isdigit(next_char)){
							token_str += buff;
							while(isdigit(next_char)){
								token_str += next_char;
								next_char = cur_line.at(index++);
							}
						}
						else
							index--;
					}
					index--;
					next_token = new Token(token_str, TOK_DIGIT, line_num);
				}
			}else if(next_char == '\"'){
				next_char = cur_line.at(index++);
				while(true){
					token_str += next_char;
					next_char = cur_line.at(index++);
					if(next_char == '\"')
						next_token = new Token(token_str, TOK_STR, line_num);
					if(index == length){
						fprintf(stderr, "There are not \". please check it.");
						SAFE_DELETE(tokens);
						return NULL;
					}
				}
					
			// コメント
			}else if (next_char == '#'){
				next_char = cur_line.at(index++);
				
				//コメントが##○○##の場合
				if(next_char == '#'){
					iscomment = true;
					continue;
				}else{
					index--;
					break;
				}
			//それ以外 (記号)
			}else{
				if (next_char == '*' ||
				    next_char == '+' ||
				    next_char == '-' ||
				    next_char == '/' ||
				    next_char == '%' ||
				    next_char == '!' ||
				    next_char == '=' ||
				    next_char == '<' ||
				    next_char == '.' ||
				    next_char == '>' ||
				    next_char == ',' || 
				    next_char == '(' ||
				    next_char == ')' ||
				    next_char == '?' ||
				    next_char == '^' ||
				    next_char == '\\'||
				    next_char == '[' ||
				    next_char == ']' ||
				    next_char == '{' ||
				    next_char == '}' ||
				    next_char == ';'){
					token_str += next_char;
					if(index < length){
						next_char = cur_line.at(index++);
						if(token_str == "=" && next_char == '=')
							token_str += next_char;
						else if(token_str == "<" && next_char == '=')
							token_str += next_char;
						else if(token_str == "<" && next_char == '-')
							token_str += next_char;
						else if(token_str == ">" && next_char == '=')
							token_str += next_char;
						else if(token_str == "!" && next_char == '=')
							token_str += next_char;
						else if(token_str == "." && next_char == '.')
							token_str += next_char;
						else if(token_str == "?" && next_char == '?')
							token_str += next_char;
						else if(token_str == "-" && next_char == '>')
							token_str += next_char;
						else if(token_str == "/" && next_char == '/')
							token_str += next_char;
						else if(token_str == "+" && next_char == '=')
							token_str += next_char;
						else if(token_str == "-" && next_char == '=')
							token_str += next_char;
						else if(token_str == "*" && next_char == '=')
							token_str += next_char;
						else if(token_str == "/" && next_char == '=')
							token_str += next_char;
						else if(token_str == "%" && next_char == '=')
							token_str += next_char;
						else if(token_str == "^" && next_char == '-')
							token_str += next_char;
						else
							index--;
						
						if(token_str == "//"){
							if(length > index){
								next_char = cur_line.at(index++);
								if(next_char == '=')
									token_str += next_char;
								else
									index--;
							}
						}
					}
					if(token_str == "..")
						next_token = new Token("for", TOK_FOR, line_num);
					else if(token_str == "?")
						next_token = new Token("if", TOK_IF, line_num);
					else if(token_str == "??")
						next_token = new Token("else if", TOK_ELSE_IF, line_num);
					else if(token_str == "->")
						next_token = new Token("else", TOK_ELSE, line_num);
					else
						next_token = new Token(token_str,TOK_SYMBOL,line_num);
				// 解析不可能
				}else{
					fprintf(stderr,"%d行目 : %cは予期せぬ文字です.\n", line_num, next_char);
					SAFE_DELETE(tokens);
					return NULL;
				}
			}
			// Tokensに追加
			if(next_token->getTokenType() == TOK_FOR){
				if(line_tokens.size() == 0)
					line_tokens.push_back(next_token);
				else
					line_tokens.insert(line_tokens.begin(), next_token);
				line_tokens.push_back(new Token("..", TOK_SYMBOL, line_num));
			}
			else
				line_tokens.push_back(next_token);
			token_str.clear();
			
		}
		
		for(int i = 0; i < line_tokens.size(); i++)
			tokens->pushToken(line_tokens.at(i));
		if(line_tokens.size() != 0 &&
				line_tokens.at(line_tokens.size()-1)->getTokenString() != ";" &&
				line_tokens.at(line_tokens.size()-1)->getTokenString() != "{" &&
				line_tokens.at(line_tokens.size()-1)->getTokenString() != "}")
			tokens->pushToken(new Token(";", TOK_SYMBOL, line_num));
		token_str.clear();
		line_tokens.clear();
		line_num++;
	}
	std::vector<Token*> mains;
	std::vector<Token*> buff;
	Token *token;
	bool block = true;
	bool is_main = false;
	bool last = false;
	bool iffor = false;
	int kakko = 0;
	int loop = tokens->getTokensSize();
	
	for(int i = 0; i < loop; i++){
		token = tokens->getToken(0);
		tokens->eraseToken(0);	

		block = true;
		if(token->getTokenType() == TOK_FOR || token->getTokenType() == TOK_IF || token->getTokenType() == TOK_ELSE_IF || token->getTokenType() == TOK_ELSE){
			iffor = true;
		}

		if(token->getTokenString() == "{"){
			if(iffor)
				iffor = false;
			else if(!iffor && kakko == 0){
				is_main = true;
				block = false;
			}
			kakko += 1;
		}
		else if(token->getTokenString() == "}"){
			kakko -= 1;
			if(kakko == 0 && is_main){
				is_main = false;
				block = false;
			}
		}

		if(block && i == loop-1){
			is_main ^= true;
			last = true;
		}
		buff.push_back(token);

		if(!block || last){
			int block_index = -1;
			if(!is_main || last)
				block_index = buff.size();
			else{
				for(int j = buff.size()-1; j >= 0; j--){
					if(buff.at(j)->getTokenString() == "}" || buff.at(j)->getTokenString() == ";"){
						block_index = j+1;
						break;
					}
				}
			}
			
			if(is_main){
				for(int j = 0; j < block_index; j++){
					mains.push_back(buff.at(0));
					buff.erase(buff.begin());
				}
			}
			else{
				for(int j = 0; j < block_index; j++){
					tokens->pushToken(buff.at(0));
					buff.erase(buff.begin());
				}
				// エラーチェックのため
				if(block_index > 0)
					tokens->pushToken(new Token("]]", TOK_SYMBOL, -1));
			}
		}
	}
	if(tokens->getTokensSize() != 0 && tokens->getToken(tokens->getTokensSize()-1)->getTokenString() != "]]"){
		fprintf(stdout, "最後に作成した関数の最後に } を挿入します.\n");
		tokens->pushToken(new Token("}", TOK_SYMBOL, -1));
		tokens->pushToken(new Token("]]", TOK_SYMBOL, -1));
	}
	tokens->pushToken(new Token("main", TOK_IDENTIFIER, -1));
	tokens->pushToken(new Token("(", TOK_SYMBOL, -1));
	tokens->pushToken(new Token(")", TOK_SYMBOL, -1));
	tokens->pushToken(new Token("{", TOK_SYMBOL, -1));
	for(int i = 0; i < mains.size(); i++)
		tokens->pushToken(mains.at(i));
	tokens->pushToken(new Token("}", TOK_SYMBOL, -1));
	
	//tokens->printTokens();
	//return NULL;
	
	// EOFの確認
	if (ifs.eof()){
		tokens->pushToken(new Token(token_str,TOK_EOF,line_num));
	}
	// クローズ
	ifs.close();
	return tokens;
}

/**
 *  * デストラクタ
 *   */
TokenStream::~TokenStream(){
	for(int i=0;i<Tokens.size();i++){
		SAFE_DELETE(Tokens[i]);
	}
	Tokens.clear();
}

/**
 *  * トークン取得
 *   * @return CurIndex番目のToken
 *    */
Token TokenStream::getToken(){
	return *Tokens[CurIndex];
}

/**
 *  * インデックスを1つ増やして次のトークンに進める
 *   * @return 成功時:true 失敗時:false
 *    */
bool TokenStream::getNextToken(){
	int size = Tokens.size();
	if (--size<=CurIndex){
		return false;
	}else{
		CurIndex++;
	        return true;
	}
}

/*
 * インデックスを1つ減らして前のトークンに戻る
 * @return 成功時:true 失敗時:false
 */
bool TokenStream::getBackToken(){
	if(CurIndex > 0){
		CurIndex--;
		return true;
	}else
		return false;
}

/**
 *  * インデックスをtimes回回す
 *   */
bool TokenStream::ungetToken(int times){
	for(int i=0;i<times;i++){
		if(CurIndex == 0)
			return false;
		else
			CurIndex--;
	}
	return true;
}

/**
 *  * 格納されたトークン一覧を表示する
 *   */
bool TokenStream::printTokens(){
	std::vector<Token*>::iterator titer = Tokens.begin();
	while(titer != Tokens.end()){
		fprintf(stdout,"%d:",(*titer)->getTokenType());
		if((*titer)->getTokenType() != TOK_EOF)
			fprintf(stdout,"%s %d\n",(*titer)->getTokenString().c_str(), (*titer)->getLine());
		++titer;
	}
	return true;
}

/**
 * 次のStatementに進める
 */
bool TokenStream::getNextStatement(){
	std::string str;
	for(int i = CurIndex; i < Tokens.size(); i++){
		str = Tokens[CurIndex]->getTokenString();
		if(str == ";" || str == "{" || str == "}")
			break;
		CurIndex++;
	}
	return true;
}

/**
 * 次のFunctionに進める
 */
bool TokenStream::getNextFunction(){
	for(int i = CurIndex; i < Tokens.size()-1; i++){
		if(Tokens[CurIndex]->getTokenString() == "]]")
			break;
		CurIndex++;
	}
	CurIndex++;
	return true;
}
