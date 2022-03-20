#ifndef IOC
#define IOC

#include <stdlib.h>
#include <stdio.h>

#include "Headers/IO.h"
#include "Headers/Integers.h"

// -- Initilization --
IOFile* InitIO(char* srcname)
{
	IOFile* file=calloc(1,sizeof(struct FileStructure));
	file->src=srcname;
	file->strsz=0;
	file->filesz=0;
	file->writing=0;

	return file;
}

IOFile* InitNewIOFile(char* name)
{
	IOFile* file=calloc(1,sizeof(struct FileStructure));
	file->src=name;
	file->strsz=0;
	file->filesz=0;
	file->line=0;
	file->writing=1;

	return file;
}

// -- Voids --
void IOReadFile(IOFile* file)
{
	FILE* fileobj=fopen(file->src,"r");

	if(fileobj!=NULL)
	{
		fseek(fileobj, 0, SEEK_END);
		file->strsz=ftell(fileobj);
		rewind(fileobj);

		file->data=malloc(file->strsz+1);
		file->filesz=fread(file->data,1,file->strsz,fileobj);
		file->data[file->strsz]='\0';

		if(file->strsz!=file->filesz)
		{
			free(file->data);
			file->data=NULL;
		}

		fclose(fileobj);
	}
	else
	{
		printf("File Not Found: %s\n",file->src);
		exit(1);
	}
}

void WriteIOFile(IOFile* file, char* data)
{
	if(file->writing==1)
	{
		file->data=realloc(file->data,file->filesz+strlen(data)+1);
		strcpy(file->data+file->filesz,data);
		file->filesz+=strlen(data);
	}
}

void SaveIOFile(IOFile* file)
{
	FILE* fileobj=fopen(file->src,"w");

	if(fileobj!=NULL)
	{
		fwrite(file->data,1,file->filesz,fileobj);
		fclose(fileobj);
	}
	else
	{
		printf("File Not Found: %s\n",file->src);
		exit(1);
	}
}

// -- Destruction --
void DestroyFile(IOFile* file)
{	
	free(file->data);
	free(file);
}

#endif