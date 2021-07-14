#ifndef LEXER_HPP
#define LEXER_HPP

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include "APP.hpp"

/**
 * トークン種別
 */
enum TokenType{
	TOK_IDENTIFIER, // 識別子
	TOK_DIGIT,      // 数字
	TOK_STR,        // 文字列
	TOK_SYMBOL,     // 記号
	TOK_NUMBER,     // NUMBER,
	TOK_RETURN,     // RETURN
	TOK_IF,         // IF
	TOK_ELSE_IF,    // ELSE IF
	TOK_ELSE,       // ELSE
	TOK_FOR,        // for
	TOK_BREAK,      // break
	TOK_CONTINUE,   // continue
	TOK_AND,        // and
	TOK_OR,         // or
	TOK_GLOBAL,     // global
	TOK_EOF         // EOF
};

/**
 * 個別トークン格納クラス
 */
class Token{
	private:
		TokenType Type;
		std::string TokenString;
		double Num;     // 値を格納する変数
		std::string Str; // string型の変数の場合に値を格納する変数
		int Line;
	
	public:
		Token(std::string string, TokenType type,int line)
			: TokenString(string),Type(type),Line(line){
			if(type == TOK_DIGIT)
				Num = atof(string.c_str());
			else
				Num = 0x7fffff;
			if(type == TOK_STR)
				Str = string.c_str();
			else
				Str = "0x7ffffff";
		};
		~Token(){};

		// トークンの種別を取得
		TokenType getTokenType(){ return Type; }

		// トークンの文字列表現を取得
		std::string getTokenString() { return TokenString; }

		// トークンの数値を取得
		double getNumValue(){return Num;}

		// トークンの文字列を取得（種別がstringである場合に使用）
		std::string getStrValue(){return Str;}

		// トークンの出現した行数を取得
		int getLine(){ return Line; }
};


/**
 * 切り出したToken格納用クラス
 */
class TokenStream{
	private:
		std::vector<Token*> Tokens;
		int CurIndex;
	
	public:
		TokenStream():CurIndex(0){}
		~TokenStream();

		bool ungetToken(int Times=1);
		bool getNextToken();
		bool getBackToken();
		bool pushToken(Token *token){
			Tokens.push_back(token);
			return true;
		}
		Token getToken();

		// トークンの種類を取得
		TokenType getCurType(){return Tokens[CurIndex]->getTokenType();}
		
		// トークンの文字列表現を取得
		std::string getCurString(){return Tokens[CurIndex]->getTokenString();}

		// トークンの数値を取得（double型）
		double getCurNumVal(){return Tokens[CurIndex]->getNumValue();}

		// トークンんの文字列を取得（string型）
		std::string getCurStrVal(){return Tokens[CurIndex]->getStrValue();}

		// 現在のインデックスを取得
		int getCurIndex(){return CurIndex;}

		// 現在の行を取得
		int getCurLine(){return Tokens[CurIndex]->getLine();}
		
		int getTokensSize(){return Tokens.size();}
		
		bool eraseToken(int i){Tokens.erase(Tokens.begin()+i); return true;}

		Token *getToken(int i){return Tokens.at(i);}

		bool getNextStatement();
		bool getNextFunction();

		// インデックスを指定した値に設定
		bool applyTokenIndex(int index){CurIndex=index;return true;}
		bool printTokens();
};


TokenStream *LexicalAnalysis(std::string input_filename);

#endif
