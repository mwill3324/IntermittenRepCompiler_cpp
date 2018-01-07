
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

// ------- token types -------------------

typedef enum { END_OF_FILE = 0, //0
    VAR, IF, WHILE, FOR, SWITCH, //5
    CASE, DEFAULT, PRINT, ARRAY, //9
    PLUS, MINUS, DIV, MULT, //13
    EQUAL, COLON, COMMA, SEMICOLON,//17
    LBRAC, RBRAC, LPAREN, RPAREN,//21
    LBRACE, RBRACE,NOTEQUAL, GREATER,//25
    LESS, NUM, ID, ERROR//29
} TokenType;

class Token {
  public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
  public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

  private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

#endif  //__LEXER__H__
