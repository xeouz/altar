#ifndef VISITORC
#define VISITORC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
        visitor->memory.includes=calloc(1,1);
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
            break;
        }

        if(strcmp(scope->trees[i]->name.variable_def_name,var)==0)
        {
            return scope->trees[i];
        }
    }

    if(scope!=visitor->scope.global)
    {
        for(Int i=1;i<visitor->scope.global->size+1;++i)
        {
            if(i>visitor->scope.global->size)
            {
                return NULL;
            }

            if(strcmp(visitor->scope.global->trees[i]->name.variable_def_name,var)==0)
            {
                return visitor->scope.global->trees[i];
            }
        }
    } 

    return 0;
}

char VisitorFunctionDeclared(VisitorType* visitor, NodeArrayType* scope, char* func)
{
    for(Int i=1;i<scope->size+1;++i)
    {
        if(i>scope->size)
        {
            break;
        }

        if(scope->trees[i]->type==AST_FUNCTION_DECLARATION)
        {
            if(strcmp(scope->trees[i]->name.function_def_name,func)==0)
            {
                return 1;
            }
        }
    }

    for(Int i=1;i<visitor->scope.global->size+1;++i)
    {
        if(i>visitor->scope.global->size)
        {
            return 0;
        }

        if(visitor->scope.global->trees[i]->type==AST_FUNCTION_DECLARATION)
        {
            if(strcmp(visitor->scope.global->trees[i]->name.function_def_name,func)==0)
            {
                return 1;
            }
        }
    }

    return 0;
}

ASTreeType* VisitorGetFunction(VisitorType* visitor, NodeArrayType* scope, char* func)
{
    for(Int i=1;i<scope->size+1;++i)
    {
        if(i>scope->size)
        {
            break;
        }

        if(scope->trees[i]->type==AST_FUNCTION_DECLARATION)
        {
            if(strcmp(scope->trees[i]->name.function_call_name,func)==0)
            {
                return scope->trees[i];
            }
        }
    }

    if(scope!=visitor->scope.global)
    {
        for(Int i=1;i<visitor->scope.global->size+1;++i)
        {
            if(i>visitor->scope.global->size)
            {
                return 0;
            }

            if(visitor->scope.global->trees[i]->type==AST_FUNCTION_DECLARATION)
            {
                if(strcmp(visitor->scope.global->trees[i]->name.function_def_name,func)==0)
                {
                    return visitor->scope.global->trees[i];
                }
            }
        }
    }
    return NULL;
}

char VisitorIsSTDFunction(char* func)
{
    if(strcmp(func,"echo")==0)
        return 1;
    else 
        return 0;
}

char VisitorCheckFunctionArgType(VisitorType* visitor, ASTreeType* func, char* type, Int i)
{
    /* Code present for debugging purposes */
    // printf("%s %s\n",func->args.function_def_arguments->trees[i]->val_type.variable_def_value_type,type);

    if(strcmp(func->args.function_def_arguments->trees[i]->val_type.variable_def_value_type,"boost::any")==0)
        return 1;
    else if(strcmp(func->args.function_def_arguments->trees[i]->val_type.variable_def_value_type,"auto")==0)
        return 1;
    else if(strcmp(func->args.function_def_arguments->trees[i]->val_type.variable_def_value_type,type)==0)
        return 1;
    else
        return 0;
}

