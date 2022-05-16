#ifndef PARSERC
#define PARSERC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/Integers.h"
#include "Headers/AltarParser.h"
#include "Headers/AltarTokens.h"
#include "Headers/AltarArithmetic.h"

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
    parser->PreviousAST = InitASTree(AST_ENDL);

    return parser;
}

// -- Errors --
void ParserEOFError()
{
    //printf("[Parser] - End of File\n");
}

void ParserStatementError(ParserType* parser)
{
    printf("[Parser] ParserStatementError - Invalid Token: %s %s\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
}

void ParserUnexpectedTokenError(ParserType* parser)
{
    printf("[Parser] ParserUnexpectedTokenError - Unexpected Token: `%s` - %s\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
}

void ParserDifferentTokenError(ParserType* parser, USInt type)
{
    printf("[Parser] ParserDifferentTokenError - Token: `%s` - %s, Expected Token: `%s` - %d\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken),TokenTypeToStr(type),type);
}

void ParserExpressionError(ParserType* parser)
{
    printf("[Parser] ParserExpressionError - Invalid Token: %s %s\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
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
        // DestroyToken(parser->PreviousToken);
        parser->PreviousToken = parser->CurrentToken;
        parser->CurrentToken = LexerAdvanceToken(parser->lexer);
    }
    else
    {
        ParserDifferentTokenError(parser, type);
        exit(1);
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
        printf("[Parser] - Value: %s\n",variableValue->value.float_value);
    }
}

// -- NodeArrays --
NodeArrayType* ParserParseParenthesis(ParserType* parser)
{
    
    NodeArrayType* parenthesis=InitNodeArray(sizeof(struct ASTreeStructure));
    ParserAdvanceToken(parser,TOKEN_LPAREN);

    if(parser->CurrentToken->type==TOKEN_RPAREN)
    {
        ParserAdvanceToken(parser,TOKEN_RPAREN);
        return parenthesis;
    }

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
    ASTreeType* AST;
    switch(parser->CurrentToken->type)
    {
        case TOKEN_ID: AST=ParserParseIdentifier(parser);break;
        case TOKEN_INTEGER: AST=ParserParseInteger(parser);break;

        case TOKEN_INCR: AST=ParserParsePreIncrDecr(parser, 0);break;
        case TOKEN_DECR: AST=ParserParsePreIncrDecr(parser, 1);break;

        case TOKEN_ADD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST=ParserParseArithmetic(parser);break;

        default: printf("Unknown Token Type: %s\n",parser->PreviousToken->value); break;
    }

    parser->PreviousAST=AST;

    ASTreeType* AST2;

    switch (parser->CurrentToken->type)
    {
        default: {
            if(parser->PreviousAST->type==AST_VARIABLE
            || parser->PreviousAST->type==AST_BLOCK_ACCESS
            || parser->PreviousAST->type==AST_FUNCTION_CALL)
            {
                if(parser->CurrentToken->type==TOKEN_DOT)
                {
                    AST2=ParserParseMemberAccess(parser);
                }
                else
                {
                    return AST;
                }
            }
            else
            {
                return AST;
            }
        };
    }

    return AST2;
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
    while(parser->CurrentToken->type == TOKEN_SEMICOL || parser->PreviousToken->type==TOKEN_RBRACE || parser->CurrentToken->type!=TOKEN_ENDFL)
    {
        // Advance the token Semicolon
        if(parser->CurrentToken->type==TOKEN_SEMICOL)
            ParserAdvanceToken(parser,TOKEN_SEMICOL);

        if(parser->CurrentToken->type==TOKEN_ENDFL)
            break;

        // Parse a Statement
        ASTreeType* statement = ParserParseStatement(parser);

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
    // If the current token is "class"
    else if(strcmp(parser->CurrentToken->value,"class")==0)
    {
        return ParserParseClass(parser);
    }

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
    ASTreeType* variable = InitASTree(AST_VARIABLE_DECLARATION);
    variable->name.variable_def_name = calloc(1,strlen(variableName)+1);
    strcpy(variable->name.variable_def_name,variableName);

    // Advance the variable name
    ParserAdvanceToken(parser,TOKEN_ID);

    if(parser->CurrentToken->type==TOKEN_COLON)
    {
        ParserAdvanceToken(parser,TOKEN_COLON);

        variable->val_type.variable_def_value_type=parser->CurrentToken->value;

        ParserAdvanceToken(parser,TOKEN_ID);

        if(parser->CurrentToken->type==TOKEN_EQUALS)
        {  
            ParserAdvanceToken(parser,TOKEN_EQUALS);
            variable = ParserParseVariableDeclarationHelper(parser,variable);
        }
    }
    else if(parser->CurrentToken->type==TOKEN_EQUALS)
    {
        ParserAdvanceToken(parser,TOKEN_EQUALS);

        variable = ParserParseVariableDeclarationHelper(parser,variable);
    }
    else if(parser->CurrentToken->type==TOKEN_SEMICOL || parser->CurrentToken->type==TOKEN_COMMA)
    {
        //printf("[Parser] - Variable Declaration w/o Value: %s\n",variable->name.variable_def_name);
    }
    else
    {
        ParserUnexpectedTokenError(parser);
        exit(1);
    }

    if(parser->CurrentToken->type==TOKEN_COMMA)
    {
        ASTreeType* AST=InitASTree(AST_MULTI_VARIABLE_DECLARATION);

        AST->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));

        AppendNodeArray(AST->RootValue,variable);

        while(parser->CurrentToken->type==TOKEN_COMMA)
        {
            ParserAdvanceToken(parser,TOKEN_COMMA);

            variableName=parser->CurrentToken->value;
            variable = InitASTree(AST_VARIABLE_DECLARATION);
            variable->name.variable_def_name = calloc(1,strlen(variableName)+1);
            strcpy(variable->name.variable_def_name,variableName);

            ParserAdvanceToken(parser,TOKEN_ID);

            if(parser->CurrentToken->type==TOKEN_COLON)
            {
                ParserAdvanceToken(parser,TOKEN_COLON);

                variable->val_type.variable_def_value_type=parser->CurrentToken->value;

                ParserAdvanceToken(parser,TOKEN_ID);

                if(parser->CurrentToken->type==TOKEN_EQUALS)
                {  
                    ParserAdvanceToken(parser,TOKEN_EQUALS);

                    variable=ParserParseVariableDeclarationHelper(parser,variable);

                    AppendNodeArray(AST->RootValue,variable);
                }
            }
            else if(parser->CurrentToken->type==TOKEN_EQUALS)
            {
                ParserAdvanceToken(parser,TOKEN_EQUALS);

                variable=ParserParseVariableDeclarationHelper(parser,variable);

                AppendNodeArray(AST->RootValue,variable);
            }
            else if(parser->CurrentToken->type==TOKEN_SEMICOL || parser->CurrentToken->type==TOKEN_COMMA)
            {
                //printf("[Parser] - Variable Declaration w/o Value: %s\n",variable->name.variable_def_name);

                AppendNodeArray(AST->RootValue,variable);
            }
            else
            {
                ParserUnexpectedTokenError(parser);
                exit(1);
            }
        }
        return AST;
    }

    return variable;
}

ASTreeType* ParserParseVariableDeclarationHelper(ParserType* parser, ASTreeType* variable)
{
    // Parse the variable value
    ASTreeType* variableValue=ParserParseExpression(parser);

    // Set the variable name and value
    variable->tree_child=variableValue;
    //printf("[Parser] - Variable Declaration w/ Value: %s\n",ASTreeTypeToString(variableValue->type));

    // Return the variable definition ASTree
    return variable;
}

// Parse an Expression
ASTreeType* ParserParseExpression(ParserType* parser)
{   
    ASTreeType* AST;

    // Switch on the current token type
    switch(parser->CurrentToken->type)
    {
        case TOKEN_ID: AST=ParserParseIdentifier(parser);break;
        case TOKEN_STR: AST=ParserParseString(parser);break;
        case TOKEN_INTEGER: AST=ParserParseInteger(parser);break;
        case TOKEN_FLOAT: AST=ParserParseFloat(parser);break;
        case TOKEN_CHAR: AST=ParserParseCharacter(parser);break;
        case TOKEN_BOOL: AST=ParserParseBool(parser);break;
        case TOKEN_LBRACK: AST=ParserParseArray(parser);break;

        case TOKEN_INCR: AST=ParserParsePreIncrDecr(parser, 0);break;
        case TOKEN_DECR: AST=ParserParsePreIncrDecr(parser, 1);break;

        case TOKEN_ADD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_LPAREN: AST=ParserParseArithParenthesis(parser);break;

        case TOKEN_CHKEQUALS: AST=ParserParseConditions(parser);break;
        case TOKEN_NOTEQUALS: AST=ParserParseConditions(parser);break;
        case TOKEN_LANGLE: AST=ParserParseConditions(parser);break;
        case TOKEN_RANGLE: AST=ParserParseConditions(parser);break;
        case TOKEN_LANGB: AST=ParserParseConditions(parser);break;
        case TOKEN_RANGB: AST=ParserParseConditions(parser);break;

        // Raise an error if the current token is not an expression
        default: ParserExpressionError(parser); exit(1);
    }

    parser->PreviousAST=AST;
    ASTreeType* AST2;

    switch(parser->CurrentToken->type)
    {
        case TOKEN_ADD: AST2=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST2=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST2=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST2=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST2=ParserParseArithmetic(parser);break;

        case TOKEN_CHKEQUALS: AST2=ParserParseConditions(parser);break;
        case TOKEN_NOTEQUALS: AST2=ParserParseConditions(parser);break;
        case TOKEN_LANGLE: AST2=ParserParseConditions(parser);break;
        case TOKEN_RANGLE: AST2=ParserParseConditions(parser);break;
        case TOKEN_LANGB: AST2=ParserParseConditions(parser);break;
        case TOKEN_RANGB: AST2=ParserParseConditions(parser);break;

        default: {
            if(parser->PreviousAST->type==AST_VARIABLE
            || parser->PreviousAST->type==AST_BLOCK_ACCESS
            || parser->PreviousAST->type==AST_FUNCTION_CALL)
            {
                if(parser->CurrentToken->type==TOKEN_DOT)
                {
                    AST2=ParserParseMemberAccess(parser);
                }
                else
                {
                    return AST;
                }
            }
            else
            {
                return AST;
            }
        };break;
    }

    return AST2;
}

ASTreeType* ParserParseArithExpression(ParserType* parser)
{
    ASTreeType* AST;

    // Switch on the current token type
    switch(parser->CurrentToken->type)
    {
        case TOKEN_ID: AST=ParserParseIdentifier(parser);break;
        case TOKEN_STR: AST=ParserParseString(parser);break;
        case TOKEN_INTEGER: AST=ParserParseInteger(parser);break;
        case TOKEN_FLOAT: AST=ParserParseFloat(parser);break;
        case TOKEN_CHAR: AST=ParserParseCharacter(parser);break;
        case TOKEN_BOOL: AST=ParserParseBool(parser);break;
        case TOKEN_LBRACK: AST=ParserParseArray(parser);break;

        case TOKEN_INCR: AST=ParserParsePreIncrDecr(parser, 0);break;
        case TOKEN_DECR: AST=ParserParsePreIncrDecr(parser, 1);break;

        case TOKEN_ADD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST=ParserParseArithmetic(parser);break;

        case TOKEN_CHKEQUALS: AST=ParserParseConditions(parser);break;
        case TOKEN_NOTEQUALS: AST=ParserParseConditions(parser);break;
        case TOKEN_LANGLE: AST=ParserParseConditions(parser);break;
        case TOKEN_RANGLE: AST=ParserParseConditions(parser);break;
        case TOKEN_LANGB: AST=ParserParseConditions(parser);break;
        case TOKEN_RANGB: AST=ParserParseConditions(parser);break;

        // Raise an error if the current token is not an expression
        default: ParserExpressionError(parser); exit(1);
    }

    parser->PreviousAST=AST;
    ASTreeType* AST2;

    switch(parser->CurrentToken->type)
    {
        case TOKEN_CHKEQUALS: AST2=ParserParseConditions(parser);break;
        case TOKEN_NOTEQUALS: AST2=ParserParseConditions(parser);break;
        case TOKEN_LANGLE: AST2=ParserParseConditions(parser);break;
        case TOKEN_RANGLE: AST2=ParserParseConditions(parser);break;
        case TOKEN_LANGB: AST2=ParserParseConditions(parser);break;
        case TOKEN_RANGB: AST2=ParserParseConditions(parser);break;

        default: return AST;
    }

    parser->PreviousAST=AST2;

    return AST2;
}

ASTreeType* ParserParseConditions(ParserType* parser)
{
    ASTreeType* prev=parser->PreviousAST;
    ASTreeType* AST;

    switch(parser->CurrentToken->type)
    {
        case TOKEN_CHKEQUALS: AST=InitASTree(AST_CHKEQ);break;
        case TOKEN_NOTEQUALS: AST=InitASTree(AST_CHKNEQ);break;
        case TOKEN_LANGLE: AST=InitASTree(AST_CHKLESSEQ);break;
        case TOKEN_RANGLE: AST=InitASTree(AST_CHKGREATEREQ);break;
        case TOKEN_LANGB: AST=InitASTree(AST_CHKLESS);break;
        case TOKEN_RANGB: AST=InitASTree(AST_CHKGREATER);break;

        // Raise an error if the current token is not an expression
        default: printf("Invalid operation: %s\n",TokenToStr(parser->CurrentToken)); exit(1);
    }

    AST->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));
    AppendNodeArray(AST->RootValue,prev);
    ParserAdvanceToken(parser,parser->CurrentToken->type);
    AppendNodeArray(AST->RootValue,ParserParseExpression(parser));
    
    switch(parser->CurrentToken->type)
    {
        case TOKEN_CHKEQUALS: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;
        case TOKEN_NOTEQUALS: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;
        case TOKEN_LANGLE: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;
        case TOKEN_RANGLE: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;
        case TOKEN_LANGB: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;
        case TOKEN_RANGB: AppendNodeArray(AST->RootValue,ParserParseExpression(parser)); break;

        // Raise an error if the current token is not an expression
        default: return AST;
    }

    return AST;
}

