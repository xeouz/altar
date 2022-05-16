#ifndef ALTARAPI
#define ALTARAPI

#include "../Parser/IncludeLinker.h"
#include "../Transpiler/IncludeLinker.h"

const char* compile(char* code)
{
    LexerType* lexer=InitLexer(code,"");
    ParserType* parser=InitParser(lexer);

    ASTreeType* root=ParserParseRoot(parser);

    DestroyLexer(lexer);
    DestroyParser(parser);

    VisitorType* visitor=InitVisitor();

    const char* result=VisitorTraverseRoot(visitor,root,1);

    DestroyASTree(root);
    
    return result;
}

#endif