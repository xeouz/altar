#ifndef VISITORH
#define VISITORH

#include "../../Parser/IncludeLinker.h"

typedef struct VisitorStructure
{
    struct 
    {
        char current_scope;
        NodeArrayType* global;
        NodeArrayType* class_local;
        NodeArrayType* func_local;
    }scope;

    struct 
    {
        char** includes;
        Int includes_length;

        ASTreeType* current;

        struct 
        {
            char boost_any;
            char iostream;
            char string;
        }std_includes;

        USInt indent;
    }memory;

    char* code;
    Int code_length;
}VisitorType;

VisitorType* InitVisitor();

void VisitorAddIncludes(VisitorType* visitor, char* include);
char VisitorInIncludes(VisitorType* visitor, char* include);
char VisitorVariableDeclared(VisitorType* visitor, NodeArrayType* scope, char* var);
char* VisitorGetVariableType(VisitorType* visitor, NodeArrayType* scope, char* var);
char VisitorFunctionDeclared(VisitorType* visitor, NodeArrayType* scope, char* func);
ASTreeType* VisitorGetFunction(VisitorType* visitor, NodeArrayType* scope, char* func);
char VisitorIsSTDFunction(char* func);
char VisitorCheckFunctionArgType(VisitorType* visitor, ASTreeType* func, char* type, Int arg_index);
char* VisitorGetConditionOperator(USInt op);
char* VisitorGetArrayType(VisitorType* visitor, ASTreeType* node);

char* VisitorGetType(VisitorType* visitor, ASTreeType* node);

ASTreeType* VisitorGetVariable(VisitorType* visitor, NodeArrayType* scope, char* var);

ASTreeType* VisitorFindVariableDeclaration(VisitorType* visitor, char* var);

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root, char isHead);

char* VisitorTraverseIncludes(VisitorType* visitor);
char* VisitorTraverseNode(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseVariable(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseVariableDeclaration(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseVariableAssignment(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseMultiVariableDeclaration(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseFunctionCall(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseFunctionDeclaration(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseReturn(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseParenthesis(VisitorType* visitor, ASTreeType* node, NodeArrayType* arr, char* sep);

char* VisitorTraverseConditions(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseIf(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseArray(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseEchoCall(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseMemberAccess(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseBlockAccess(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseArithmetic(VisitorType* visitor, ASTreeType* node);

#endif