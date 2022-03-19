#ifndef ASTREEC
#define ASTREEC

#include "Headers/AltarASTree.h"
#include "NodeArray.c"
#include "Headers/Integers.h"

char* ASTreeTypeToString(Int type)
{
    switch(type)
    {
        case AST_VARIABLE: return "AST_VARIABLE";

		case AST_VARIABLE_DECLARATION: return "AST_VARIABLE_DECLARATION";
		case AST_MULTI_VARIABLE_DECLARATION: return "AST_MULTI_VARIABLE_DECLARATION";
		case AST_PROTO_DECLARATION: return "AST_PROTO_DECLARATION";
		case AST_CLASS_DECLARATION: return "AST_CLASS_DECLARATION";
		case AST_FUNCTION_DECLARATION: return "AST_FUNCTION_DECLARATION";

		case AST_VARIABLE_ASSIGNMENT: return "AST_VARIABLE_ASSIGNMENT";

		case AST_FUNCTION_CALL: return "AST_FUNCTION_CALL";
		case AST_CLASS_MAKE: return "AST_CLASS_MAKE";

		case AST_STRING: return "AST_STRING";
		case AST_CHARACTER: return "AST_CHARACTER";
		case AST_INTEGER: return "AST_INTEGER";
		case AST_FLOAT: return "AST_FLOAT";
		case AST_BOOL: return "AST_BOOL"; 

		case AST_VARIABLE_INCREMENT: return "AST_VARIABLE_INCREMENT";
		case AST_VARIABLE_DECREMENT: return "AST_VARIABLE_DECREMENT";

		case AST_IF: return "AST_IF";
		case AST_ELSE: return "AST_ELSE"; 
		case AST_ELSEIF: return "AST_ELSEIF";
		case AST_WHILE: return "AST_WHILE";
		case AST_FOR: return "AST_FOR";
		case AST_BREAK: return "AST_BREAK";
		case AST_CONTINUE: return "AST_CONTINUE";
		case AST_RETURN: return "AST_RETURN";
  
		case AST_TRY: return "AST_TRY";
		case AST_CATCH: return "AST_CATCH"; 
		case AST_FINALLY: return "AST_FINALLY";

		case AST_PARENTHESIS: return "AST_PARENTHESIS";

		case AST_ADD: return "AST_ADD";
		case AST_SUB: return "AST_SUB";
		case AST_MUL: return "AST_MUL";
		case AST_DIV: return "AST_DIV";
		case AST_MOD: return "AST_MOD";
		case AST_FLR: return "AST_FLR";

		case AST_SHIFTLN: return "AST_SHIFTLN";
		case AST_BLOCK: return "AST_BLOCK";
		case AST_PLACEHOLDER: return "AST_PLACEHOLDER";
		case AST_ROOT: return "AST_ROOT";

        default: return "UNKNOWN";
    }
}

// -- Initialization --
ASTreeType* InitASTree(char type)
{
    ASTreeType* AST=calloc(1,sizeof(struct ASTreeStructure));
    AST->type=type;

    return AST;
}

// -- Destruction --
void DestroyASTree(ASTreeType* AST)
{
    if(AST->tree_child)
    {
        DestroyASTree(AST->tree_child);
    }
    if(AST->RootValue)
    {
        DestroyASTreeArray(AST->RootValue);
    }

    free(AST);
}

#endif