// ---

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root, int indent)
{   
    // Traversing every node
    char* code=calloc(1,1);
    char* line;
    int j;
    for(Int i=1;i<root->RootValue->size+1;++i)
    {
        if(i>root->RootValue->size)
            break;

        if(root->RootValue->trees[i]->type==AST_ENDL)
            break;

        line=VisitorTraverseNode(visitor,root->RootValue->trees[i]);
        code=realloc(code,(strlen(code)+strlen(line)+2+(indent*3)));

        for(j=0;j<indent;++j)
        {
            strcat(code,"   ");
        }

        strcat(code,line);

        if(root->RootValue->trees[i]->type!=AST_FUNCTION_DECLARATION && root->RootValue->trees[i]->type!=AST_CLASS_DECLARATION)
        {
            code=realloc(code,(strlen(code)+2));
            strcat(code,";");
        }
        strcat(code,"\n");
        
        free(line);
    }

    char* includes=VisitorTraverseIncludes(visitor);

    if(indent==0)
    {
        char* code_with_includes=calloc(1,(strlen(code)+strlen(includes)+2));
        sprintf(code_with_includes,"%s\n%s",includes,code);
        free(includes);

        return code_with_includes;
    }

    return code;
}

char* VisitorTraverseIncludes(VisitorType* visitor)
{
    char* code=calloc(1,1);
    if(visitor->memory.std_includes.boost_any==1)
    {
        code=realloc(code,(strlen(code)+strlen("#include <boost/any.hpp>\n")+2));
        strcat(code,"#include <boost/any.hpp>\n");
    }
    if(visitor->memory.std_includes.iostream==1)
    {
        code=realloc(code,(strlen(code)+strlen("#include <iostream>\n")+2));
        strcat(code,"#include <iostream>\n");
    }
    if(visitor->memory.std_includes.string==1)
    {
        code=realloc(code,(strlen(code)+strlen("#include <string>\n")+2));
        strcat(code,"#include <string>\n");
    }

    return code;
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
        case AST_FUNCTION_DECLARATION: return VisitorTraverseFunctionDeclaration(visitor,node);
        case AST_RETURN: return VisitorTraverseReturn(visitor, node);

        case AST_INTEGER: {
            char* code=calloc(node->value.integer_value==0?1:log10((double)node->value.integer_value)+2,1);
            sprintf(code,"%d",node->value.integer_value);
            return code;
        }

        case AST_CHARACTER: {
            char* code=calloc(4,1);
            sprintf(code,"'%c'",node->value.char_value);
            return code;
        }

        case AST_FLOAT: {
            char* code=calloc(strlen(node->value.float_value),1);
            sprintf(code,"%s",node->value.float_value);
            return code;
        }

        case AST_STRING: {
            char* code=calloc(strlen(node->value.string_value)+3,1);
            sprintf(code,"\"%s\"",node->value.string_value);
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
    if(!VisitorVariableDeclared(visitor,scope,node->name.variable_name) && !VisitorVariableDeclared(visitor,visitor->scope.global,node->name.variable_name))
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
            node->val_type.variable_def_value_type=variable;
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
    //sprintf(variable,"%s %s = %s",variable_type,variable_name,variable_value);
    variable=calloc(strlen(variable_type)+strlen(variable_name)+strlen(variable_value)+6,1);
    if(explicit_value)
    {
        sprintf(variable,"%s %s = %s",variable_type,variable_name,variable_value);
    }
    else
    {
        sprintf(variable,"%s %s",variable_type,variable_name);
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
    if(strcmp(variable->val_type.variable_def_value_type,"boost::any")!=0)
    {
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
                if(strcmp(VisitorGetVariableType(visitor,scope,node->tree_child->name.variable_name),type)!=0)
                {
                    diff_type=1;
                }
                break;
            }

            default: printf("Unknown Variable Type: %s\n",ASTreeTypeToString(node->tree_child->type)); exit(1); return NULL;
        }
    }

    char* variable_value;

    variable_value=VisitorTraverseNode(visitor,node->tree_child);

    if(diff_type)
    {
        printf("Variable %s cannot be assigned %s\n",variable_name,variable_value);
        exit(1);
        return NULL;
    }

    char* variable_str=calloc(strlen(variable_name)+strlen(variable_value)+5,1);

    sprintf(variable_str,"%s = %s",variable_name,variable_value);

    printf("%s\n",variable_str);

    return variable_str;
}

