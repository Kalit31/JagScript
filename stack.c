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
    if (tn->isLeaf)
    {
        node->terminal = 1;
        node->id = tn->nodeType.leafNode.terminal;
    }
    else
    {
        node->terminal = 0;
        node->id = tn->nodeType.nonLeafNode.nonterminal;
    }
    // node->val = tn;
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
        printf("PUSHING INTO STACK %s\n", TOKENS[tn->nodeType.leafNode.terminal]);
    }
    else
    {
        printf("PUSHING INTO STACK %s\n", NONTERMINALS[tn->nodeType.nonLeafNode.nonterminal]);
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
        if (t->terminal == 1)
        {
            printf("POPPING FROM STACK1 %s\n", TOKENS[t->id]);
        }
        else
        {
            printf("POPPING FROM STACK2 %s\n", NONTERMINALS[t->id]);
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
