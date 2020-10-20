#include <stdio.h>
#include <stdlib.h>
#include "parsetree.h"
#include "naryTree.c"
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
    return -1;
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
    return -1;
}

int build(TreeNode *curr, Stack *st, Token *s, Node *g, int currTokenIndex)
{
    StackNode *t = top(st);
    if (t->val->isLeaf)
    {
    }
    else
    {
        int ruleNo = t->val->nodeType.nonLeafNode.nonterminal;
        printf("RULE NO: %d\n", ruleNo);
        printf("%d RULE SIZE: %d\n", t->val->nodeType.nonLeafNode.nonterminal, g[ruleNo].ruleSize);
        int rhsSize = g[ruleNo].ruleSize;
        Node *list[rhsSize];

        int ptrInd = rhsSize - 1;
        Node *curr = g[ruleNo].nxt;
        for (; ptrInd >= 0; ptrInd--)
        {
            list[ptrInd] = curr;
            curr = curr->nxt;
        }
        pop(st);
        for (int i = 0; i < rhsSize; i++)
        {
            TreeNode *n;
            if (list[i]->element[0] >= 65 && list[i]->element[0] <= 90)
            {
                n = createNonLeafNode(findNonTerminal(list[i]->element));
            }
            else
            {
                n = createLeafNode(findTerminal(list[i]->element));
            }
            push(st, n);
        }
    }
    return 0;
}

// Think of an algorithm for parsing. Currently, the idea is to implement
//  a recursive one with backtrackong, but it is not working :(

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g)
{
    int error = 0;
    Stack *st = createStack();

    TreeNode *root = initialiseParseTree();
    push(st, root);

    build(root, st, s, g, 0);
    return root;
}