char* VisitorTraverseMultiVariableDeclaration(VisitorType* visitor, ASTreeType* node)
{
    char* statement = calloc(1,1);
    char* declaration;

    for(Int i=1;i<node->RootValue->size+1;i++)
    {
        if(i>node->RootValue->size)
            return "";

        declaration=VisitorTraverseVariableDeclaration(visitor,node->RootValue->trees[i]);
        statement=realloc(statement,strlen(statement)+strlen(declaration)+2);
        strcat(statement,declaration);
        strcat(statement,"\n");
        free(declaration);
    }

    return statement;
}

char* VisitorTraverseFunctionCall(VisitorType* visitor, ASTreeType* node)
{
    char* name=node->name.function_call_name;

    // Checking if the function is a default function

    if(VisitorIsSTDFunction(name))
    {
        if(strcmp(name,"echo")==0)
            return VisitorTraverseEchoCall(visitor,node);
    }
    else
    {
        // if it is not a default function
        // check if it is a user defined function
        if(!VisitorFunctionDeclared(visitor, visitor->scope.global,name))
        {
            printf("Function %s not declared\n",name);
            exit(1);
            return NULL;
        }

        char* args=VisitorTraverseParenthesis(visitor,node,node->args.function_call_arguments,",");
        char* code=calloc(strlen(args)+strlen(name)+4,1);
        sprintf(code,"%s(%s)",name,args);

        return code;
    }
    return "";
}

char* VisitorTraverseFunctionDeclaration(VisitorType* visitor, ASTreeType* node)
{
    char* name=node->name.function_def_name;
    char* type;
    char* args=calloc(1,1);
    char* returntype;
    ASTreeType* current;

    // Return type
    if(node->val_type.function_def_return_type==NULL)
    {
        for(Int i=1;i<node->funcbody->RootValue->size+1;i++)
        {
            if(i>node->funcbody->RootValue->size)
            {
                visitor->memory.std_includes.boost_any=1;
                returntype="boost::any";
                break;
            }
            current=node->funcbody->RootValue->trees[i];
            if(current->type==AST_RETURN)
            {
                if(strcmp(name,"main")==0)
                {
                    returntype="int";
                    node->val_type.function_def_return_type="int";
                    break;
                }

                visitor->memory.std_includes.boost_any=1;
                returntype="boost::any";
                break;
            }
        }

        returntype="void";
    }
    else
    {
        if(strcmp(node->val_type.variable_def_value_type,"any")==0)
            returntype="boost::any";
        else if(strcmp(node->val_type.variable_def_value_type,"str")==0)
            returntype="std::string";
        else
            returntype=node->val_type.function_def_return_type;
    }

    // Arguments
    visitor->scope.current_scope='f';
    for(Int i=1;i<node->args.function_def_arguments->size+1;i++)
    {
        if(i>node->args.function_def_arguments->size)
            break;

        current=node->args.function_def_arguments->trees[i];
        
        if(current->type!=AST_VARIABLE)
        {
            printf("Function %s cannot have definition with literal/constant arguments\n",name);
            exit(1);
            return NULL;
        }

        if(current->val_type.variable_def_value_type==NULL)
        {
            type="auto";
        }
        else if(strcmp(current->val_type.variable_def_value_type,"any")==0)
        {
            visitor->memory.std_includes.boost_any=1;
            type="boost::any";
        }
        else if(strcmp(current->val_type.variable_def_value_type,"str")==0)
        {
            visitor->memory.std_includes.boost_any=1;
            type="std::string";
        }
        else
        {
            type=current->val_type.variable_def_value_type;
        }

        current->val_type.variable_def_value_type=type;
        node->args.function_def_arguments->trees[i]=current;
        AppendNodeArray(visitor->scope.func_local,current);

        args=realloc(args,strlen(args)+strlen(current->name.variable_name)+strlen(type)+2);
        strcat(args,type);
        strcat(args," ");
        strcat(args,current->name.variable_name);
        
        if(i!=node->args.function_def_arguments->size)
        {
            args=realloc(args,strlen(args)+3);
            strcat(args,", ");
        }
    }

    visitor->memory.current=node;

    char* block=VisitorTraverseRoot(visitor,node->funcbody,1);

    returntype=visitor->memory.current->val_type.function_def_return_type;

    char* code=calloc(1,1);
    code=realloc(code,strlen(name)+strlen(args)+strlen(block)+strlen(returntype)+10+1);
    sprintf(code,"%s %s(%s)\n{\n%s}\n",returntype,name,args,block);

    visitor->scope.current_scope='g';
    DestroyASTreeArray(visitor->scope.func_local);
    visitor->scope.func_local=InitNodeArray(sizeof(struct ASTreeStructure));

    if(visitor->scope.current_scope=='g')
    {
        AppendNodeArray(visitor->scope.global,node);
    }
    else if(visitor->scope.current_scope=='c')
    {
        AppendNodeArray(visitor->scope.class_local,node);
    }

    return code;
}

