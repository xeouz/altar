#ifndef ASTREES
#define ASTREES

#include "Integers.h"

typedef struct NodeArrayStructure NodeArrayType;

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

		AST_SHIFTLN,
		AST_BLOCK,
		AST_ROOT
	}type;

	struct ASTreeStructure* tree_child;

	union
	{
		char* variable_name;
		char* variable_def_name;
		
		char* function_call_name;
		char* function_def_name;

		char* class_def_name;
	}name;

	union
	{
		Int function_call_arguments_size;
		Int function_def_arguments_size;
	}size;

	union
	{
		struct NodeArrayStructure* function_def_arguments;
		struct NodeArrayStructure* function_call_arguments;
		struct NodeArrayStructure* class_def_arguments;
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
		char* function_def_return_type;
	}val_type;

	struct ASTreeStructure* ifexpr;
	struct ASTreeStructure* ifbody;
	struct ASTreeStructure* ifelse;
	struct ASTreeStructure* elsebody;

	struct ASTreeStructure* whileexpr;
	struct ASTreeStructure* whilebody;

	struct ASTreeStructure* forinit;
	struct ASTreeStructure* forcond;
	struct ASTreeStructure* forinc;
	struct ASTreeStructure* forbody;

	struct ASTreeStructure* funcbody;
	struct ASTreeStructure* classbody;

	struct NodeArrayStructure* RootValue;
}ASTreeType;

// Initialization
ASTreeType* InitASTree(char type);

// Destruction
void DestroyASTree(ASTreeType* AST);

#endif