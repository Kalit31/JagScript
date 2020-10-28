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