#ifndef VISITORC
#define VISITORC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Headers/AltarVisitor.h"
#include "../Parser/IncludeLinker.h"

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
    visitor->memory.indent=0;
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
            return NULL;
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
            if(i>scope->size)
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

NodeArrayType* VisitorGetScope(VisitorType* visitor)
{
    NodeArrayType* scope;

    switch(visitor->scope.current_scope)
    {
        case 'c':
            scope=visitor->scope.class_local;
            break;
        
        case 'f':
            scope=visitor->scope.func_local;
            break;

        default: 
            scope=visitor->scope.global;
            break;
    }

    return scope;
}

char* VisitorGetArrayType(VisitorType* visitor, ASTreeType* node)
{
    char* type="";

    if(node->RootValue->size==0)
        return "";

    type=VisitorGetType(visitor,node->RootValue->trees[1]);
    char* current_type;

    for(Int i=1;i<node->RootValue->size+1;++i)
    {
        current_type=VisitorGetType(visitor,node->RootValue->trees[i]);

        if(strcmp(type,current_type)==0)
        {
            type=current_type;
        }
        else
        {
            type="boost::any";
        }
    }

    return type;
}

char* VisitorGetType(VisitorType* visitor, ASTreeType* node)
{
    switch(node->type)
    {
        case AST_INTEGER: return "int";
        case AST_FLOAT: return "float";
        case AST_STRING: return "std::string";
        case AST_BOOL: return "bool";
        case AST_CHARACTER: return "char";

        case AST_ADD: return VisitorGetType(visitor,node->arithleft);
        case AST_SUB: return VisitorGetType(visitor,node->arithleft);
        case AST_MUL: return VisitorGetType(visitor,node->arithleft);
        case AST_DIV: return VisitorGetType(visitor,node->arithleft);
        case AST_MOD: return VisitorGetType(visitor,node->arithleft);
        case AST_ARITHPARENTHESIS: return VisitorGetType(visitor,node->tree_child->arithleft);

        case AST_ARRAY: return VisitorGetArrayType(visitor,node);

        case AST_FUNCTION_CALL: {
            NodeArrayType* scope=VisitorGetScope(visitor);

            ASTreeType* func = VisitorGetFunction(visitor,scope,node->name.function_call_name);
            if(func==NULL)
            {
                return "";
            }
            else
            {
                return func->val_type.function_def_return_type;
            }
        }

        case AST_VARIABLE: {
            NodeArrayType* scope;
            switch(visitor->scope.current_scope)
            {
                case 'c': scope = visitor->scope.class_local; break;
                case 'f': scope = visitor->scope.func_local; break;
                default: scope = visitor->scope.global; break;
            }

            return VisitorGetVariableType(visitor,scope,node->name.variable_name);
        }
    
        default:return "";
    }
}

char* VisitorGetConditionOperator(USInt op)
{
    switch(op)
    {
        case TOKEN_CHKEQUALS: return "==";
        case TOKEN_NOTEQUALS: return "!=";
        case TOKEN_LANGLE: return "<=";
        case TOKEN_RANGLE: return ">=";
        case TOKEN_LANGB: return "<";
        case TOKEN_RANGB: return ">";
        case TOKEN_ANDOP: return "&&";
        case TOKEN_OROP: return "||";

        default: return "";
    }
}

char* VisitorPurifyType(char* type)
{
    if(strcmp(type,"std::string")==0)
        return "str";
    else if(strcmp(type,"boost::any")==0)
        return "any";
    else 
        return type;
}

// ---

