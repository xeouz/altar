#ifndef ERRORSH
#define ERRORSH

#include "../Parser/IncludeLinker.h"
#include "Colors.h"

char* LexErrors[] = {
    "%s[AltarC] Error(s) in\n  %sFile `%s` in line %d\n   %s%d|  %s\n   %s%d|  %s\n   %s%d|  %s\n\n%sInvalid Character '%c' found\n\n",
};

char* ParseErrors[] = {
    ""
};

char* TraverseErrors[] = {
    ""
};

#endif