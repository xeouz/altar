#include <stdio.h>
#include <stdlib.h>

#include "Headers/AltarVisitor.h"

#include "../Parser/IncludeLinker.h"

int main()
{
    printf("Initlializing Parser\n\n");

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

    printf("Root Size (Bytes): %zu\n",sizeof(root));

    //DestroyASTree(root);
    //DestroyParser(parser);
    //DestroyFile(file);
}