char* VisitorTraverseRoot(VisitorType* visitor, ASTreeType* root, char isHead)
{   
    // Traversing every node
    char* code=calloc(1,1);
    char* line;
    for(Int i=1;i<root->RootValue->size+1;++i)
    {
        if(i>root->RootValue->size)
            break;

        if(root->RootValue->trees[i]->type==AST_ENDL)
            break;

        line=VisitorTraverseNode(visitor,root->RootValue->trees[i]);
        code=realloc(code,strlen(code)+strlen(line)+(isHead?0:2)+2);

        if(!isHead)
            strcat(code,"\t");

        strcat(code,line);

        if(root->RootValue->trees[i]->type!=AST_FUNCTION_DECLARATION 
        && root->RootValue->trees[i]->type!=AST_CLASS_DECLARATION
        && root->RootValue->trees[i]->type!=AST_IF)
        {
            code=realloc(code,(strlen(code)+2));
            strcat(code,";");
        }
        code=realloc(code,(strlen(code)+2));
        strcat(code,"\n");
        
        free(line);
    }

    if(isHead)
    {
        char* includes=VisitorTraverseIncludes(visitor);
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

    code=realloc(code,strlen(code)+39);
    strcat(code,"using namespace std::string_literals;\n");

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

        case AST_IF: return VisitorTraverseIf(visitor,node);

        case AST_ARRAY: return VisitorTraverseArray(visitor,node);

        case AST_MEMBER_ACCESS: return VisitorTraverseMemberAccess(visitor,node);
        case AST_BLOCK_ACCESS: return VisitorTraverseBlockAccess(visitor,node);

        case AST_ADD: return VisitorTraverseArithmetic(visitor,node);
        case AST_SUB: return VisitorTraverseArithmetic(visitor,node);
        case AST_MUL: return VisitorTraverseArithmetic(visitor,node);
        case AST_DIV: return VisitorTraverseArithmetic(visitor,node);
        case AST_MOD: return VisitorTraverseArithmetic(visitor,node);

        case AST_ARITHPARENTHESIS: {
            char* code=calloc(1,1);
            strcat(code,"(");
            strcat(code,VisitorTraverseNode(visitor,node->tree_child));
            strcat(code,")");
            return code;
        }

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
            char* code=calloc(strlen(node->value.string_value)+4,1);
            sprintf(code,"\"%s\"s",node->value.string_value);
            return code;
        }

        case AST_CHKEQ: return VisitorTraverseConditions(visitor,node);
        case AST_CHKNEQ: return VisitorTraverseConditions(visitor,node);
        case AST_CHKGREATER: return VisitorTraverseConditions(visitor,node);
        case AST_CHKLESS: return VisitorTraverseConditions(visitor,node);
        case AST_CHKGREATEREQ: return VisitorTraverseConditions(visitor,node);
        case AST_CHKLESSEQ: return VisitorTraverseConditions(visitor,node);

        default: printf("Unknown Node Type: %s:%d\n",ASTreeTypeToString(node->type),node->type); exit(1); return NULL;
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
    NodeArrayType* scope=VisitorGetScope(visitor);

    // Check for the scope and assign the current scope

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
            variable_type=VisitorGetType(visitor, node->tree_child);
        }
    }
    else
    {
        if(strcmp(node->val_type.variable_def_value_type,"any")==0)
        {
            printf("Cannot use type `any` as a variable type for now\n");
            exit(1);
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
        if(strcmp(VisitorPurifyType(variable_type),VisitorPurifyType(VisitorGetType(visitor,node->tree_child)))!=0)
        {
            printf("Variable `%s` declared with type `%s` but assigned with type `%s`\n",variable_name,VisitorPurifyType(variable_type),VisitorPurifyType(VisitorGetType(visitor,node->tree_child)));
            exit(1);
        }
        variable_value=VisitorTraverseNode(visitor,node->tree_child);
    }

    // - Variable 

    /* Apply the corresponding format to the variable

     * type name = value;

     * sprintf() will format the string 
     * and assign it to the variable
    */
    //sprintf(variable,"%s %s = %s",variable_type,variable_name,variable_value);

    char* addname=calloc(1,1);

    if(node->tree_child!=NULL && node->tree_child->type==AST_ARRAY)
    {
        addname=realloc(addname,strlen(variable_name)+2);
        sprintf(addname,"%s[]",variable_name);
    }
    else
    {
        addname=realloc(addname,strlen(variable_name)+1);
        sprintf(addname,"%s",variable_name);
    }

    variable=calloc(strlen(variable_type)+strlen(addname)+strlen(variable_value)+6,1);
    if(explicit_value)
    {
        sprintf(variable,"%s %s = %s",variable_type,addname,variable_value);
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

    char* variable_name="";
    char* add_name="";

    variable_name=node->name.variable_name;

    if(node->blockaccess!=NULL)
    {
        add_name=VisitorTraverseBlockAccess(visitor,node->blockaccess);
    }
    else
    {
        add_name=variable_name;
    }

    ASTreeType* variable=VisitorGetVariable(visitor,scope,variable_name);

    char diff_type=0;

    // Checking definition type
    if(strcmp(VisitorPurifyType(variable->val_type.variable_def_value_type),VisitorPurifyType(VisitorGetType(visitor,node->tree_child)))==0)
    {
        diff_type=0;
    }
    else
    {
        diff_type=1;
    }

    char* variable_value="";

    variable_value=VisitorTraverseNode(visitor,node->tree_child);

    if(diff_type)
    {
        printf("Variable %s cannot be assigned %s\n",variable_name,variable_value);
        exit(1);
        return NULL;
    }

    char* variable_str=calloc(strlen(add_name)+strlen(variable_value)+5,1);

    sprintf(variable_str,"%s = %s",add_name,variable_value);

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
        if(!VisitorFunctionDeclared(visitor, visitor->scope.global,name) && !VisitorFunctionDeclared(visitor, VisitorGetScope(visitor),name))
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
    char* returntype="";
    ASTreeType* current;

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
            visitor->memory.std_includes.string=1;
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

    ++visitor->memory.indent;

    // Body
    char* block=VisitorTraverseRoot(visitor,node->funcbody, 0);
    --visitor->memory.indent;

    // Return type
    if(node->val_type.function_def_return_type==NULL)
    {
        if(strcmp(name,"main")==0)
        {
            returntype="int";
            node->val_type.function_def_return_type="int";
        }
        else
        {
            for(Int i=1;i<node->funcbody->RootValue->size+1;i++)
            {
                if(i>node->funcbody->RootValue->size)
                {
                    returntype="void";
                    break;
                }
                current=node->funcbody->RootValue->trees[i];
                if(current->type==AST_RETURN)
                {
                    visitor->memory.std_includes.boost_any=1;
                    returntype=VisitorGetType(visitor,current->RootValue->trees[1]);
                    break;
                }
            }
        }
    }
    else
    {
        if(strcmp(node->val_type.function_def_return_type,"any")==0)
            returntype="boost::any";
        else if(strcmp(node->val_type.function_def_return_type,"str")==0)
            returntype="std::string";
        else
        {
            returntype=node->val_type.function_def_return_type;
        }
    }

    if(strcmp(returntype,"")==0)
    {
        returntype="void";
    }
    else
    {
        //printf("%s\n",returntype);
    }

    char* code=calloc(1,1);
    code=realloc(code,strlen(name)+strlen(args)+strlen(block)+strlen(returntype)+visitor->memory.indent+7+1);
    sprintf(code,"%s %s(%s){\n%s",returntype,name,args,block);

    for(Int i=0;i<visitor->memory.indent;++i)
        strcat(code,"\t");
    strcat(code,"}");

    node->val_type.function_def_return_type=returntype;
    
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
    char typeisany=0;

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
    {
        returntype=VisitorPurifyType(VisitorGetType(visitor,node->RootValue->trees[1]));
    }
    else
    {
        returntype=func->val_type.function_def_return_type;
    }

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
            returntype=VisitorGetType(visitor, node->RootValue->trees[1]);
        }
        else if(strcmp(returntype,"str")==0)
        {
            returntype = "std::string";
        }
        else if(strcmp(returntype,"int")==0)
        {
            returntype = "int";
        }
        else if(strcmp(returntype,"bool")==0)
        {
            returntype = "bool";
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
        typeisany=1;
        returntype="boost::any"; 
    }

    if(strcmp(returntype,VisitorGetType(visitor,node->RootValue->trees[1]))!=0)
    {
        errorcode=1;
    }

    if(errorcode==1)
    {
        char* variable_type=VisitorGetType(visitor, node->RootValue->trees[1]);
        printf("Type `%s` cannot be returned by function `%s` with return type `%s`\n",
               VisitorPurifyType(variable_type),
               func->name.function_def_name, 
               VisitorPurifyType(returntype));
        exit(1);
        return NULL;
    }

    func->val_type.function_def_return_type=returntype;
    visitor->memory.current=func;
    
    if(node->RootValue->trees[1]->type==AST_VARIABLE)
    {
        if(typeisany)
        {
            if(strcmp(returntype,VisitorGetType(visitor,node->RootValue->trees[1]))!=0)
            {
                printf("Can only return a variable with type `any` from function `%s` with return type `any`\n",func->name.function_def_name);
                exit(1);
                return NULL;
            }
        }
        code=calloc(strlen("return ")+strlen(node->RootValue->trees[1]->name.variable_name)+1,1);
        sprintf(code,"return %s",node->RootValue->trees[1]->name.variable_name);
    }
    else
    {
        if(typeisany)
        {
            printf("Can only return a variable from return type `any` in function `%s`\n",func->name.function_def_name);
            exit(1);
            return NULL;
        }
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

    scope=VisitorGetScope(visitor);

    ASTreeType* variable;

    for(Int i=1;i<arr->size+1;i++)
    {
        if(i>arr->size)
            break;

        if(check)
        {
            //function=VisitorGetFunction(visitor, scope, node->name.function_call_name);
            char* type;
            if(arr->trees[i]->type==AST_VARIABLE)
            {
                variable=VisitorGetVariable(visitor, scope, arr->trees[i]->name.variable_name);
                type=variable->val_type.variable_def_value_type;
            }
            else
            {
                type=VisitorGetType(visitor, arr->trees[i]);
            }
            if(strcmp(VisitorGetType(visitor,arr->trees[i]),type)!=0)
            {
                printf("Invalid function argument type %s, needs %s\n",type, arr->trees[i]->val_type.variable_def_value_type);
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

char* VisitorTraverseConditions(VisitorType* visitor, ASTreeType* node)
{
    ASTreeType* first;
    ASTreeType* second;

    char* code=calloc(1,1);

    if(strcmp(VisitorGetType(visitor,node->RootValue->trees[1]),VisitorGetType(visitor,node->RootValue->trees[2]))!=0)
    {
        printf("Type mismatch in condition\n");
        exit(1);
        return NULL;
    }

    first=node->RootValue->trees[1];
    second=node->RootValue->trees[2];

    char* first_code=VisitorTraverseNode(visitor,first);
    char* second_code=VisitorTraverseNode(visitor,second);

    char* operator;
    switch (node->type)
    {
        case AST_MOD: operator="%"; break;
        case AST_CHKEQ: operator="==";break;
        case AST_CHKNEQ: operator="!=";break;
        case AST_CHKGREATER: operator=">";break;
        case AST_CHKLESS: operator="<";break;
        case AST_CHKGREATEREQ: operator=">=";break;
        case AST_CHKLESSEQ: operator="<=";break;

        default: printf("Condition type not implemented\n"); exit(1); break;
    }

    code=realloc(code,strlen(first_code)+strlen(second_code)+strlen(operator)+1);
    sprintf(code,"%s%s%s",first_code,operator,second_code);

    return code;
}

char* VisitorTraverseIf(VisitorType* visitor, ASTreeType* node)
{
    char* code=calloc(1,1);

    char* condition=VisitorTraverseNode(visitor,node->ifexpr);

    ++visitor->memory.indent;
    char* block=VisitorTraverseRoot(visitor,node->ifbody,0);
    --visitor->memory.indent;

    code=realloc(code,strlen("if(){}\n\t")+strlen(condition)+strlen(block)+(visitor->memory.indent)+2);
    sprintf(code,"if(%s){\n\t%s",condition,block);
    for(Int i=0;i<visitor->memory.indent;++i)
        strcat(code,"\t");
    strcat(code,"}");
    free(block);

    if(node->ifelse!=NULL)  // if it has an else-if 
    {
        char* elseifcode=calloc(1,1);
        char* elseifcode_if=VisitorTraverseIf(visitor,node->ifelse);
        elseifcode=realloc(elseifcode,strlen(elseifcode_if)+6);
        sprintf(elseifcode,"else %s",elseifcode_if);

        code=realloc(code,strlen(code)+strlen(elseifcode)+1);
        strcat(code,elseifcode);
        free(elseifcode_if);
        free(elseifcode);
    }

    if(node->elsebody!=NULL)
    {
        char* elsecode=calloc(1,1);
        ++visitor->memory.indent;
        char* elseblock=VisitorTraverseRoot(visitor,node->elsebody,0);
        --visitor->memory.indent;

        elsecode=realloc(elsecode,strlen(elseblock)+visitor->memory.indent+9);
        sprintf(elsecode,"else{\n\t%s",elseblock);
        for(Int i=0;i<visitor->memory.indent;++i)
            strcat(elsecode,"\t");
        strcat(elsecode,"}");
        free(elseblock);

        code=realloc(code,strlen(code)+strlen(elsecode)+1);
        strcat(code,elsecode);
        free(elsecode);
    }

    return code;
}

char* VisitorTraverseArray(VisitorType* visitor, ASTreeType* node)
{
    char* code=calloc(2,1);
    strcat(code,"{");
    
    for(Int i=1;i<node->RootValue->size+1;++i)
    {
        char* statement=VisitorTraverseNode(visitor,node->RootValue->trees[i]);
        code=realloc(code,strlen(code)+strlen(statement)+1);
        strcat(code,statement);

        if(i!=node->RootValue->size)
        {
            code=realloc(code,strlen(code)+2);
            strcat(code,",");
        }

        free(statement);
    }

    code=realloc(code,strlen(code)+2);
    strcat(code,"}");

    return code;
}

char* VisitorTraverseMemberAccess(VisitorType* visitor, ASTreeType* node)
{
    if(node->tree_child->type!=AST_VARIABLE
    && node->tree_child->type!=AST_FUNCTION_CALL)
    {
        printf("Member access can only be done on variables and function calls\n");
        exit(1);
        return NULL;
    }

    char* code=calloc(1,1);

    char* head=VisitorTraverseNode(visitor,node->memaccess);
    char* tail=VisitorTraverseNode(visitor,node->tree_child);

    code=realloc(code,strlen(head)+strlen(tail)+2);
    strcat(code,head);
    strcat(code,".");
    strcat(code,tail);

    return code;
}

char* VisitorTraverseBlockAccess(VisitorType* visitor, ASTreeType* node)
{
    if(node->blockaccess->type!=AST_INTEGER && node->blockaccess->type!=AST_VARIABLE)
    {
        printf("Block access can only be done with integers, not on %s\n",ASTreeTypeToString(node->blockaccess->type));
        exit(1);
        return NULL;
    }

    ASTreeType* var=VisitorGetVariable(visitor,VisitorGetScope(visitor),node->name.variable_name);
    if(var->tree_child->type!=AST_ARRAY)
    {
        printf("Block access can only be done on arrays\n");
        exit(1);
        return NULL;
    }

    int intval=node->blockaccess->value.integer_value;

    char* intstr=calloc(intval==0?1:log10((double)intval)+2,1);
    sprintf(intstr,"%d",intval);

    char* varname=calloc(strlen(intstr)+strlen(node->name.variable_name)+2,1);
    sprintf(varname,"%s[%s]",node->name.variable_name,intstr);

    return varname;
}

char* VisitorTraverseArithmetic(VisitorType* visitor, ASTreeType* node)
{
    char* code=calloc(1,1);

    char* first=VisitorTraverseNode(visitor,node->arithleft);
    char* second=VisitorTraverseNode(visitor,node->arithright);

    if(node->arithleft->type==AST_ADD
    || node->arithleft->type==AST_SUB
    || node->arithleft->type==AST_MUL
    || node->arithleft->type==AST_DIV
    || node->arithleft->type==AST_MOD)
    {
        code=realloc(code,strlen(first)+3);
        strcat(code,"(");
        strcat(code,first);
        strcat(code,")");
    }
    else
    {
        code=realloc(code,strlen(first)+1);
        strcat(code,first);
    }

    code=realloc(code,strlen(code)+2);
    switch(node->type)
    {
        case AST_ADD: strcat(code,"+"); break;
        case AST_SUB: strcat(code,"-"); break;
        case AST_MUL: strcat(code,"*"); break;
        case AST_DIV: strcat(code,"/"); break;
        case AST_MOD: strcat(code,"%"); break;
        default: printf("Arithmetic type not implemented\n"); exit(1); break;
    }

    if(node->arithright->type==AST_ADD
    || node->arithright->type==AST_SUB
    || node->arithright->type==AST_MUL
    || node->arithright->type==AST_DIV
    || node->arithright->type==AST_MOD)
    {
        code=realloc(code,strlen(code)+strlen(second)+3);
        strcat(code,"(");
        strcat(code,second);
        strcat(code,")");
    }
    else
    {
        code=realloc(code,strlen(code)+strlen(second)+1);
        strcat(code,second);
    }

    free(first);
    free(second);

    return code;
}

#endif