#ifndef ASTREES
#define ASTREES

#include "NodeArray.h"

#include "Integers.h"

typedef struct ASTreeStructure
{
	enum
	{
		AST_VARIABLE,

		AST_VARIABLE_DECLARATION,
		AST_MULTI_VARIABLE_DECLARATION,
		AST_PROTO_DECLARATION,
		AST_CLASS_DECLARATION,
		AST_FUNCTION_DECLARATION,

		AST_VARIABLE_ASSIGNMENT,

		AST_FUNCTION_CALL,
		AST_CLASS_MAKE,

		AST_STRING,
		AST_CHARACTER,
		AST_INTEGER,
		AST_FLOAT,
		AST_BOOL,

		AST_VARIABLE_OPERATION,

		AST_INCREMENT,
		AST_DECREMENT,

		AST_IF,
		AST_ELSE,
		AST_ELSEIF,
		AST_WHILE,
		AST_FOR,
		AST_BREAK,
		AST_CONTINUE,
		AST_RETURN,

		AST_TRY,
		AST_CATCH,
		AST_FINALLY,

		AST_PARENTHESIS,

		AST_ROOT
	}type;

	struct ASTreeStructure* tree_child;

	union
	{
		char* variable_name;
		char* variable_def_name;
		
		char* function_call_name;
		char* function_def_name;
	}name;

	union
	{
		Int function_call_arguments_size;
		Int function_def_arguments_size;
	}size;

	union
	{
		NodeArrayType* function_def_arguments;
		NodeArrayType* function_call_arguments;
	}args;

	// Value
	union
	{
		char* string_value;
		char char_value;
		SInt integer_value;
		IChar bool_value;
		float float_value;
	}value;

	union
	{
		char* variable_def_value_type;
	}val_type;

	NodeArrayType* RootValue;
}ASTreeType;

// Initialization
ASTreeType* InitASTree(char type);

// Destruction
void DestroyASTree(ASTreeType* AST);

#endif