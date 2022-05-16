#ifndef IO
#define IO

#include "Integers.h"

typedef struct FileStructure{
	char* src;
	char* data;

	Int line;

	Int strsz;
	Int filesz;

	char writing;
	FILE* fileobj;
}IOFile;

// Initialization
IOFile* InitIOFile(char* srcname);
IOFile* InitNewIOFile(char* filename);

// Voids
void IOReadFile(IOFile* file);

void WriteIOFile(IOFile* file, char* data);
void SaveIOFile(IOFile* file);

// Destruction
void DestroyFile(IOFile* file);

#endif