#ifndef PRINT_H
#define PRINT_H
#include "naryTree.h"
#include "typeExpression.h"

void printParseTree(TreeNode *root);
void printTypeExpression(int type, TypeExpression typeExpr);
char *printArrayType(int n);
char *printType(int type);

#endif