ASTreeType* ParserParseAssignment(ParserType* parser, ASTreeType* variable)
{
    if(parser->CurrentToken->type==TOKEN_ADDEQ
    || parser->CurrentToken->type==TOKEN_SUBEQ
    || parser->CurrentToken->type==TOKEN_MULEQ
    || parser->CurrentToken->type==TOKEN_DIVEQ
    || parser->CurrentToken->type==TOKEN_MODEQ)
    {
        ParserAdvanceToken(parser,parser->CurrentToken->type);
    }
    else
    {
        ParserAdvanceToken(parser,TOKEN_EQUALS);
    }
    variable->tree_child=ParserParseExpression(parser);

    return variable;
}

ASTreeType* ParserParseReturn(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // Advance the keyword "return"

    char paren=0;

    ASTreeType* return_statement=InitASTree(AST_RETURN);

    if(parser->CurrentToken->type==TOKEN_SEMICOL)
    {
        return_statement->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));
        return return_statement;
    }
    
    if(parser->CurrentToken->type==TOKEN_LPAREN)
    {
        ParserAdvanceToken(parser,TOKEN_LPAREN);
        paren=1;
    }

    return_statement->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));

    AppendNodeArray(return_statement->RootValue,ParserParseExpression(parser));

    while(parser->CurrentToken->type==TOKEN_COMMA)
    {
        ParserAdvanceToken(parser,TOKEN_COMMA);
        AppendNodeArray(return_statement->RootValue,ParserParseExpression(parser));
    }

    if(paren)
    {
        ParserAdvanceToken(parser,TOKEN_RPAREN);
    }

    return return_statement;
}

