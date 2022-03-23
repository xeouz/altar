#ifndef VISITORC
#define VISITORC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Headers/AltarVisitor.h"

VisitorType* InitVisitor()
{
    VisitorType* visitor=calloc(1,sizeof(VisitorType));
    visitor->scope.current_scope='g';
    visitor->scope.global=InitNodeArray(sizeof(struct ASTreeStructure));
    visitor->scope.class_local=InitNodeArray(sizeof(struct ASTreeStructure));
    visitor->scope.func_local=InitNodeArray(sizeof(struct ASTreeStructure));
    visitor->code="";
    visitor->code_length=0;
    visitor->memory.includes=NULL;
    visitor->memory.includes_length=0;
    visitor->memory.std_includes.boost_any=0;
    visitor->memory.std_includes.iostream=0;
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

char VisitorVariableDeclared(VisitorType* visitor, NodeArrayType* scope, char* var)
{
    for(Int i=1;i<scope->size+1;++i)
    {
        if(i>scope->size)
        {
            return 0;
        }

        if(strcmp(scope->trees[i]->name.variable_def_name,var)==0)
        {
            return 1;
        }
    }
    return 0;
}

char* VisitorGetVariableType(VisitorType* visitor, NodeArrayType* scope, char* var)
{
    for(Int i=1;i<scope->size+1;++i)
    {
        if(i>scope->size)
        {
            return "";
        }

        if(strcmp(scope->trees[i]->name.variable_def_name,var)==0)
        {
            return scope->trees[i]->val_type.variable_def_value_type;
        }
    }
    return "";
}

ASTreeType* VisitorGetVariable(VisitorType* visitor, NodeArrayType* scope, char* var)
{
    for(Int i=1;i<scope->size+1;++i)
    {
        if(i>scope->size)
        {
            return 0;
        }

        if(strcmp(scope->trees[i]->name.variable_def_name,var)==0)
        {
            return scope->trees[i];
        }
    }
    return 0;
}

// ---

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root)
{   
    // Traversing every node

    for(Int i=1;i<root->RootValue->size;++i)
    {
        VisitorTraverseNode(visitor,root->RootValue->trees[i]);
    }

    return "";
}

char* VisitorTraverseNode(VisitorType* visitor, ASTreeType* node)
{
    switch(node->type)
    {
        case AST_VARIABLE: return VisitorTraverseVariable(visitor,node);
        case AST_VARIABLE_DECLARATION: return VisitorTraverseVariableDeclaration(visitor,node);
        case AST_VARIABLE_ASSIGNMENT: return VisitorTraverseVariableAssignment(visitor,node);
        case AST_MULTI_VARIABLE_DECLARATION: return VisitorTraverseMultiVariableDeclaration(visitor,node);

        case AST_FUNCTION_CALL: return VisitorTraverseFunctionCall(visitor,node);

        case AST_INTEGER: {
            char* code=calloc(1,1);
            sprintf(code,"%d",node->value.integer_value);
            return code;
        }

        default: printf("Unknown Node Type: %s\n",ASTreeTypeToString(node->type)); exit(1); return NULL;
    }
}

char* VisitorTraverseVariable(VisitorType* visitor, ASTreeType* node)
{
    NodeArrayType* scope;
    switch(visitor->scope.current_scope)
    {
        case 'g': scope=visitor->scope.global; break;
        case 'c': scope=visitor->scope.class_local; break;

        default: scope=visitor->scope.func_local; break;
    }
    if(!VisitorVariableDeclared(visitor,scope,node->name.variable_name))
    {
        printf("Variable %s not declared\n",node->name.variable_name);
        exit(1);
        return NULL;
    }
    return node->name.variable_name;
}