char* VisitorTraverseReturn(VisitorType* visitor, ASTreeType* node)
{
    char* returntype;
    char* code;
    char errorcode=0;

    ASTreeType* func=visitor->memory.current;

    if(node->RootValue->size>1)
    {
        printf("Multiple value return statement has not been implemented yet\n");
        exit(1);
        return NULL;
    }
    else if(node->RootValue->size==0)
    {
        if(strcmp(func->val_type.function_def_return_type,"void")!=0)
        {
            printf("Return statement without value not allowed with return type %s\n",func->val_type.function_def_return_type);
            exit(1);
            return NULL;
        }
        returntype="void";
        func->val_type.function_def_return_type="void";
        visitor->memory.current=func;

        code=calloc(strlen("return")+1,1);
        strcpy(code,"return");
        return code;
    }

    if(func->val_type.function_def_return_type==NULL)
        returntype="boost::any";
    else
        returntype=func->val_type.function_def_return_type;

    // return type checking
    if(strcmp(returntype,"any")!=0)
    {
        if(strcmp(returntype,"void")==0)
        {
            if(node->RootValue->size!=0)
            {
                printf("Void function cannot return a value\n");
                exit(1);
                return NULL;
            }
            else
            {
                code=calloc(strlen("return")+1,1);
                strcpy(code,"return");
                return code;
            }
        }
        else if(strcmp(returntype,"auto")==0)
        {
            switch(node->RootValue->trees[1]->type)
            {
                case AST_INTEGER: returntype="int"; break;
                case AST_FLOAT: returntype="float"; break;
                case AST_STRING: returntype="std::string"; break;
                case AST_BOOL: returntype="bool"; break;
                case AST_CHARACTER: returntype="char"; break;

                default:
                {   
                    if(strcmp(VisitorGetVariableType(visitor, func->args.function_def_arguments, node->RootValue->trees[1]->name.variable_name),"boost::any")==0)
                    {
                        returntype="boost::any";
                        break;
                    }
                    else if(strcmp(VisitorGetVariableType(visitor, func->args.function_def_arguments, node->RootValue->trees[1]->name.variable_name),"std::string")==0)
                    {
                        returntype="std::string";
                        break;
                    }
                    else if(strcmp(VisitorGetVariableType(visitor, func->args.function_def_arguments, node->RootValue->trees[1]->name.variable_name),"auto")==0)
                    {
                        returntype="auto";
                        break;
                    }
                    else
                    {
                        char* type=VisitorGetVariableType(visitor, func->args.function_def_arguments, node->RootValue->trees[1]->name.variable_name);
                        if(strcmp(type,"int")==0)
                        {
                            returntype="int";
                            break;
                        }
                        else if(strcmp(type,"float")==0)
                        {
                            returntype="float";
                            break;
                        }
                        else if(strcmp(type,"bool")==0)
                        {
                            returntype="bool";
                            break;
                        }
                        else if(strcmp(type,"char")==0)
                        {
                            returntype="char";
                            break;
                        }
                        else
                        {
                            printf("Return type %s is not supported\n",type);
                            exit(1);
                            return NULL;
                        }  
                    }
                }
            }
        }
        else if(strcmp(returntype,"str")==0)
        {
            if(node->RootValue->trees[1]->type!=AST_STRING)
                errorcode=1;
            returntype = "std::string";
        }
        else if(strcmp(returntype,"int")==0)
        {
            if(node->RootValue->trees[1]->type!=AST_INTEGER)
                errorcode=1;
        }
        else if(strcmp(returntype,"bool")==0)
        {
            if(node->RootValue->trees[1]->type!=AST_BOOL)
                errorcode=1;
        }
        else
        {
            printf("Return type %s not implemented\n",returntype);
            exit(1);
            return NULL;
        }
    }
    else
    {
        returntype="boost::any";
    }

    if(errorcode==1)
    {
        printf("Return type %s cannot be returned by function %s with type %s\n",
               node->RootValue->trees[1]->val_type.variable_def_value_type,
               func->name.function_def_name, 
               returntype);
        exit(1);
        return NULL;
    }

    func->val_type.function_def_return_type=returntype;
    visitor->memory.current=func;
    
    if(node->RootValue->trees[1]->type==AST_VARIABLE)
    {
        code=calloc(strlen("return ")+strlen(node->RootValue->trees[1]->name.variable_name)+1,1);
        sprintf(code,"return %s",node->RootValue->trees[1]->name.variable_name);
    }
    else
    {
        char* value=VisitorTraverseNode(visitor,node->RootValue->trees[1]);
        code=calloc(strlen("return ")+strlen(value)+1,1);
        sprintf(code,"return %s",value);
    }

    return code;
}

