#include <stdio.h>

#include "AltarParser.c"
#include "IO.c"

#include "Headers/Integers.h"
#include "Headers/AltarTokens.h"

int main(){
    printf("Running...\n\n");
	IOFile* file=InitIO("./Compiler/Syntax/Test.altr");
	IOReadFile(file);
	
	LexerType* lexer=InitLexer(file->data);

	printf("File Preview\n...\n\n%s\n...\n\n",lexer->src);

	ParserType* parser=InitParser(lexer);
	ASTreeType* root=ParserParseRoot(parser);

	printf("Root Size (in Bytes) - %zu\n",sizeof(root));

	printf("Root Type - %d",root->RootValue->size);


	printf("\n\n");

	return 0;
}
