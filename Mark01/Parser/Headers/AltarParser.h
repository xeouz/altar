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
    ASTreeType* PreviousAST;
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
NodeArrayType* ParserParseClosure(ParserType* parser);

// ASTrees
ASTreeType* ParserParseRoot(ParserType* parser);
ASTreeType* ParserParseStatement(ParserType* parser);
ASTreeType* ParserParseStatements(ParserType* parser);
ASTreeType* ParserParseExpression(ParserType* parser);
ASTreeType* ParserParseFactor(ParserType* parser);
ASTreeType* ParserParseTerm(ParserType* parser, TokenType* term);
ASTreeType* ParserParseBlock(ParserType* parser);

ASTreeType* ParserParseReturn(ParserType* parser);
ASTreeType* ParserParseFor(ParserType* parser);
ASTreeType* ParserParseWhile(ParserType* parser);

ASTreeType* ParserParseIdentifier(ParserType* parser);
ASTreeType* ParserParseVariable(ParserType* parser);
ASTreeType* ParserParseVariableDeclaration(ParserType* parser);
ASTreeType* ParserParseMultiVariableDeclaration(ParserType* parser);
ASTreeType* ParserParseVariableDeclarationHelper(ParserType* parser, ASTreeType* AST);
ASTreeType* ParserParseVariableAssignment(ParserType* parser);
ASTreeType* ParserParseInteger(ParserType* parser);
ASTreeType* ParserParseFloat(ParserType* parser);
ASTreeType* ParserParseString(ParserType* parser);
ASTreeType* ParserParseCharacter(ParserType* parser);
ASTreeType* ParserParseBool(ParserType* parser);

ASTreeType* ParserParseIf(ParserType* parser);
ASTreeType* ParserParseFunction(ParserType* parser);
ASTreeType* ParserParseFunctionCall(ParserType* parser);
ASTreeType* ParserParseClass(ParserType* parser);

ASTreeType* ParserParseArithmetic(ParserType* parser);

// Destruction
void DestroyParser(ParserType* Parser);

#endif