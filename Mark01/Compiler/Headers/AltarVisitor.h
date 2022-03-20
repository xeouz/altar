#ifndef VISITORH
#define VISITORH

#include "../../Parser/IncludeLinker.h"

typedef struct VisitorStructure
{
    struct 
    {
        NodeArrayType* global;
        NodeArrayType* class_local;
        NodeArrayType* func_local;
    }scope;

    struct 
    {
        char** includes;
        Int includes_length;

        char** dynamic_vars;
        Int dynamic_vars_length;

        char** declared_vars;
        Int declared_vars_length;
    }memory;

    char* code;
    Int code_length;
}VisitorType;

VisitorType* InitVisitor();

void VisitorAddIncludes(VisitorType* visitor, char* include);
char VisitorInIncludes(VisitorType* visitor, char* include);

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root);

char* VisitorTraverseNode(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseVariable(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseVariableDeclaration(VisitorType* visitor, ASTreeType* node);
char* VisitorTraverseVariableAssignment(VisitorType* visitor, ASTreeType* node);

char* VisitorTraverseFunctionCall(VisitorType* visitor, ASTreeType* node);

#endif