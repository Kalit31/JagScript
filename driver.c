#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ReadGrammar.c"
#include "tokenizer.c"
#include "parsetree.c"
//Max to be fixed.
#define MAX 1000

void preorder(TreeNode *root)
{
    if (root == NULL)
    {
        return;
    }
    if (root->isLeaf)
    {
        printf("%s ", TOKENS[root->terminal]);
        return;
    }
    else
    {
        printf("%s ", NONTERMINALS[root->nonterminal]);
        TreeNode *child = root->child;
        while (child != NULL)
        {
            preorder(child);
            child = child->next;
        }
    }
}

int main()
{
    clock_t begin = clock();
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *fileName = "grammar.txt";
    readGrammar(fileName, arr);

    printf("------------READING GRAMMER COMPLETED-------------\n");

    Token *ts = NULL;
    ts = tokeniseSourcecode("sourcecodetest.txt", ts);
    // Token *temp = ts;
    // while (ts != NULL)
    // {
    //     printf("%s\n", TOKENS[ts->tokenName]);
    //     ts = ts->next;
    // }
    printf("-------------TOKENISING SOURCE CODE COMPLETED-------------\n");

    TreeNode *t = NULL;
    t = createParseTree(t, ts, arr);
    TreeNode *root = t;
    preorder(root);
    printf("\n");

    TypeExprEntry typeExprTable[MAX];
    currentTableEntry = 0;
    traverseParseTree(t, typeExprTable);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("TIME SPENT: %lf\n", time_spent);

    return 0;
}
