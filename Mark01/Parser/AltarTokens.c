#ifndef TOKENC
#define TOKENC

#include <stdlib.h>

#include "Headers/AltarTokens.h"
#include "Headers/Integers.h"

// -- Initialization --
TokenType* InitToken(USInt type, char* value)
{
	TokenType* token=calloc(1,sizeof(struct TokenStructure));
	token->type=type;
	token->value=value;

	return token;
}

// -- Char Arrays --
char* TokenToStr(TokenType* token)
{
	switch(token->type)
	{	
		case TOKEN_ID: return "ID";

		case TOKEN_EQUALS: return "EQUALS";
		case TOKEN_CHKEQUALS: return "CHKEQUALS";

		case TOKEN_LPAREN: return "LPAREN";
		case TOKEN_LBRACK: return "LBRACK";
		case TOKEN_LBRACE: return "LBRACE";
		case TOKEN_RPAREN: return "RPAREN";
		case TOKEN_RBRACK: return "RBRACK";
		case TOKEN_RBRACE: return "RBRACE";

		case TOKEN_STR: return "STR";
		case TOKEN_CHAR: return "CHAR";
		case TOKEN_INTEGER: return "INTEGER";
		case TOKEN_FLOAT: return "FLOAT";

		case TOKEN_COLON: return "COLON";
		case TOKEN_SEMICOL: return "SEMICOL";

		case TOKEN_ADD: return "ADD";
		case TOKEN_SUB: return "SUB";
		case TOKEN_MUL: return "MUL";
		case TOKEN_DIV: return "DIV";
		case TOKEN_MOD: return "MOD";
		case TOKEN_FLR: return "FLR";

		case TOKEN_ADDEQ: return "ADDEQ";
		case TOKEN_SUBEQ: return "SUBEQ";
		case TOKEN_MULEQ: return "MULEQ";
		case TOKEN_DIVEQ: return "DIVEQ";
		case TOKEN_MODEQ: return "MODEQ";
		case TOKEN_FLREQ: return "FLREQ";

		case TOKEN_INCR: return "INCR";
		case TOKEN_DECR: return "DECR";
		case TOKEN_EXP: return "EXP";

		case TOKEN_BSLSH: return "BSLSH";
		
		case TOKEN_RANGB: return "RANGB";
		case TOKEN_LANGB: return "LANGB";

		case TOKEN_LBSHIFT: return "LBSHIFT";
		case TOKEN_RBSHIFT: return "RBSHIFT";

		case TOKEN_ANDOP: return "ANDOP";
		case TOKEN_NOTOP: return "NOTOP";
		case TOKEN_OROP: return "OROP";

		case TOKEN_CONDIT: return "CONDIT";

		case TOKEN_ENDFL: return "ENDFL";

		case TOKEN_CARET: return "CARET";
		case TOKEN_HASHT: return "HASHT";
		case TOKEN_ATSYM: return "ATSYM";
		case TOKEN_DOT: return "DOT";
		case TOKEN_COMMNT: return "CMMNT";
		case TOKEN_LSCMMNT: return "LSCMMNT";
		case TOKEN_LECMMNT: return "LECMMNT";

		default: return "UNKNOWN";
	}
}

char* TokenToSym(TokenType* token)
{
	switch(token->type)
	{	
		case TOKEN_ID: return "Identifier";
		
		case TOKEN_EQUALS: return "=";
		case TOKEN_CHKEQUALS: return "==";

		case TOKEN_LPAREN: return "(";
		case TOKEN_LBRACK: return "[";
		case TOKEN_LBRACE: return "{";
		case TOKEN_RPAREN: return ")";
		case TOKEN_RBRACK: return "]";
		case TOKEN_RBRACE: return "}";

		case TOKEN_STR: return "String";
		case TOKEN_CHAR: return "Char";
		case TOKEN_INTEGER: return "Integer";
		case TOKEN_FLOAT: return "Float";

		case TOKEN_COLON: return ":";
		case TOKEN_SEMICOL: return ";";

		case TOKEN_ADD: return "+";
		case TOKEN_SUB: return "-";
		case TOKEN_MUL: return "*";
		case TOKEN_DIV: return "/";
		case TOKEN_FLR: return "//";
		case TOKEN_MOD: return "%";

		case TOKEN_ADDEQ: return "+=";
		case TOKEN_SUBEQ: return "-=";
		case TOKEN_MULEQ: return "*=";
		case TOKEN_DIVEQ: return "/=";
		case TOKEN_FLREQ: return "//=";
		case TOKEN_MODEQ: return "%=";

		case TOKEN_BSLSH: return "\\";
		case TOKEN_RANGB: return ">";
		case TOKEN_LANGB: return "<";

		case TOKEN_LBSHIFT: return "<<";
		case TOKEN_RBSHIFT: return ">>";
		
		case TOKEN_INCR: return "++";
		case TOKEN_DECR: return "--";
		case TOKEN_EXP: return "**";
		
		case TOKEN_CARET: return "^";
		case TOKEN_HASHT: return "#";
		case TOKEN_ATSYM: return "@";
		case TOKEN_DOT: return ".";
		case TOKEN_COMMNT: return "/-";
		case TOKEN_LSCMMNT: return "/--";
		case TOKEN_LECMMNT: return "--/";

		case TOKEN_ANDOP: return "&&";
		case TOKEN_OROP: return "||";
		case TOKEN_NOTOP: return "!"; 
		
		case TOKEN_CONDIT: return "?:";

		case TOKEN_ENDFL: return "";

		default: return "UNKNOWN";
	}
}

char* TokenTypeToStr(USInt type)
{
	TokenType* token=InitToken(type,"a");

	char* str=TokenToStr(token);

	DestroyToken(token);

	return str;
}
// -- Destruction --
void DestroyToken(TokenType* token)
{
	free(token->value);
	free(token);
}

#endif