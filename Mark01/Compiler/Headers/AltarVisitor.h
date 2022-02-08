#ifndef VISITORS
#define VISITORS

#include "../../Parser/IncludeLinker.h"

ASTreeType* VisitorVisitNode(ASTreeType* node);

ASTreeType* VisitorVisitVariable(ASTreeType* node);
ASTreeType* VisitorVisitVariableDeclaration(ASTreeType* node);
ASTreeType* VisitorVisitVariableAssignment(ASTreeType* node);

ASTreeType* VisitorVisitFunctionCall(ASTreeType* node);
ASTreeType* VisitorVisitFunctionDeclaration(ASTreeType* node);

ASTreeType* VisitorVisitString(ASTreeType* node);
ASTreeType* VisitorVisitCharacter(ASTreeType* node);
ASTreeType* VisitorVisitInteger(ASTreeType* node);
ASTreeType* VisitorVisitFloat(ASTreeType* node);
ASTreeType* VisitorVisitBool(ASTreeType* node);

ASTreeType* VisitorVisitRoot(ASTreeType* node);

#endif