char* VisitorTraverseVariableDeclaration(VisitorType* visitor, ASTreeType* node)
{   
    // Declare variables
    char* variable_name="";
    char* variable_type="";
    char* variable_value="";
    char* variable="";

    char explicit_value=0, free_variable_value=0;
    NodeArrayType* scope;

    // Check for the scope and assign the current scope
    switch(visitor->scope.current_scope)
    {
        case 'g': scope=visitor->scope.global; break;
        case 'c': scope=visitor->scope.class_local; break;
        default: scope=visitor->scope.func_local; break;
    }

    // Check if the variable is already declared
    if(VisitorVariableDeclared(visitor,scope,node->name.variable_def_name))
    {
        printf("Variable %s already declared\n",node->name.variable_def_name);
        exit(1);
        return NULL;
    }

    if(node->tree_child!=NULL)
        explicit_value=1;

    // - Variable name
    variable_name=node->name.variable_def_name;

    // - Variable type
    if(node->val_type.variable_def_value_type==NULL)
    {
        if(node->tree_child==NULL)
        {
            visitor->memory.std_includes.boost_any=1;
            variable_type="boost::any";
        }
        else
        {
            switch(node->tree_child->type)
            {
                case AST_INTEGER: variable_type="int"; free_variable_value=1; break;
                case AST_FLOAT: variable_type="float"; free_variable_value=1; break;
                case AST_CHARACTER: variable_type="char"; break;
                case AST_BOOL: variable_type="bool"; break;
                case AST_STRING: {
                    variable_type="std::string";
                    visitor->memory.std_includes.string=1;
                    break;
                }

                case AST_VARIABLE: {
                    variable_type=VisitorGetVariableType(visitor,scope,node->tree_child->name.variable_name);
                    break;
                }

                default: printf("Unknown Variable Type: %d\n",node->tree_child->type); exit(1); return NULL;
            }
        }
    }
    else
    {
        if(strcmp(node->val_type.variable_def_value_type,"any")==0)
        {
            variable_type="boost::any";
            visitor->memory.std_includes.boost_any=1;
        }
        else if(strcmp(node->val_type.variable_def_value_type,"str")==0)
        {
            variable_type="std::string";
            visitor->memory.std_includes.string=1;
        }
        else
        {
            variable_type=node->val_type.variable_def_value_type;
        }
    }
    node->val_type.variable_def_value_type=variable_type;

    // - Variable value
    if(explicit_value)
    {
        variable_value=VisitorTraverseNode(visitor,node->tree_child);
    }

    // - Variable 

    /* Apply the corresponding format to the variable

     * type name = value;

     * sprintf() will format the string 
     * and assign it to the variable
    */
    //sprintf(variable,"%s %s = %s;",variable_type,variable_name,variable_value);
    variable=calloc(1,1);
    if(explicit_value)
    {
        sprintf(variable,"%s %s = %s;",variable_type,variable_name,variable_value);
    }
    else
    {
        sprintf(variable,"%s %s;",variable_type,variable_name);
    }

    if(free_variable_value)
    {
        free(variable_value);
    }
    
    // Reset the original scope
    switch(visitor->scope.current_scope)
    {
        case 'g': visitor->scope.global=scope; break;
        case 'c': visitor->scope.class_local=scope; break;
        default: visitor->scope.func_local=scope; break;
    }

    AppendNodeArray(scope,node);
    
    return variable;
}

char* VisitorTraverseVariableAssignment(VisitorType* visitor, ASTreeType* node)
{
    NodeArrayType* scope;
    switch(visitor->scope.current_scope)
    {
        case 'g': scope=visitor->scope.global; break;
        case 'c': scope=visitor->scope.class_local; break;
        default: scope=visitor->scope.func_local; break;
    }

    // Checking if the variable is declared
    if(!VisitorVariableDeclared(visitor,scope,node->name.variable_name))
    {
        printf("Variable %s not declared\n",node->name.variable_name);
        exit(1);
        return NULL;
    }

    char* variable_name=node->name.variable_name;
    ASTreeType* variable=VisitorGetVariable(visitor,scope,variable_name);

    char diff_type=0;

    // Checking definition type
    switch(node->tree_child->type)
    {
        case AST_INTEGER: {
            if(strcmp(variable->val_type.variable_def_value_type,"int")!=0)
                diff_type=1;
            break;
        }

        case AST_CHARACTER: {
            if(strcmp(variable->val_type.variable_def_value_type,"char")!=0)
                diff_type=1;
            break;
        }

        case AST_FLOAT: {
            if(strcmp(variable->val_type.variable_def_value_type,"float")!=0)
                diff_type=1;
            break;
        }

        case AST_BOOL: {
            if(strcmp(variable->val_type.variable_def_value_type,"bool")!=0)
                diff_type=1;
            break;
        }

        case AST_STRING: {
            if(strcmp(variable->val_type.variable_def_value_type,"std::string")!=0)
                diff_type=1;
            break;
        }

        case AST_VARIABLE: {
            char* type=VisitorGetVariableType(visitor,scope,node->name.variable_name);
            if(strcmp(type,"boost::any")==0)
                diff_type=0;
            else if(strcmp(VisitorGetVariableType(visitor,scope,node->tree_child->name.variable_name),type)!=0)
            {
                diff_type=1;
            }
            break;
        }

        default: printf("Unknown Variable Type: %s\n",ASTreeTypeToString(node->tree_child->type)); exit(1); return NULL;
    }

    char* variable_value;

    variable_value=VisitorTraverseNode(visitor,node->tree_child);

    if(diff_type)
    {
        printf("Variable %s cannot be assigned %s\n",variable_name,variable_value);
        exit(1);
        return NULL;
    }

    char* variable_str=calloc(1,1);

    sprintf(variable_str,"%s = %s;",variable_name,variable_value);

    return variable_str;
}

char* VisitorTraverseMultiVariableDeclaration(VisitorType* visitor, ASTreeType* node)
{
    char* statement = calloc(1,1);

    for(Int i=1;i<node->RootValue->size+1;i++)
    {
        if(i>node->RootValue->size)
            return "";

        strcat(statement,VisitorTraverseVariableDeclaration(visitor,node->RootValue->trees[i]));
        strcat(statement,"\n");
    }

    return statement;
}

char* VisitorTraverseFunctionCall(VisitorType* visitor, ASTreeType* node)
{
    return "";
}

#endif