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
    parser->PreviousAST = InitASTree(AST_SHIFTLN);

    return parser;
}

// -- Errors --
void ParserEOFError()
{
    printf("[Parser] - End of File\n");
}

void ParserStatementError(ParserType* parser)
{
    printf("[Parser] ParserStatementError - Invalid Token: %s %s\n\n",parser->CurrentToken->value,TokenToStr(parser->CurrentToken)); 
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

        case TOKEN_ADD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST=ParserParseArithmetic(parser);break;

        default: AST=InitASTree(AST_SHIFTLN); break;
    }

    return AST;
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
    while(parser->CurrentToken->type == TOKEN_SEMICOL || parser->CurrentToken->type!=TOKEN_ENDFL)
    {
        // Advance the token SemiColon
        if(parser->CurrentToken->type == TOKEN_SEMICOL)
            ParserAdvanceToken(parser, TOKEN_SEMICOL);

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
    // printf("[Parser] - Variable Declaration w/ Value: %s\n",variable->name.variable_def_name);
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

        case TOKEN_ADD: AST=ParserParseArithmetic(parser);break;
        case TOKEN_SUB: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MUL: AST=ParserParseArithmetic(parser);break;
        case TOKEN_DIV: AST=ParserParseArithmetic(parser);break;
        case TOKEN_MOD: AST=ParserParseArithmetic(parser);break;

        // Raise an error if the current token is not an expression
        default: ParserExpressionError(parser); exit(1);
    }

    parser->PreviousAST=AST;
    return AST;
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

*/

ASTreeType* ParserParseReturn(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // Advance the keyword "return"

    char paren=0;

    ASTreeType* return_statement=InitASTree(AST_RETURN);

    if(parser->CurrentToken->type==TOKEN_SEMICOL)
    {
        ParserAdvanceToken(parser,TOKEN_SEMICOL);
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

        if(parser->CurrentToken->type==TOKEN_SEMICOL)
        {
            ParserAdvanceToken(parser,TOKEN_SEMICOL);
        }

        AppendNodeArray(block->RootValue,statement);
    }

    ParserAdvanceToken(parser,TOKEN_RBRACE);

    if(parser->CurrentToken->value==NULL)
    {
        parser->CurrentToken->value=calloc(1,1);
        parser->CurrentToken->value[0]=0;
    }

    return block;
}

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

ASTreeType* ParserParseIf(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_ID); // "if"

    ASTreeType* AST=InitASTree(AST_IF);

    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("
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
    AST->forinit=ParserParseStatement(parser);
    ParserAdvanceToken(parser,TOKEN_SEMICOL); // ";"
    AST->forcond=ParserParseExpression(parser);
    ParserAdvanceToken(parser,TOKEN_SEMICOL); // ";"
    AST->forinc=ParserParseStatement(parser);
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
            NodeArrayType* closure=ParserParseClosure(parser);

            for(Int i=1;i<closure->size;i++)
            {
                AppendNodeArray(Sequence,closure->trees[i]);
            }
        }

        // else, add it directly to the sequence
        else
            AppendNodeArray(Sequence,ParserParseExpression(parser));
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

    ASTreeType* AST=PopNodeArray(stack);

    return AST;
}

NodeArrayType* ParserParseClosure(ParserType* parser)
{
    ParserAdvanceToken(parser,TOKEN_LPAREN); // "("

    NodeArrayType* line=InitNodeArray(sizeof(struct ASTreeStructure));

    ASTreeType* leftParen=InitASTree(AST_PLACEHOLDER);
    leftParen->value.char_value='(';
    AppendNodeArray(line,leftParen);

    while(parser->CurrentToken->type!=TOKEN_RPAREN)
    {
        if(parser->CurrentToken->type!=TOKEN_LPAREN)
            AppendNodeArray(line,ParserParseExpression(parser));
        else
        {
            NodeArrayType* closure=ParserParseClosure(parser);

            for(Int i=1;i<closure->size;i++)
            {
                AppendNodeArray(line,closure->trees[i]);
            }

        }

        if(parser->CurrentToken->type==TOKEN_COMMA)
        {
            ParserAdvanceToken(parser,TOKEN_COMMA);
        }
    }
    ParserAdvanceToken(parser,TOKEN_RPAREN); // ")"

    ASTreeType* rightParen=InitASTree(AST_PLACEHOLDER);
    rightParen->value.char_value=')';
    AppendNodeArray(line,rightParen);

    return line;
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
    //char* variableName=parser->PreviousToken->value;

    //printf("[Parser] - Function Call: %s\n",variableName);

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