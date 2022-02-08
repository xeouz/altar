#ifndef IO
#define IO

#include "Integers.h"

typedef struct FileStructure{
	char* src;
	char* data;
	Int strsz;
	Int filesz;
}IOFile;

// Initialization
IOFile* InitIOFile(char* srcname);

// Voids
void IOReadFile(IOFile* file);

// Destruction
void DestroyFile(IOFile* file);

#endif