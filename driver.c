#include <stdio.h>
#include <stdlib.h>
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
        printf("%s ", TOKENS[root->nodeType.leafNode.terminal]);
        return;
    }
    else
    {
        printf("%s ", NONTERMINALS[root->nodeType.nonLeafNode.nonterminal]);
        TreeNode *child = root->nodeType.nonLeafNode.child;
        while (child != NULL)
        {
            preorder(child);
            child = child->next;
        }
    }
}

int main()
{
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *fileName = "grammar.txt";
    readGrammar(fileName, arr);

    printf("------------READING GRAMMER COMPLETED-------------\n");

    Token *ts = NULL;
    ts = tokeniseSourcecode("sourcecodetest.txt", ts);

    printf("-------------TOKENISING SOURCE CODE COMPLETED-------------\n");

    TreeNode *t = NULL;
    t = createParseTree(t, ts, arr);
    TreeNode *root = t;
    preorder(root);

    TypeExprEntry typeExprTable[MAX];
    currentTableEntry = 0;
    traverseParseTree(t, typeExprTable);

    return 0;
}
