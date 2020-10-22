#include "typeExpression.h"
#include "node.h"

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g);

void traverseParseTree(TreeNode *t, TypeExprEntry *table);

void printParseTree(TreeNode *t);