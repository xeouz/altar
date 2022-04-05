#ifndef ARITHH
#define ARITHH

#include <stdio.h>  

#include "NodeArray.h"
#include "AltarASTree.h"

int ArithIsOprnd(ASTreeType* ch);
int ArithGetPrecendence(ASTreeType* op1, ASTreeType* op2);

char ArithisEmpty(NodeArrayType* s);

ASTreeType* ArithGetTop(NodeArrayType* s);
NodeArrayType* ArithPostix(NodeArrayType* infix);

int ArithIsOprnd(ASTreeType* ch)
{
   if(
        ch->type==AST_ADD ||
        ch->type==AST_SUB ||
        ch->type==AST_MUL ||
        ch->type==AST_DIV ||
        ch->type==AST_MOD
   )
   {
      return 0;
   }
   else
   {
      return 1;
   }
}

char ArithIsEmpty(NodeArrayType* s)
{
    if (s->size==0)
        return 1;
    else
        return 0;
}

ASTreeType* ArithGetTop(NodeArrayType* s)
{
    return s->trees[s->size];
}

int ArithGetPrecendence(ASTreeType* op1, ASTreeType* op2)
{
    if(op2->type==AST_MUL || op2->type==AST_DIV || op2->type==AST_MOD)
    {
        return 0;
    }
    else if(op1->type==AST_MUL || op1->type==AST_DIV || op1->type==AST_MOD)
    {
        return 1;
    }
    else if(op2->type==AST_ADD || op2->type==AST_ADD)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

NodeArrayType* ArithPostfix(NodeArrayType* infix)
{
    NodeArrayType* postfix = InitNodeArray(sizeof(struct ASTreeStructure));
    NodeArrayType* stack = InitNodeArray(sizeof(struct ASTreeStructure));

    Int length=infix->size+1;

    ASTreeType* Current;
    ASTreeType* Temp;

    for(Int i=1;i<length;++i)
    {
        Current=infix->trees[i];

        if(ArithIsOprnd(Current)==1)
        {
            AppendNodeArray(postfix,Current);
        }
        else
        {
            if(Current->type==AST_ARITHPARENTHESIS && Current->value.char_value=='(')
            {
                AppendNodeArray(stack,Current);
            }
            else
            {
                if(Current->type==AST_ARITHPARENTHESIS && Current->value.char_value==')')
                {
                    while((Temp=PopNodeArray(stack))->value.char_value!='(')
                    {
                        AppendNodeArray(postfix,Temp);
                    }
                }
                else
                {
                    while(ArithIsEmpty(stack)==0)
                    {
                        if(ArithGetPrecendence(Current,ArithGetTop(stack))==1)
                        {
                            break;
                        }
                        AppendNodeArray(postfix,PopNodeArray(stack));
                    }
                    AppendNodeArray(stack,Current);
                }
            }
        }
    }

    while(ArithIsEmpty(stack)==0)
    {
        AppendNodeArray(postfix,PopNodeArray(stack));
    }

    DestroyASTreeArray(stack);
    return postfix;
}
#endif