#ifndef TOKENH
#define TOKENH

#include "Integers.h"

typedef struct TokenStructure{
	enum{
		TOKEN_ID, // Any Word
		
		TOKEN_EQUALS, // =
		TOKEN_CHKEQUALS, // ==

		TOKEN_LPAREN, // (
		TOKEN_LBRACK, // [
		TOKEN_LBRACE, // {
		
		TOKEN_RPAREN, // )
		TOKEN_RBRACK, // ]
		TOKEN_RBRACE, // }

		TOKEN_COLON, // :
		TOKEN_SEMICOL, // ;
		TOKEN_COMMA, // ,
		
		TOKEN_STR, // abc
		TOKEN_CHAR, // a
		TOKEN_BOOL, // true or false

		TOKEN_INTEGER, // 123
		TOKEN_FLOAT, // 123.123

		TOKEN_BSLSH,	// Backslash

		TOKEN_RANGB, // >
		TOKEN_LANGB, // <

		TOKEN_ADD, // +
		TOKEN_SUB, // -
		TOKEN_MUL, // *
		TOKEN_DIV, // /
		TOKEN_FLR, // //
		TOKEN_MOD, // %

		TOKEN_INCR, // ++
		TOKEN_DECR, // --
		TOKEN_EXP, // **

		TOKEN_ADDEQ, // +=
		TOKEN_SUBEQ, // -=
		TOKEN_MULEQ, // *=
		TOKEN_DIVEQ, // /=
		TOKEN_FLREQ, // //=
		TOKEN_MODEQ, // %=
		
		TOKEN_ANDOP, // and, &&
		TOKEN_OROP, // or, ||
		TOKEN_NOTOP, // not, !
		
		TOKEN_LBSHIFT, // <<
		TOKEN_RBSHIFT, // >>
		TOKEN_CARET, // ^
		TOKEN_HASHT, // #
		TOKEN_ATSYM, // @
		TOKEN_DOT, // .
		TOKEN_RARROW, // ->

		TOKEN_COMMNT, // /-
		TOKEN_LSCMMNT, // /--
		TOKEN_LECMMNT, // --/

		TOKEN_CONDIT, // ?truthy:falsy, ?

		TOKEN_ENDFL
	}type;
	char* value;
}TokenType;

// Initilization
TokenType* InitToken(USInt type, char* value);

// Char Arrays
char* TokenToStr(TokenType* token);
char* TokenToSym(TokenType* token);

// Destruction
void DestroyToken(TokenType* token);

#endif