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

int findRuleIndex(int nonTerminalInd, Node *g)
{
    for (int i = 0; i < NUM_RULES; i++)
    {
        if (strcmp(g[i].element, NONTERMINALS[nonTerminalInd]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int findLastRuleIndex(int nonTerminalInd, Node *g)
{
    int lastRuleInd = -1;
    for (int i = 0; i < NUM_RULES; i++)
    {
        if (strcmp(g[i].element, NONTERMINALS[nonTerminalInd]) == 0)
        {
            lastRuleInd = i;
        }
    }
    return lastRuleInd;
}

int build(TreeNode *curr, Stack *st, Token *s, Node *g)
{
    StackNode *t = top(st);
    if (t->val->isLeaf)
    {
        int tokenIdTopStack = t->val->nodeType.leafNode.terminal;
        int tokenIdTokenStream = s->tokenName;
        //printf("LEAF: %s\n", TOKENS[tokenIdTokenStream]);
        if (tokenIdTopStack == tokenIdTokenStream)
        {
            printf("POPPING %s\n", TOKENS[tokenIdTopStack]);
            pop(st);
            return build(curr, st, s->next, g);
        }
        else
        {
            return 0;
        }
    }
    else
    {
        int nonTerminalID = t->val->nodeType.nonLeafNode.nonterminal;

        int firstRuleNo = findRuleIndex(nonTerminalID, g);
        int lastRuleNo = findLastRuleIndex(nonTerminalID, g);

        printf("POPPING %s\n", NONTERMINALS[st->head->val->nodeType.nonLeafNode.nonterminal]);
        pop(st);

        for (int rule = firstRuleNo; rule <= lastRuleNo; rule++)
        {
            int rhsSize = g[rule].ruleSize;
            Node *list[rhsSize];

            int ptrInd = rhsSize - 1;
            Node *c = g[rule].nxt;
            for (; ptrInd >= 0; ptrInd--)
            {
                list[ptrInd] = c;
                c = c->nxt;
            }
            for (int i = 0; i < rhsSize; i++)
            {
                TreeNode *n;
                if (list[i]->element[0] >= 65 && list[i]->element[0] <= 90)
                {
                    n = createLeafNode(findTerminal(list[i]->element));
                }
                else
                {
                    n = createNonLeafNode(findNonTerminal(list[i]->element));
                }
                printf("PUSHING %s\n", list[i]->element);
                push(st, n);
            }
            build(curr, st, s, g);
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

    build(root, st, s, g);
    return root;
}