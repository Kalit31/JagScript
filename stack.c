#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack *createStack()
{
    Stack *st = (Stack *)malloc(sizeof(Stack));
    st->head = NULL;
    st->size = 0;
    return st;
}

StackNode *createStackNode(TreeNode *tn)
{
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    node->val = tn;
    node->nxt = NULL;
    return node;
}

void push(Stack *st, TreeNode *tn)
{
    StackNode *newNode = createStack(tn);
    newNode->nxt = st->head;
    st->head = newNode;
    st->size = st->size + 1;
}

StackNode *pop(Stack *st)
{
    if (st->size == 0)
    {
        printf("Stack is empty.Unable to pop.\n");
        return NULL;
    }
    else
    {
        StackNode *t = st->head;
        st->head = st->head->nxt;
        st->size = st->size - 1;
        return t;
    }
}
StackNode *top(Stack *st)
{
    if (st->size == 0)
    {
        printf("Stack is empty.");
        return NULL;
    }
    else
    {
        StackNode *t = st->head;
        return t;
    }
}
