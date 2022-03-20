#ifndef NODEARRC
#define NODEARRC

#include <stdio.h>

#include "Headers/NodeArray.h"
#include "Headers/Integers.h"
#include "Headers/AltarASTree.h"

#include "AltarASTree.c"

// -- Initialization --
NodeArrayType* InitNodeArray(SInt tree_size)
{
    NodeArrayType* Array=calloc(1,sizeof(struct NodeArrayStructure));
    Array->size=0;
    Array->trees=0;
    Array->tree_size=tree_size;

    return Array;
}

// -- ASTreeTypes --
ASTreeType* GetNodeArray(NodeArrayType* Array, SInt index)
{
    if(index>=Array->size)
    {
        printf("Error: NodeArray index out of bounds.\n");
        return 0;
    }

    return Array->trees[index];
}

// -- Voids --
void AppendNodeArray(NodeArrayType* Array, ASTreeType* itm)
{
    ++Array->size;

    if(!Array->trees)
    {
        Array->trees=calloc(1,Array->tree_size);
    }
    else
    {
        Array->trees=realloc(Array->trees,Array->tree_size*Array->size);
    }

    Array->trees[Array->size]=itm;
}

ASTreeType* PopNodeArray(NodeArrayType* Array)
{
    if(Array->size<=0)
    {
        printf("Error: NodeArray is empty.\n");
        return 0;
    }

    ASTreeType* itm=Array->trees[Array->size];
    --Array->size;

    if(Array->size==0)
    {
        free(Array->trees);
        Array->trees=0;
    }
    else
    {
        Array->trees=realloc(Array->trees,Array->tree_size*Array->size);
    }

    return itm;
}

// -- Destruction --
void ClearNodeArray(NodeArrayType* Array)
{
    if(Array->trees)
    {
        while(Array->size>0)
        {
            DestroyASTree(PopNodeArray(Array));
        }
    }

    Array->size=0;
}

void DestroyNodeArray(NodeArrayType* Array)
{
    for(Int i=0;i<Array->size;++i)
    {
        DestroyASTree(Array->trees[i]);
    }
    free(Array);
}

void DestroyASTreeArray(NodeArrayType* Array)
{
    for(Int i=1;i<Array->size+1;++i)
    {
        DestroyASTree(Array->trees[i]);
    }
}

#endif