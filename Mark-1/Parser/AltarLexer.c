#ifndef LEXERC
#define LEXERC

// Include Standard C headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Include the headers
#include "Headers/Integers.h"
#include "Headers/AltarLexer.h"
#include "Headers/AltarTokens.h"

// Include the Tokens
#include "AltarTokens.c"

// --- Init ---

// Inititalize the Lexer
LexerType* InitLexer(char *src)
{
	// Allocate Memory for the Lexer
	LexerType* lexer=calloc(1,sizeof(struct LexerStructure));

	// Set the source
	lexer->src=src;

	// Set the current position
	lexer->indx=0;

	// Set the current Character
	lexer->cursor=lexer->src[lexer->indx];

	// Set the Size
	lexer->size=strlen(src);

	// Return the Lexer
	return lexer;
}

// --- Voids ---

// Advance the Lexer to the next Character
void LexerAdvanceChar(LexerType* lexer)
{
	// If the current index is less than the size and the current character is not a null character
	if(lexer->cursor!='\0' && (lexer->indx)<(lexer->size))
	{
		// Increment the index
		++lexer->indx;

		// Set the current character
		lexer->cursor=lexer->src[lexer->indx];
	}
}

// Skipping Whitespace
void LexerSkipWS(LexerType* lexer)
{
	// While the current character is a whitespace character
	while(lexer->cursor==' '||lexer->cursor=='\n'||lexer->cursor=='\t')
	{
		// Advance the character
		LexerAdvanceChar(lexer);
	}
}

// Skipping Comments
void LexerSkipComments(LexerType* lexer)
{
	// If the current character is a '/'
	if(lexer->cursor=='/'&&LexerPeek(lexer,1)=='-')
	{ 
		// If the current character is a '/' and the next character is a '-'
		if(LexerPeek(lexer,2)=='-')
		{
			int i=0;

			// Advance those characters
			for(i=0;i<3;++i) LexerAdvanceChar(lexer);

			// Skip the rest of the comments
			while(lexer->cursor!='-'&&LexerPeek(lexer,1)!='-'&&LexerPeek(lexer,2)!='/')
			{
				LexerAdvanceChar(lexer);
			}
			
			// Advance the last characters
			for(i=0;i<3;++i) LexerAdvanceChar(lexer);
		}

		// If the current character is a '/' but the next character is not a '-'
		else
		{
			// Advance the character
			LexerAdvanceChar(lexer);

			// Skip the rest of the comments
			while(lexer->cursor!='\n')
			{
				LexerAdvanceChar(lexer);
			}
			
			// Advance the last character
			LexerAdvanceChar(lexer);
		}
	}
}

// Raising an Error
void LexerRaiseError(LexerType* lexer)
{
	// Print the error
	printf("[Lexer]: Invalid Character %c, Character ASCII - %d\n",lexer->cursor,lexer->cursor);

	// Exit the program
	exit(1);
}

// --- Chars --- 

// Peeking n+current_indx th character
char LexerPeek(LexerType* lexer, char peek)
{
	return lexer->src[lexer->indx+peek];
}

// --- String ---

// Get the string of the current character
char* LexerCursorStr(LexerType* lexer)
{
	// Return the string
	return (char[]){lexer->cursor,'\0'};
}


// --- Tokens ---

