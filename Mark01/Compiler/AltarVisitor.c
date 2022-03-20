#ifndef VISITORC
#define VISITORC

#include <stdio.h>
#include <stdlib.h>

#include "Headers/AltarVisitor.h"

VisitorType* InitVisitor()
{
    VisitorType* visitor=calloc(1,sizeof(VisitorType));
    visitor->scope.global=NULL;
    visitor->scope.class_local=NULL;
    visitor->scope.func_local=NULL;
    visitor->code="";
    visitor->code_length=0;
    visitor->memory.includes=NULL;
    visitor->memory.includes_length=0;
    visitor->memory.dynamic_vars=NULL;
    visitor->memory.dynamic_vars_length=0;
    return visitor;
}

void VisitorAddIncludes(VisitorType* visitor, char* include)
{
    if(visitor->memory.includes_length==0)
    {
        visitor->memory.includes=calloc(1,sizeof(char*));
        visitor->memory.includes_length=1;
    }
    else
    {
        visitor->memory.includes=realloc(visitor->memory.includes,(visitor->memory.includes_length+1)*sizeof(char*));
        visitor->memory.includes_length++;
    }
    visitor->memory.includes[visitor->memory.includes_length-1]=include;
}

char VisitorInIncludes(VisitorType* visitor, char* include)
{
    for(Int i=0;i<visitor->memory.includes_length;i++)
    {
        if(strcmp(visitor->memory.includes[i],include)==0)
        {
            return 1;
        }
    }
    return 0;
}

// ---

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root)
{
    VisitorTraverseVariableDeclaration(visitor,root->RootValue->trees[1]);

    return "";
}

char* VisitorTraverseNode(VisitorType* visitor, ASTreeType* node)
{
    switch(node->type)
    {
        case AST_VARIABLE: return VisitorTraverseVariable(visitor,node);
        case AST_VARIABLE_DECLARATION: return VisitorTraverseVariableDeclaration(visitor,node);
        case AST_VARIABLE_ASSIGNMENT: return VisitorTraverseVariableAssignment(visitor,node);

        case AST_FUNCTION_CALL: return VisitorTraverseFunctionCall(visitor,node);

        default: printf("Unknown Node Type: %d\n",node->type); exit(1); return NULL;
    }
}

char* VisitorTraverseVariable(VisitorType* visitor, ASTreeType* node)
{
    return "";
}

char* VisitorTraverseVariableDeclaration(VisitorType* visitor, ASTreeType* node)
{

    if(visitor->memory.declared_vars_length==0)
    {
        visitor->memory.declared_vars=calloc(1,sizeof(char*));
        visitor->memory.declared_vars_length=1;
        visitor->memory.declared_vars[0]=node->name.variable_def_name;
    }
    else
    {
        for(Int i=0;i<visitor->memory.declared_vars_length;i++)
        {
            if(strcmp(visitor->memory.declared_vars[i],node->name.variable_def_name)==0)
            {
                printf("Variable %s already declared\n",node->name.variable_def_name);
                exit(1);
                return NULL;
            }
        }
        visitor->memory.declared_vars=realloc(visitor->memory.declared_vars,(visitor->memory.declared_vars_length+1)*sizeof(char*));
        ++visitor->memory.declared_vars_length;
        visitor->memory.declared_vars[visitor->memory.declared_vars_length-1]=node->name.variable_def_name;
    }

    // Variable type determination
    char* variable_type="";
    if(node->val_type.variable_def_value_type==NULL)
    {
        switch(node->tree_child->type)
        {
            case AST_INTEGER: variable_type="int"; break;
            case AST_FLOAT: variable_type="float"; break;
            case AST_STRING: variable_type="std::string"; break;
            case AST_BOOL: variable_type="bool"; break;
            default: variable_type="auto"; break;
        }
    }
    else
    {
        char* type=node->val_type.variable_def_value_type;
        type=type;
        
        if(strcmp(type,"int")==0)
        {
            variable_type="int";
        }
        else if(strcmp(type,"float")==0)
        {
            variable_type="float";
        }
        else if(strcmp(type,"string")==0)
        {
            variable_type="std::string";
        }
        else if(strcmp(type,"char")==0)
        {
            variable_type="char";
        }
        else if(strcmp(type,"bool")==0)
        {
            variable_type="bool";
        }
        else if(strcmp(type,"any")==0)
        {
            VisitorAddIncludes(visitor,"<boost/any.hpp>");
            variable_type="boost::any";
        }
        else
        {
            variable_type="auto";
        }
    }

    // Variable name
    char* variable_name=node->name.variable_def_name;

    // Variable value
    char* variable_value=calloc(1,1);
    switch(node->tree_child->type)
    {
        case AST_BOOL: {
            if(node->tree_child->value.bool_value==1)
            {
                variable_value=realloc(variable_value,5);
                strcpy(variable_value,"true");
            }
            else
            {
                variable_value=realloc(variable_value,6);
                strcpy(variable_value,"false");
            }
            break;
        }
        case AST_INTEGER: sprintf(variable_value,"%d",node->tree_child->value.integer_value); break;
        case AST_ARRAY: {
            variable_value="";
            break;
        }
        case AST_STRING: {
            char temp[strlen(node->tree_child->value.string_value)+3];
            sprintf(temp,"\"%s\"",node->tree_child->value.string_value);
            variable_value=realloc(variable_value,strlen(temp)+1);
            strcpy(variable_value,temp);
            break;
        }
        case AST_CHARACTER: {
            sprintf(variable_value,"'%c'",(char)node->tree_child->value.char_value);
            break;
        }

        default: printf("Unknown Variable Value Type: %s\n",ASTreeTypeToString(node->tree_child->type)); exit(1); return NULL;
    }

    char* variable = calloc(1,1);
    char temp[strlen(variable_type)+strlen(variable_name)+strlen(variable_value)+100];
    sprintf(temp,"%s %s = %s;\n",variable_type,variable_name,variable_value);
    variable=realloc(variable,strlen(temp)+1);
    strcpy(variable,temp);

    free(variable_value);

    printf("%s",variable);

    return variable;
}

char* VisitorTraverseVariableAssignment(VisitorType* visitor, ASTreeType* node)
{
    return "";
}

char* VisitorTraverseFunctionCall(VisitorType* visitor, ASTreeType* node)
{
    return "";
}

#endif