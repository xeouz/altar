#ifndef ASTARRAYS
#define ASTARRAYS

#include "Integers.h"
#include "AltarASTree.h"

typedef struct NodeArrayStructure
{
    ASTreeType** trees;
    Int size;
    Int tree_size;

    char id;
}NodeArrayType;

// Initialization
NodeArrayType* InitNodeArray(SInt tree_size);

// Voids
void AppendNodeArray(NodeArrayType* Array, ASTreeType* itm);

// ASTreeTypes
ASTreeType* GetNodeArray(NodeArrayType* Array, SInt index);

// Destruction
void DestroyNodeArray(NodeArrayType* Array);
void DestroyASTreeArray(NodeArrayType* Array);

#endif