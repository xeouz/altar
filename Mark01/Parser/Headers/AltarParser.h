#ifndef PARSERS
#define PARSERS

#include "AltarLexer.h"
#include "AltarTokens.h"
#include "AltarASTree.h"

#include "Integers.h"

typedef struct ParserStructure
{
    LexerType* lexer;
    TokenType* CurrentToken;
    TokenType* PreviousToken;
    char** VariableScope;
    Int VariableScopeSize;
}ParserType;

// Initialization
ParserType* InitParser(LexerType* lexer);

// Voids
void ParserAdvanceToken(ParserType* parser, USInt TokenType);
void ParserPrintVar(ParserType* parser, ASTreeType* variableValue);

// Errors

void ParserEOFError();
void ParserStatementError(ParserType* parser);

// NodeArrays
NodeArrayType* ParserParseParenthesis(ParserType* parser);

// ASTrees
ASTreeType* ParserParseRoot(ParserType* parser);
ASTreeType* ParserParseStatement(ParserType* parser);
ASTreeType* ParserParseStatements(ParserType* parser);
ASTreeType* ParserParseExpression(ParserType* parser);
ASTreeType* ParserParseFactor(ParserType* parser);
ASTreeType* ParserParseTerm(ParserType* parser);

ASTreeType* ParserParseIdentifier(ParserType* parser);
ASTreeType* ParserParseVariable(ParserType* parser);
ASTreeType* ParserParseVariableDeclaration(ParserType* parser);
ASTreeType* ParserParseVariableDeclarationHelper(ParserType* parser, ASTreeType* AST);
ASTreeType* ParserParseVariableAssignment(ParserType* parser);
ASTreeType* ParserParseInteger(ParserType* parser);
ASTreeType* ParserParseFloat(ParserType* parser);
ASTreeType* ParserParseString(ParserType* parser);
ASTreeType* ParserParseCharacter(ParserType* parser);
ASTreeType* ParserParseBool(ParserType* parser);
ASTreeType* ParserParseFunction(ParserType* parser);
ASTreeType* ParserParseFunctionCall(ParserType* parser);

// Destruction
void DestroyParser(ParserType* Parser);

#endif