#include <stdio.h>
#include <stdlib.h>

#include "Headers/AltarVisitor.h"
#include "AltarVisitor.c"

#include "../Parser/IncludeLinker.h"

int main()
{
    printf("Initializing Parser\n\n");

    // Create Altar Parser/Lexer

    IOFile* file;
    LexerType* lexer;
    ParserType* parser;
    ASTreeType* root;

    file=InitIO("../Mark01/SyntaxTests/Test.altr");
    IOReadFile(file);

    lexer=InitLexer(file->data);

    parser=InitParser(lexer);

    root=ParserParseRoot(parser);

    printf("Root Size: %d\n",root->RootValue->size);

    /* Destroy Lexer and Parser
     * Currently encounters errors, dangerous!

    DestroyASTree(root);
    DestroyParser(parser);
    DestroyFile(file);

    */

    VisitorType* visitor=InitVisitor();
    VisitorTraverseRoot(visitor,root);  // This is the main function that will be called by the compiler

    return 0;
}