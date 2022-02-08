#ifndef ASTARRAYS
#define ASTARRAYS

#include "Integers.h"
#include "AltarASTree.h"

typedef struct NodeArrayStructure
{
    void** trees;
    Int size;
    Int tree_size;

    char id;
}NodeArrayType;

// Initialization
NodeArrayType* InitNodeArray(SInt tree_size);

// Voids
void AppendNodeArray(NodeArrayType* Array, void* itm);

// Destruction
void DestroyNodeArray(NodeArrayType* Array);
void DestroyASTreeArray(NodeArrayType* Array);

#endif