// Get the Next Token Type
TokenType* LexerAdvanceToken(LexerType* lexer)
{
	// While the current character is not a null character and the current index is less than the size 
	while(lexer->cursor!='\0' && lexer->cursor!=EOF && lexer->indx < lexer->size){
		// Skip Whitespace
		LexerSkipWS(lexer);

		// Skip Comments
		LexerSkipComments(lexer);

		// Peeking the next character
		char peek=LexerPeek(lexer,1);

		// If the current character is an alphabet
		if(isalpha(lexer->cursor))
		{
			// Return the Identifier Token
			return LexerGatherIdentifier(lexer);
		}

		// If the current character is a number
		else if(isdigit(lexer->cursor))
		{
			// Return the Number Token
			return LexerGatherNumber(lexer);
		}

		// Switch on the current character
		switch(lexer->cursor)
		{
			// I don't have time to write these comments, infer yourself
			case '+':
			{
				if(peek=='+')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_INCR,"++"));
				}
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer, InitToken(TOKEN_ADDEQ,"+="));
				}
				return LexerAdvanceInplace(lexer,TOKEN_ADD);
			}
			
			case '-':
			{
				if(peek=='-')
				{
					LexerAdvanceChar(lexer);
					if(LexerPeek(lexer,1)=='/')
					{
						LexerAdvanceChar(lexer);
						return LexerAdvanceWithToken(lexer,InitToken(TOKEN_LECMMNT,"--/"));
					}
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_DECR,"--"));
				}
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer, InitToken(TOKEN_SUBEQ,"-="));
				}
				return LexerAdvanceInplace(lexer,TOKEN_SUB);
			}

			case '*':
			{
				if(peek=='*')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_EXP,"**"));
				}
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer, InitToken(TOKEN_MULEQ,"*="));
				}
				return LexerAdvanceInplace(lexer,TOKEN_MUL);
			}

			case '/':
			{
				if(peek=='/')
				{
					LexerAdvanceChar(lexer);
					if(LexerPeek(lexer,1)=='=')
						return LexerAdvanceWithToken(lexer,InitToken(TOKEN_FLREQ,"//="));
					else
						return LexerAdvanceWithToken(lexer,InitToken(TOKEN_FLR,"//"));
				}
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer, InitToken(TOKEN_DIVEQ,"/="));
				}
				if(peek=='-')
				{	
					LexerAdvanceChar(lexer);
					if(LexerPeek(lexer,1)=='-')
					{
						LexerAdvanceChar(lexer);
						return LexerAdvanceWithToken(lexer,InitToken(TOKEN_LSCMMNT,"/--"));
					}
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_COMMNT,"/-"));
				}
				return LexerAdvanceInplace(lexer,TOKEN_DIV);
			}

			case '%':
			{
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer, InitToken(TOKEN_MODEQ,"+="));
				}
				return LexerAdvanceInplace(lexer,TOKEN_MOD);
			}

			// Conditionals
			case '|':
			{
				if(peek=='|')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_OROP,"||"));
				}
			}

			case '&':
			{	
				if(peek=='&')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_ANDOP,"&&"));
				}
			}

			case '<':
			{
				if(peek=='<')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_LBSHIFT,"<<"));
				}
				return LexerAdvanceInplace(lexer,TOKEN_LANGB);
			}

			case '>':
			{
				if(peek=='>')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_RBSHIFT,">>"));
				}
				return LexerAdvanceInplace(lexer,TOKEN_RANGB);
			}

			case '=':
			{
				if(peek=='=')
				{
					LexerAdvanceChar(lexer);
					return LexerAdvanceWithToken(lexer,InitToken(TOKEN_CHKEQUALS,"=="));
				}
				return LexerAdvanceInplace(lexer,TOKEN_EQUALS);
			}

			case '!': return LexerAdvanceInplace(lexer,TOKEN_NOTOP);

			case '(': return LexerAdvanceInplace(lexer,TOKEN_LPAREN);
			case '[': return LexerAdvanceInplace(lexer,TOKEN_LBRACK);
			case '{': return LexerAdvanceInplace(lexer,TOKEN_LBRACE);
			case ')': return LexerAdvanceInplace(lexer,TOKEN_RPAREN);
			case ']': return LexerAdvanceInplace(lexer,TOKEN_RBRACK);
			case '}': return LexerAdvanceInplace(lexer,TOKEN_RBRACE);

			case ':': return LexerAdvanceInplace(lexer,TOKEN_COLON);
			case ';': return LexerAdvanceInplace(lexer,TOKEN_SEMICOL);
			case ',': return LexerAdvanceInplace(lexer,TOKEN_COMMA);

			case '#': return LexerAdvanceInplace(lexer,TOKEN_HASHT);
			case '^': return LexerAdvanceInplace(lexer,TOKEN_CARET);
			case '@': return LexerAdvanceInplace(lexer,TOKEN_ATSYM);

			case '"': return LexerGatherStrChar(lexer);
			case '\'':return LexerGatherStrChar(lexer);

			case '\0': return InitToken(TOKEN_ENDFL,"");

			default: LexerRaiseError(lexer);
		}
	}
	
	// If we get here, we've reached the end of the file
	return InitToken(TOKEN_ENDFL,0);
}

// Gathering a string
TokenType* LexerGatherStrChar(LexerType* lexer)
{
	// Allocating Memory for the string
	char* value=calloc(1,1);

	// If the current character is a quote then we need to look for the closing quote
	// If the current character is a double quote then we need to look for the closing double quote
	
	char end=lexer->cursor;

	// Advancing the Quote 
	LexerAdvanceChar(lexer);
	
	// If the current character is a quote or a double quote and the next character is a quote or a double quote
	// then it is a Character
	if(LexerPeek(lexer,1)==end)
	{	
		// Initializing the token
		TokenType* token=InitToken(TOKEN_CHAR,LexerCursorStr(lexer));

		// Advancing the Character
		LexerAdvanceChar(lexer); 

		// Advancing the Quote
		LexerAdvanceChar(lexer);

		// Returning the token
		return token;
	}
	else
	{
		// Gathering the string
		while(lexer->cursor!=end&&lexer->cursor!=0)
		{	
			// Reallocating the string
			value=realloc(value,strlen(value)+2);

			// Adding the character to the string
			strcat(value,LexerCursorStr(lexer));

			// Advancing the character
			LexerAdvanceChar(lexer);
		}

		// Advancing the Quote
		LexerAdvanceChar(lexer);

		// Initializing the token
		TokenType* token=InitToken(TOKEN_STR,value);

		// Returning the token
		return token;
	}
}


