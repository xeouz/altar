#ifndef PARSERC
#define PARSERC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/Integers.h"
#include "Headers/AltarParser.h"
#include "Headers/AltarTokens.h"

#include "AltarASTree.c"
#include "AltarLexer.c"
#include "NodeArray.c"

// -- Initialization --
ParserType* InitParser(LexerType* lexer)
{
    ParserType* parser = calloc(1, sizeof(struct ParserStructure));
    parser->lexer = lexer;
    parser->CurrentToken = LexerAdvanceToken(parser->lexer);
    parser->PreviousToken = parser->CurrentToken;
    parser->VariableScope=(char**)calloc(1,sizeof(char*));
    parser->VariableScopeSize=0;
    return parser;
}

// -- Errors --
void ParserEOFError()
{
    printf("[Parser] - End of File\n");
}

void ParserStatementError(ParserType* parser)
{
    printf("[Parser] ParserStatementError - Invalid Token: %s\n%s\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
}

void ParserUnexpectedTokenError(ParserType* parser)
{
    printf("[Parser] ParserUnexpectedTokenError - Unexpected Token: `%s` - %s\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
}

void PareserDifferentTokenError(ParserType* parser, USInt type)
{
    printf("[Parser] ParserDifferentTokenError - Token: `%s` - %s, Expected Token: `%s` - %d\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken),TokenTypeToStr(type),type);
}


void ParserExpressionError(ParserType* parser)
{
    printf("[Parser] ParserExpressionError - Invalid Token: %s\n%s\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
}
// -- Voids -- 

void ParserAdvanceToken(ParserType* parser, USInt type)
{
    if(parser->CurrentToken->type==TOKEN_ENDFL)
    {
        ParserEOFError();
    }
    if (parser->CurrentToken->type == type)
    {
        parser->PreviousToken = parser->CurrentToken;
        parser->CurrentToken = LexerAdvanceToken(parser->lexer);
    }
    else
    {
        PareserDifferentTokenError(parser, type);
    }
}

void ParserPrintVar(ParserType* parser, ASTreeType* variableValue)
{
    if(variableValue->type==AST_INTEGER)
    {
        printf("[Parser] - Value: %d\n",variableValue->value.integer_value);
    }
    else if(variableValue->type==AST_STRING)
    {
        printf("[Parser] - Value: %s\n",variableValue->value.string_value);
    }
    else if(variableValue->type==AST_CHARACTER)
    {
        printf("[Parser] - Value: %c\n",variableValue->value.char_value);
    }
    else if(variableValue->type==AST_FLOAT)
    {
        printf("[Parser] - Value: %f\n",variableValue->value.float_value);
    }
}

// -- NodeArrays --
NodeArrayType* ParserParseParenthesis(ParserType* parser)
{
    NodeArrayType* parenthesis=InitNodeArray(sizeof(struct ASTreeStructure));
    ParserAdvanceToken(parser,TOKEN_LPAREN);

    ASTreeType* statement=ParserParseExpression(parser);

    AppendNodeArray(parenthesis,statement);

    while(parser->CurrentToken->type==TOKEN_COMMA)
    {
        ParserAdvanceToken(parser,TOKEN_COMMA);

        statement=ParserParseExpression(parser);
        AppendNodeArray(parenthesis,statement);
    }

    ParserAdvanceToken(parser,TOKEN_RPAREN);

    return parenthesis;
}

// -- ASTrees --

// Root Program In
ASTreeType* ParserParseRoot(ParserType* parser)
{
    return ParserParseStatements(parser);
}

// Parse a Statement
ASTreeType* ParserParseStatement(ParserType* parser)
{
    switch(parser->CurrentToken->type)
    {
        case TOKEN_ID: return ParserParseIdentifier(parser);

        case TOKEN_ENDFL: ParserEOFError(); return NULL;

        default: ParserStatementError(parser); exit(1);
    }
}

// Parse multiple statements
ASTreeType* ParserParseStatements(ParserType* parser)
{
    // Initialize the ASTree
    ASTreeType* AST=InitASTree(AST_ROOT);

    // Initialize the node array
    AST->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));

    // Parse a Statement

    ASTreeType* statement = ParserParseStatement(parser);

    // Add the statement to the node array
    AppendNodeArray(AST->RootValue, statement);

    // Parse more statements
    while(parser->CurrentToken->type == TOKEN_SEMICOL)
    {
        // Advance the token SemiColon
        ParserAdvanceToken(parser, TOKEN_SEMICOL);

        // Parse a Statement
        ASTreeType* statement = ParserParseStatement(parser);

        if(!statement)
        {
            break;
        }

        // Add the statement to the node array
        AppendNodeArray(AST->RootValue, statement);
    }

    // Return the ASTree
    return AST;
}

// Parse an Identifier
ASTreeType* ParserParseIdentifier(ParserType* parser)
{   
    // If the current token is "var"
    if(strcmp(parser->CurrentToken->value,"var")==0)
    {
        return ParserParseVariableDeclaration(parser);
    }
    
    /*
    // If the current token is "if"
    else if(strcmp(parser->CurrentToken->value,"if")==0)
    {
        return ParserParseIf(parser);
    }
    
    // If the current token is "while"
    else if(strcmp(parser->CurrentToken->value,"while")==0)
    {
        return ParserParseWhile(parser);
    }

    // If the current token is "for"
    else if(strcmp(parser->CurrentToken->value,"for")==0)
    {
        return ParserParseFor(parser);
    }

    // If the current token is "return"
    else if(strcmp(parser->CurrentToken->value,"return")==0)
    {
        return ParserParseReturn(parser);
    }

    // If the current token is "func"
    else if(strcmp(parser->CurrentToken->value,"func")==0)
    {
        return ParserParseFunction(parser);
    }
    */
    // else
    else
    {
        return ParserParseVariable(parser);
    }
}

// Parse a variable declaration
ASTreeType* ParserParseVariableDeclaration(ParserType* parser)
{
    // Advance the keyword "var"
    ParserAdvanceToken(parser,TOKEN_ID);

    // Save the variable name
    char* variableName = parser->CurrentToken->value;

    // Advance the variable name
    ParserAdvanceToken(parser,TOKEN_ID);

    if(parser->CurrentToken->type==TOKEN_COLON)
    {
        ParserAdvanceToken(parser,TOKEN_COLON);

        ASTreeType* variable=InitASTree(AST_VARIABLE);
        variable->name.variable_name=variableName;
        variable->val_type.variable_def_value_type=parser->CurrentToken->value;

        ParserAdvanceToken(parser,TOKEN_ID);

        if(parser->CurrentToken->type==TOKEN_EQUALS)
        {  
            ParserAdvanceToken(parser,TOKEN_EQUALS);
            return ParserParseVariableDeclarationHelper(parser,variable);
        }
    }
    else if(parser->CurrentToken->type==TOKEN_EQUALS)
    {
        ParserAdvanceToken(parser,TOKEN_EQUALS);

        ASTreeType* variable=InitASTree(AST_VARIABLE_DECLARATION);
        variable->name.variable_name=variableName;

        return ParserParseVariableDeclarationHelper(parser,variable);
    }
    else
    {
        ParserUnexpectedTokenError(parser);
        exit(1);
    }

    return NULL;
}

ASTreeType* ParserParseVariableDeclarationHelper(ParserType* parser, ASTreeType* variable)
{
    printf("[Parser] - Variable Declaration: %s\n",variable->name.variable_def_name);
    // Parse the variable value
    ASTreeType* variableValue=ParserParseExpression(parser);

    // Set the variable name and value
    variable->tree_child=variableValue;

    // Printing Variable and Value
    // ParserPrintVar(parser,variableValue);

    // Return the variable definition ASTree
    return variable;
}
// Parse an Expression
ASTreeType* ParserParseExpression(ParserType* parser)
{   
    // Switch on the current token type
    switch(parser->CurrentToken->type)
    {
        case TOKEN_ID: return ParserParseIdentifier(parser);
        case TOKEN_STR: return ParserParseString(parser);
        case TOKEN_INTEGER: return ParserParseInteger(parser);
        case TOKEN_FLOAT: return ParserParseFloat(parser);
        case TOKEN_CHAR: return ParserParseCharacter(parser);
        case TOKEN_BOOL: return ParserParseBool(parser);

        // Raise an error if the current token is not an expression
        default: ParserExpressionError(parser);
    }

    return NULL;
}


ASTreeType* ParserParseAssignment(ParserType* parser)
{
    ASTreeType* assignment=InitASTree(AST_VARIABLE_ASSIGNMENT);

    ParserAdvanceToken(parser,TOKEN_EQUALS);

    assignment->tree_child=ParserParseExpression(parser);

    return assignment;
}

/*

ASTreeType* ParserParseFactor(ParserType* parser)
{

}

ASTreeType* ParserParseTerm(ParserType* parser)
{

}
*/

// Parse a variable
ASTreeType* ParserParseVariable(ParserType* parser)
{
    // Save the variable name
    char* variableName = parser->CurrentToken->value;

    // Advance the variable name
    ParserAdvanceToken(parser,TOKEN_ID);

    // If the current token is a left parenthesis
    if(parser->CurrentToken->type==TOKEN_LPAREN)
    {
        // We assume that the it is a function call
        return ParserParseFunctionCall(parser);
    }
    else if(parser->CurrentToken->type==TOKEN_COLON)
    {
        ParserAdvanceToken(parser,TOKEN_COLON);

        ASTreeType* variable=InitASTree(AST_VARIABLE);
        variable->name.variable_name=variableName;
        variable->val_type.variable_def_value_type=parser->CurrentToken->value;

        ParserAdvanceToken(parser,TOKEN_ID);

        if(parser->CurrentToken->type==TOKEN_EQUALS)
        {  
            ParserAdvanceToken(parser,TOKEN_EQUALS);
            return ParserParseVariableDeclarationHelper(parser,variable);
        }
    }
    else if(parser->CurrentToken->type==TOKEN_EQUALS)
    {
        return ParserParseAssignment(parser);
    }
    else if(parser->CurrentToken->type==TOKEN_LPAREN)
    {
        return ParserParseFunctionCall(parser);
    }

    // Create the variable ASTree and set the name
    ASTreeType* variable=InitASTree(AST_VARIABLE);
    variable->name.variable_name=variableName;

    // Return the variable ASTree
    return variable;
}


// Parse an integer
ASTreeType* ParserParseInteger(ParserType* parser)
{
    // Create the integer ASTree and set the value
    ASTreeType* AST=InitASTree(AST_INTEGER);
    AST->value.integer_value=atoi(parser->CurrentToken->value);
    // Advance the integer
    ParserAdvanceToken(parser,TOKEN_INTEGER);

    // Return the integer ASTree
    return AST;
}

// Parse a float
ASTreeType* ParserParseFloat(ParserType* parser)
{   
    // Create the float ASTree and set the value
    ASTreeType* AST=InitASTree(AST_FLOAT);
    AST->value.float_value=atof(parser->CurrentToken->value);

    // Advance the float
    ParserAdvanceToken(parser,TOKEN_FLOAT);

    // Return the float ASTree
    return AST;
}

// Parse a string
ASTreeType* ParserParseString(ParserType* parser)
{
    // Create the string ASTree and set the value
    ASTreeType* AST=InitASTree(AST_STRING);
    AST->value.string_value=parser->CurrentToken->value;

    // Advance the string
    ParserAdvanceToken(parser,TOKEN_STR);

    // Return the string ASTree
    return AST;
}

// Parse a character
ASTreeType* ParserParseCharacter(ParserType* parser)
{
    // Create the character ASTree and set the value
    ASTreeType* AST=InitASTree(AST_CHARACTER);
    AST->value.char_value=parser->CurrentToken->value[0];

    // Advance the character
    ParserAdvanceToken(parser,TOKEN_CHAR);

    // Return the character ASTree
    return AST;
}

// Parse a boolean
ASTreeType* ParserParseBool(ParserType* parser)
{
    // Create the boolean ASTree and set the value
    ASTreeType* AST=InitASTree(AST_BOOL);
    AST->value.bool_value=atoi(parser->CurrentToken->value);

    // Advance the boolean
    ParserAdvanceToken(parser,TOKEN_BOOL);

    // Return the boolean ASTree
    return AST;
}


/*
ASTreeType* ParserParseFunction(ParserType* parser)
{

}
*/

// Parse a function call
ASTreeType* ParserParseFunctionCall(ParserType* parser)
{
    char* variableName=parser->PreviousToken->value;

    printf("[Parser] - Function Call: %s\n",variableName);

    // Create the function call ASTree
    ASTreeType* functionCall=InitASTree(AST_FUNCTION_CALL);

    functionCall->args.function_call_arguments=ParserParseParenthesis(parser);

    return functionCall;
}

// -- Destruction --

// Destroy a parser
void DestroyParser(ParserType* parser)
{   
    // Destroy the token
    DestroyToken(parser->CurrentToken);
    
    // Destroy the Lexer
    DestroyLexer(parser->lexer);
    
    // Destroy the parser
    free(parser);
}

#endif