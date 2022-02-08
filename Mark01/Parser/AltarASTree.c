#ifndef ASTREEC
#define ASTREEC

#include "Headers/AltarASTree.h"
#include "NodeArray.c"
#include "Headers/Integers.h"

// -- Initialization --
ASTreeType* InitASTree(char type)
{
    ASTreeType* AST=calloc(1,sizeof(struct ASTreeStructure));
    AST->type=type;

    return AST;
}

// -- Destruction --
void DestroyASTree(ASTreeType* AST)
{
    if(AST->variable_def_value)
    {
        printf("eHy\n");
        DestroyASTree(AST->variable_def_value);
    }
    if(AST->RootValue)
    {
        FreeASTreeArray(AST->RootValue);
    }

    free(AST);
}

#endif