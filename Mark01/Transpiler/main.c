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

    lexer=InitLexer(file->data, file->src);

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
    char* code=VisitorTraverseRoot(visitor,root,1);  // This is the main function that will be called by the compiler
    printf("\n--- Code ---\n\n%s\n",code);

    return 0;
}