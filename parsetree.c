#include <stdio.h>
#include <stdlib.h>
#include "parsetree.h"
#include "stack.c"

int findTerminal(char *s)
{
    for (int i = 0; i < NUM_TOKENS; i++)
    {
        if (strcmp(TOKENS[i], s) == 0)
        {
            return i;
        }
    }
}

int findNonTerminal(char *s)
{
    for (int i = 0; i < NUM_NON_TERMINALS; i++)
    {
        if (strcmp(NONTERMINALS[i], s) == 0)
        {
            return i;
        }
    }
}

TreeNode *createTreeNode()
{
}

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g)
{
    Stack *st = createStack();
    int ruleNo = 0;
    Node *curr = &g[0];

    NonLeafNode nonLeafNode;
    nonLeafNode.ruleNo = ruleNo;
    nonLeafNode.child = NULL;
    nonLeafNode.nonterminal = findNonTerminal(curr->element);

    NodeType nodeType;
    nodeType.nonLeafNode = nonLeafNode;

    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    newNode->isLeaf = 0;
    newNode->parent = NULL;
    newNode->next = NULL;
    newNode->nodeType = nodeType;

    printf("%d\n", newNode->nodeType.nonLeafNode.nonterminal);

    push(st, newNode);
    pop(st);
    pop(st);
    return NULL;
}

/*
 Node *curr;
    for (int i = 0; i < 2; i++)
    {
        curr = &g[i];
        while (curr != NULL)
        {
            printf("%s ", curr->element);
            curr = curr->nxt;
        }
        printf("\n");
    }
    Token *tokCurr = s;
    while (tokCurr != NULL)
    {
        printf("%s-%d\n", tokCurr->lexeme, tokCurr->tokenName);
        tokCurr = tokCurr->next;
    }
*/