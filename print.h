/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#ifndef PRINT_H
#define PRINT_H
#include "naryTree.h"
#include "typeExpression.h"

void printParseTree(TreeNode *root);
void printTypeExpression(int type, TypeExpression typeExpr);
char *printArrayType(int n);
char *printType(int type);

#endif
