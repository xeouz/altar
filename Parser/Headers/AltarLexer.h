#ifndef LEXERS
#define LEXERS

#include "AltarTokens.h"
#include "Integers.h"

typedef struct LexerStructure{
	char cursor;
	char* src;
	char* filename;
	Int indx;
	Int size;
	Int line;
}LexerType;

// Initilization
LexerType* InitLexer(char *src, char* filename);

// Voids
void LexerAdvanceChar(LexerType* lexer);
void LexerSkipWS(LexerType* lexer);
void LexerSkipComments(LexerType* lexer);
void LexerRaiseError(LexerType* lexer);

// TokenTypes
TokenType* LexerAdvanceToken(LexerType* lexer);
TokenType* LexerGatherStrChar(LexerType* lexer);
TokenType* LexerGatherNumber(LexerType* lexer);
TokenType* LexerGatherFloat(LexerType* lexer);
TokenType* LexerGatherConditional(LexerType* lexer, char* target);
TokenType* LexerGatherIdentifier(LexerType* lexer);
TokenType* LexerAdvanceInplace(LexerType* lexer, USInt tokentype);
TokenType* LexerAdvanceWithToken(LexerType* lexer, TokenType* token);

// char 
char LexerPeek(LexerType* lexer,char peek);

// Strings
char* LexerCursorStr(LexerType* lexer);

// Destruction
void DestroyLexer(LexerType* lexer);

#endif