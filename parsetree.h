#include "typeExpression.h"
#include "node.h"

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g);

void traverseParseTree(TreeNode *t);

void printParseTree(TreeNode *t);