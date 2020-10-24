#ifndef STACK_H
#define STACK_H

#include "naryTree.h"

struct stackNode
{
    TreeNode *val;
    struct stackNode *nxt;
};

typedef struct stackNode StackNode;

struct stack
{
    StackNode *head;
    int size;
};

typedef struct stack Stack;

Stack *createStack();
StackNode *createStackNode(TreeNode *treenode);
StackNode *pop(Stack *stk);
StackNode *top(Stack *stk);
void push(Stack *stk, TreeNode *treenode);

#endif
