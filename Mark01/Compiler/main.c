#include <stdio.h>
#include <stdlib.h>

#include "Headers/AltarVisitor.h"

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

    //DestroyASTree(root);
    //DestroyParser(parser);
    //DestroyFile(file);

    return 0;
}