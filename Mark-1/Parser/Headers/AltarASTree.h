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
		AST_FUNCTION_DECLARATION,

		AST_VARIABLE_ASSIGNMENT,

		AST_FUNCTION_CALL,

		AST_STRING,
		AST_CHARACTER,
		AST_INTEGER,
		AST_FLOAT,
		AST_BOOL,

		AST_ROOT
	}type;

	struct ASTreeStructure* variable_def_value;

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