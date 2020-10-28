/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#ifndef PARSE_TREE
#define PARSE_TREE

#include "typeExpression.h"
#include "node.h"

static int currentTableEntry = 0;
static int TYPETABLESIZE = 0;
static TypeExprEntry *table;

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g);
void traverseParseTree(TreeNode *t);
void printTypeExpressionTable();

#endif