ASTreeType* ParserParseBlock(ParserType* parser)
{
    ASTreeType* block=InitASTree(AST_BLOCK);

    ParserAdvanceToken(parser,TOKEN_LBRACE);

    block->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));

    while(parser->CurrentToken->type!=TOKEN_RBRACE)
    {
        ASTreeType* statement=ParserParseStatement(parser);

        if(parser->PreviousToken->type!=TOKEN_RBRACE)
            ParserAdvanceToken(parser,TOKEN_SEMICOL);
        AppendNodeArray(block->RootValue,statement);
    }

    ParserAdvanceToken(parser,TOKEN_RBRACE);
    
    return block;
}

// Parse a variable
ASTreeType* ParserParseVariable(ParserType* parser)
{
    // Save the variable name
    char* variableName = parser->CurrentToken->value;
    ASTreeType* variable_=InitASTree(AST_VARIABLE);
    variable_->name.variable_name=variableName;

    ASTreeType* variable=InitASTree(AST_VARIABLE);
    ParserAdvanceToken(parser,TOKEN_ID);

    if(parser->CurrentToken->type==TOKEN_LBRACK)
    {
        variable_->type=AST_BLOCK_ACCESS;
        ParserAdvanceToken(parser,TOKEN_LBRACK);
        variable_->blockaccess=ParserParseExpression(parser);
        ParserAdvanceToken(parser,TOKEN_RBRACK);

        variable->name.variable_name=variableName;
        variable->blockaccess=variable_;
    }
    else
    {
        variable=variable_;
    }

    // If the current token is a left parenthesis
    if(parser->CurrentToken->type==TOKEN_LPAREN)
    {
        // We assume that the it is a function call
        return ParserParseFunctionCall(parser);
    }
    else if(parser->CurrentToken->type==TOKEN_COLON)
    {
        ParserAdvanceToken(parser,TOKEN_COLON);

        variable->type=AST_VARIABLE_DECLARATION;
        variable->val_type.variable_def_value_type=parser->CurrentToken->value;

        ParserAdvanceToken(parser,TOKEN_ID);

        if(parser->CurrentToken->type==TOKEN_EQUALS)
        {  
            ParserAdvanceToken(parser,TOKEN_EQUALS);
            return ParserParseVariableDeclarationHelper(parser,variable);
        }
        else
        {
            variable->type=AST_VARIABLE;
            variable->val_type.variable_def_value_type=parser->PreviousToken->value;

            return variable;
        }
    }
    else if(parser->CurrentToken->type==TOKEN_EQUALS)
    {
        variable->type=AST_VARIABLE_ASSIGNMENT;

        return ParserParseAssignment(parser, variable);
    }
    else if(parser->CurrentToken->type==TOKEN_INCR)
    {
        ParserAdvanceToken(parser,TOKEN_INCR);
        variable->type=AST_VARIABLE_INCREMENT;
        variable->opts.preincrement_decrement=1; // post increment
        return variable;
    }
    else if(parser->CurrentToken->type==TOKEN_DECR)
    {
        ParserAdvanceToken(parser,TOKEN_DECR);
        variable->type=AST_VARIABLE_DECREMENT;
        variable->opts.preincrement_decrement=1; // post decrement
        return variable;
    }
    else if(parser->CurrentToken->type==TOKEN_ADDEQ
        ||  parser->CurrentToken->type==TOKEN_SUBEQ
        ||  parser->CurrentToken->type==TOKEN_MULEQ
        ||  parser->CurrentToken->type==TOKEN_DIVEQ
        ||  parser->CurrentToken->type==TOKEN_MODEQ)
    {
        variable->type=AST_VARIABLE_ASSIGNMENT;
        variable->opts.assignment=1;
        variable->opts.assignment_operator=parser->CurrentToken->type;

        return ParserParseAssignment(parser,variable);
    }

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
    AST->value.float_value=parser->CurrentToken->value;

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

ASTreeType* ParserParseArray(ParserType* parser)
{
    ASTreeType* AST=InitASTree(AST_ARRAY);

    AST->RootValue=InitNodeArray(sizeof(struct ASTreeStructure));

    ParserAdvanceToken(parser,TOKEN_LBRACK);

    AppendNodeArray(AST->RootValue,ParserParseExpression(parser));

    while(parser->CurrentToken->type==TOKEN_COMMA)
    {
        ParserAdvanceToken(parser,TOKEN_COMMA);
        AppendNodeArray(AST->RootValue,ParserParseExpression(parser));
    }

    ParserAdvanceToken(parser,TOKEN_RBRACK);

    return AST;
}

ASTreeType* ParserParsePreIncrDecr(ParserType* parser, char incrOrDecr)
{
    ParserAdvanceToken(parser,incrOrDecr==0?TOKEN_INCR:TOKEN_DECR);
    ASTreeType* AST=InitASTree(incrOrDecr==0?AST_VARIABLE_INCREMENT:AST_VARIABLE_DECREMENT);
    AST->opts.preincrement_decrement=0; // pre increment/decrement
    AST->tree_child=ParserParseVariable(parser);
    AST->name.variable_name=AST->tree_child->name.variable_name;

    return AST;
}

ASTreeType* ParserParseIf(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "if"

    ASTreeType* AST=InitASTree(AST_IF);

    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("
    AST->ifexpr=ParserParseExpression(parser);
    if(parser->CurrentToken->type!=TOKEN_RPAREN)
        AST->ifexpr=ParserParseExpression(parser);
    ParserAdvanceToken(parser,TOKEN_RPAREN); // ")"

    if(parser->CurrentToken->type!=TOKEN_LBRACE)
    {
        AST->ifbody=ParserParseStatement(parser);
    }
    else
    {
        AST->ifbody=ParserParseBlock(parser);
    }

    if(strcmp(parser->CurrentToken->value,"else")==0)
    {
        ParserAdvanceToken(parser,TOKEN_ID); // "else"

        if(strcmp(parser->CurrentToken->value,"if")==0)
        {
            AST->ifelse=ParserParseIf(parser);
        }
        else
        {
            AST->elsebody=ParserParseBlock(parser);
        }
    }

    return AST;
}

ASTreeType* ParserParseFor(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "for"

    ASTreeType* AST=InitASTree(AST_FOR);

    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("

    AST->forline=InitNodeArray(sizeof(struct ASTreeStructure));

    while(parser->CurrentToken->type!=TOKEN_RPAREN)
    {
        AppendNodeArray(AST->forline,ParserParseExpression(parser));

        if(parser->CurrentToken->type==TOKEN_SEMICOL)
        {
            ParserAdvanceToken(parser,TOKEN_SEMICOL);
        }
    }

    ParserAdvanceToken(parser,TOKEN_RPAREN); // ")"

    if(parser->CurrentToken->type!=TOKEN_LBRACE)
    {
        AST->forbody=ParserParseStatement(parser);
    }
    else
    {
        AST->forbody=ParserParseBlock(parser);
    }

    return AST;
}

ASTreeType* ParserParseWhile(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "while"

    ASTreeType* AST=InitASTree(AST_WHILE);

    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("
    AST->whileexpr=ParserParseExpression(parser);
    ParserAdvanceToken(parser,TOKEN_RPAREN); // ")"

    if(parser->CurrentToken->type!=TOKEN_LBRACE)
    {
        AST->whilebody=ParserParseStatement(parser);
    }
    else
    {
        AST->whilebody=ParserParseBlock(parser);
    }

    return AST;
}

ASTreeType* ParserParseClass(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "class"

    ASTreeType* AST=InitASTree(AST_CLASS_DECLARATION);
    AST->name.class_def_name=parser->CurrentToken->value;

    ParserAdvanceToken(parser,TOKEN_ID); // "extends"
    AST->args.class_def_arguments=InitNodeArray(sizeof(struct ASTreeStructure));

    if(parser->CurrentToken->type!=TOKEN_LBRACE)
    {
        AppendNodeArray(AST->args.class_def_arguments,ParserParseExpression(parser));
        ParserAdvanceToken(parser,TOKEN_ID);
    }

    while(parser->CurrentToken->type==TOKEN_COMMA)
    {
        ParserAdvanceToken(parser,TOKEN_COMMA);
        
        if(parser->CurrentToken->type==TOKEN_LBRACE)
            break;

        AppendNodeArray(AST->args.class_def_arguments,ParserParseExpression(parser));
    }

    AST->classbody=ParserParseBlock(parser);

    return AST;
}

ASTreeType* ParserParseMemberAccess(ParserType* parser)
{
    ASTreeType* AST=InitASTree(AST_MEMBER_ACCESS);
    AST->memaccess=parser->PreviousAST;

    ParserAdvanceToken(parser,TOKEN_DOT);
    
    AST->tree_child=ParserParseExpression(parser);

    return AST;
}

ASTreeType* ParserParseArithmetic(ParserType* parser)
{
    // Arithmetic, uses BODMAS rules

    // Initiate arithmetic sequence and add the previous variable/literal
    NodeArrayType* Sequence=InitNodeArray(sizeof(struct ASTreeStructure));
    AppendNodeArray(Sequence,parser->PreviousAST);
    
    // if the current token is an operator, add it to the sequence
    while( parser->CurrentToken->type==TOKEN_ADD
        || parser->CurrentToken->type==TOKEN_SUB 
        || parser->CurrentToken->type==TOKEN_MUL 
        || parser->CurrentToken->type==TOKEN_DIV 
        || parser->CurrentToken->type==TOKEN_MOD)
    {
        // if the current token is an operator, add it to the sequence
        switch(parser->CurrentToken->type)
        {
            case TOKEN_ADD: AppendNodeArray(Sequence,InitASTree(AST_ADD)); break;
            case TOKEN_SUB: AppendNodeArray(Sequence,InitASTree(AST_SUB)); break;
            case TOKEN_MUL: AppendNodeArray(Sequence,InitASTree(AST_MUL)); break;
            case TOKEN_DIV: AppendNodeArray(Sequence,InitASTree(AST_DIV)); break;
            case TOKEN_MOD: AppendNodeArray(Sequence,InitASTree(AST_MOD)); break;

            default: printf("Error: Unknown arithmetic operator - %s\n",parser->CurrentToken->value); break;
        }
        ParserAdvanceToken(parser,parser->CurrentToken->type);

        // if the current token is an opening parenthesis, then parse the closure and add the parenthesis to the sequence
        if(parser->CurrentToken->type==TOKEN_LPAREN)
        {
            ASTreeType* paren=ParserParseArithParenthesis(parser);

            AppendNodeArray(Sequence,paren);
        }

        // else, add it directly to the sequence
        else
            AppendNodeArray(Sequence,ParserParseArithExpression(parser));
    }

    // convert the sequence to postfix notation
    NodeArrayType* postfix=ArithPostfix(Sequence);

    //Commenting this out for now, as it's not needed.

    // * Used for debugging infix to postfix conversion
    /*
    for(Int i=1;i<postfix->size+1;i++)
    {
        printf("Postfix: %s\n",ASTreeTypeToString(postfix->trees[i]->type));
    }
    printf("\n");
    */
    //DestroyASTreeArray(Sequence);
    

    // convert the postfix notation to a tree
    NodeArrayType* stack=InitNodeArray(sizeof(struct ASTreeStructure));
    ASTreeType* A;
    ASTreeType* B;

    for(Int i=1;i<postfix->size+1;++i)
    {
        if(ArithIsOprnd(postfix->trees[i])==1)
        {
            AppendNodeArray(stack,postfix->trees[i]);
        }
        else
        {
            B=PopNodeArray(stack);
            A=PopNodeArray(stack);

            postfix->trees[i]->arithleft=A;
            postfix->trees[i]->arithright=B;

            AppendNodeArray(stack,postfix->trees[i]);
        }
    }

    ASTreeType* AST=PopNodeArray(postfix);

    return AST;
}

ASTreeType* ParserParseArithParenthesis(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("
    ASTreeType* AST=InitASTree(AST_ARITHPARENTHESIS);
    
    ParserParseArithExpression(parser);
    AST->tree_child=ParserParseArithmetic(parser);
    ParserAdvanceToken(parser,TOKEN_RPAREN); // ")"

    return AST;
}

ASTreeType* ParserParseFunction(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "func"

    ASTreeType* AST=InitASTree(AST_FUNCTION_DECLARATION);
    AST->name.function_def_name=parser->CurrentToken->value;
    ParserAdvanceToken(parser,TOKEN_ID); // name

    AST->args.function_def_arguments=ParserParseParenthesis(parser);

    if(parser->CurrentToken->type==TOKEN_RARROW)
    {
        ParserAdvanceToken(parser,TOKEN_RARROW);
        AST->val_type.function_def_return_type=parser->CurrentToken->value;
        ParserAdvanceToken(parser,TOKEN_ID); // return type
    }

    AST->funcbody=ParserParseBlock(parser);

    return AST;
}

// Parse a function call
ASTreeType* ParserParseFunctionCall(ParserType* parser)
{
    // Create the function call ASTree
    ASTreeType* functionCall=InitASTree(AST_FUNCTION_CALL);

    functionCall->name.function_call_name=parser->PreviousToken->value;

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