#include <stdio.h>
#include <stdlib.h>

#include "Transpiler/Headers/AltarVisitor.h"
#include "Transpiler/AltarVisitor.c"

#include "Parser/IncludeLinker.h"

#include "API/AltarAPI.h"

int test()
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
    */

    DestroyLexer(lexer);
    DestroyParser(parser);
    DestroyFile(file);

    VisitorType* visitor=InitVisitor();
    char* code=VisitorTraverseRoot(visitor,root,1);  // This is the main function that will be called by the compiler
    
    DestroyASTree(root);

    printf("\n--- Code ---\n\n%s\n\n",code);

    return 0;
}

void cli_format(char* name)
{
    printf("\n\nFormat: %s (command) (filename)",name);
}

void cli_help()
{
    printf("Currently Available Commands:\n\trun: Runs the code from the specified file\n\temit: Prints C++ compiled code\n\tcompile: Writes the Compiled C++ code");
}

void cli_run(char* fp)
{
    IOFile* file=InitIO(fp);
    IOReadFile(file);

    LexerType* lexer=InitLexer(file->data,fp);
    ParserType* parser=InitParser(lexer);
    ASTreeType* root=ParserParseRoot(parser);

    DestroyParser(parser);
    
    DestroyFile(file);

    VisitorType* visitor=InitVisitor();
    char* code=VisitorTraverseRoot(visitor,root,1);
    char* dst=calloc(strlen(fp)+strlen(".cpp"),1);
    strcat(dst,fp);
    strcat(dst,".cpp");

    IOFile* out=InitNewIOFile(dst);
    out->data=code;
    out->filesz=strlen(code);
    SaveIOFile(out);
    DestroyFile(out);

    free(code);

    char* bin=calloc(strlen(fp)+strlen(".out"),1);
    strcat(bin,fp);
    strcat(bin,".out");

    char* cmd=calloc(4+strlen(dst)+4+strlen(bin)+5,1);

    sprintf(cmd,"g++ %s -o %s -O2",dst,bin);
    printf("> %s\n\n",cmd);

    system(cmd);

    remove(dst);
}

void cli_compile(char* fp)
{
    IOFile* file=InitIO(fp);
    IOReadFile(file);

    LexerType* lexer=InitLexer(file->data,fp);
    ParserType* parser=InitParser(lexer);
    ASTreeType* root=ParserParseRoot(parser);

    DestroyParser(parser);
    
    DestroyFile(file);

    VisitorType* visitor=InitVisitor();
    char* code=VisitorTraverseRoot(visitor,root,1);
    char* dst=calloc(strlen(fp)+strlen(".cpp"),1);
    strcat(dst,fp);
    strcat(dst,".cpp");

    IOFile* out=InitNewIOFile(dst);

    char* comments="//-//==========\n/*\nThis file has been generated with the legacy AltarC transpiler\nKindly refrain from using it. The new compiler is in development and is going to have a different name.\n\t-- Sole Developer, XeouzR0\n*/\n//-//==========\n\n";
    out->data=calloc(strlen(code)+strlen(comments),1);
    strcat(out->data,comments);
    strcat(out->data,code);

    out->filesz=strlen(out->data);
    SaveIOFile(out);
    DestroyFile(out);

    free(code);
}

int main(int argc, char* argv[])
{
    if(argc<2)
    {
        printf("Invalid arguments:\n\tLess than 2 Arguments");
        cli_format(argv[0]);
        printf("\n\n");
        exit(1);
    }

    if(strcmp(argv[1],"help")==0)
    {
        cli_help();
        printf("\n\n");
        exit(0);
    }

    if(argc<3)
    {
        printf("Invalid Arguments:\n\tThe commands `run` and `compile` require an argument specifying the filepath\n\n");
        exit(1);
    }

    if(strcmp(argv[1],"run")==0)
    {
        cli_run(argv[2]);
        exit(0);
    }

    else if(strcmp(argv[1],"compile")==0)
    {
        cli_compile(argv[2]);
        exit(0);
    }

    else
    {
        printf("Invalid command: `%s`\n",argv[1]);
        cli_format(argv[1]);
        printf("\n\n");
        exit(1);
    }

}