char* VisitorTraverseParenthesis(VisitorType* visitor, ASTreeType* node, NodeArrayType* arr, char* sep)
{
    char* code=calloc(1,1);
    Int sep_size=strlen(sep);
    char check=0;

    if(VisitorIsSTDFunction(node->name.function_call_name)==0)
        check=1;

    NodeArrayType* scope;

    switch(visitor->scope.current_scope)
    {
        case 'g': scope=visitor->scope.global; break;
        case 'c': scope=visitor->scope.class_local; break;
        default: scope=visitor->scope.func_local; break;
    }

    ASTreeType* variable;
    ASTreeType* function;

    for(Int i=1;i<arr->size+1;i++)
    {
        if(i>arr->size)
            break;

        if(check)
        {
            function=VisitorGetFunction(visitor, scope, node->name.function_call_name);
            variable=VisitorGetVariable(visitor, scope, arr->trees[i]->name.variable_name);
            if(VisitorCheckFunctionArgType(visitor, function, variable->val_type.variable_def_value_type,i)==0)
            {
                printf("Invalid function argument type\n");
                exit(1);
                return NULL;
            }
        }
        char* statement=VisitorTraverseNode(visitor,arr->trees[i]);
        code=realloc(code,strlen(code)+strlen(statement)+sep_size+2);
        strcat(code,statement);
        
        if(i!=arr->size)
            strcat(code,sep);

        free(statement);
    }

    return code;
}

char* VisitorTraverseEchoCall(VisitorType* visitor, ASTreeType* node)
{
    visitor->memory.std_includes.iostream=1;
    char* code=calloc(14,1);
    strcat(code,"std::cout << ");

    char* paren=VisitorTraverseParenthesis(visitor,node,node->args.function_call_arguments," << \" \" << ");
    code=realloc(code,strlen(code)+strlen(paren)+16);
    strcat(code,paren);
    strcat(code," << std::endl");

    free(paren);

    return code;
}

#endif