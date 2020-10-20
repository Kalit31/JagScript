#include <stdio.h>
#include <stdlib.h>
#include "parsetree.h"

TreeNode *createParseTree(TreeNode *t, Token *s, Node *g)
{
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