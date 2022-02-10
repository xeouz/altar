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
    if(AST->tree_child)
    {
        printf("eHy\n");
        DestroyASTree(AST->tree_child);
    }
    if(AST->RootValue)
    {
        FreeASTreeArray(AST->RootValue);
    }

    free(AST);
}

#endif