// Gathering a Conditional, Bool?Truthy:Falsy
TokenType* LexerGatherConditional(LexerType* lexer, char* value)
{
	// Adding a separator to the string
	strcat(value,"@");
	
	// While the current character is not a colon
	while(lexer->cursor!=':')
	{
		// Reallocating the string
		value=realloc(value,strlen(value)+2);

		// Adding the character to the string
		strcat(value,LexerCursorStr(lexer));

		// Advancing the character
		LexerAdvanceChar(lexer);
	}
	
	// Advancing the colon
	LexerAdvanceChar(lexer);

	// Adding a separator to the string
	strcat(value,"@");

	// While the current character is alpha numeric
	while(isalnum(lexer->cursor))
	{
		// Reallocating the string
		value=realloc(value,strlen(value)+2);

		// Adding the character to the string
		strcat(value,LexerCursorStr(lexer));

		// Advancing the character
		LexerAdvanceChar(lexer);
	}

	// Initializing the token
	return InitToken(TOKEN_CONDIT,value);
}

// Gathering an Identifier
TokenType* LexerGatherIdentifier(LexerType* lexer)
{
	// Allocating Memory for the string
	char* value=calloc(1,1);

	// While the current character is alpha numeric
	while(isalnum(lexer->cursor))
	{
		// Reallocating the string
		value=realloc(value,strlen(value)+2);

		// Adding the character to the string
		strcat(value,LexerCursorStr(lexer));

		// Advancing the character
		LexerAdvanceChar(lexer);
	}


	// If the value is "and" 
	if(strcmp(value,"and")==0)
	{
		// Return an "and" operator token
		return InitToken(TOKEN_ANDOP,"&&");
	}

	// If the value is "or"
	else if(strcmp(value,"or")==0)
	{
		// Return an "or" operator token
		return InitToken(TOKEN_OROP,"||");
	}
	
	// If the value is "not"
	else if(strcmp(value,"not")==0)
	{
		// Return an "not" operator token
		return InitToken(TOKEN_NOTOP,"!");
	}

	// If the value is "true"
	else if(strcmp(value,"true")==0)
	{
		// Return a 1 Integer token
		return InitToken(TOKEN_BOOL,"1");
	}

	// If the value is "false"
	else if(strcmp(value,"false")==0)
	{
		// Return a 0 Integer token
		return InitToken(TOKEN_BOOL,"0");
	}

	// If the value is a question mark
	else if(lexer->cursor=='?')
	{
		// Return a conditional token
		return LexerGatherConditional(lexer,value);
	}

	// Return an identifier token
	return InitToken(TOKEN_ID,value);
}

// Gathering a Number
TokenType* LexerGatherNumber(LexerType* lexer)
{
	// Allocating Memory for the string
	char* value=calloc(1,1);

	char isfloat='n';

	// While the current character is a digit
	while(isdigit(lexer->cursor)||lexer->cursor=='.')
	{
		// If the current character is a decimal point then It is a float
		if(lexer->cursor=='.') isfloat='y';

		// Reallocating the string
		value=realloc(value,strlen(value)+2);

		// Adding the character to the string
		strcat(value,LexerCursorStr(lexer));

		// Advancing the character
		LexerAdvanceChar(lexer);
	}

	// Return a number token or a float token
	return InitToken(isfloat=='n'?TOKEN_INTEGER:TOKEN_FLOAT,value);
}

// Advancing Inplace
TokenType* LexerAdvanceInplace(LexerType* lexer, USInt tokentype)
{
	// Creating a token 
	TokenType* token=InitToken(tokentype, LexerCursorStr(lexer));

	// Advancing the cursor
	LexerAdvanceChar(lexer);

	// Returning the token
	return token;
}

// Advancing with a Token
TokenType* LexerAdvanceWithToken(LexerType* lexer,TokenType* token)
{
	// Advancing the cursor
	LexerAdvanceChar(lexer);

	// Returning the token
	return token;
}

// -- Destruction --

// Destroying a Lexer
void DestroyLexer(LexerType* lexer)
{	
	// Freeing the lexer
	free(lexer);
}

#endif