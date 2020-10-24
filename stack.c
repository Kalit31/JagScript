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
    StackNode *newNode = createStackNode(tn);
    newNode->nxt = st->head;
    st->head = newNode;
    st->size = st->size + 1;
    if (tn->isLeaf)
    {
        printf("PUSHING INTO STACK %s\n", TOKENS[tn->terminal]);
    }
    else
    {
        printf("PUSHING INTO STACK %s\n", NONTERMINALS[tn->nonterminal]);
    }
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
        if (t->val->isLeaf == 1)
        {
            printf("POPPING FROM STACK1 %s\n", TOKENS[t->val->terminal]);
        }
        else
        {
            printf("POPPING FROM STACK2 %s\n", NONTERMINALS[t->val->nonterminal]);
        }
        return t;
    }
}

StackNode *top(Stack *st)
{
    if (st->size == 0)
    {
        printf("Stack is empty.\n");
        return NULL;
    }
    else
    {
        StackNode *t = st->head;
        return t;
    }
}
