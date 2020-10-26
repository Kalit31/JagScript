#include "naryTree.h"
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "printTypeExpression.c"

void printTerminal(TreeNode *node, int level)
{
    //printing leaf node in pretty columns
    printf("%-20s\tTERMINAL\t\t%-10s\t%d\t\t--\t\t%d\t\t--\n", TOKENS[node->terminal], node->tok->lexeme, node->tok->lineNo, level);
}

void printNonTerminal(TreeNode *node, int level)
{
    //printing non-leaf node
    printf("%-20s\tNON TERMINAL\t\t--\t\t--\t\t%d\t\t%d\t\t", NONTERMINALS[node->nonterminal], node->ruleNo, level);
    if (node->typeInformationStored == 1)
    {
        printTypeExpression(node->type, node->expression);
    }
    else
    {
        printf("--");
    }
    printf("\n");
}

void printTraverse(TreeNode *root, int level)
{
    // preorder traversal
    if (root == NULL)
    {
        return;
    }

    if (root->isLeaf)
    {
        // is a leaf node
        printTerminal(root, level);
    }
    else
    {
        printNonTerminal(root, level);
    }

    TreeNode *iter = root->child;
    while (iter != NULL)
    {
        printTraverse(iter, level + 1);
        iter = iter->next;
    }
}

void printParseTree(TreeNode *root)
{
    printf("SymbolName\t\tTerminal/NonTerminal\tNameOfLexeme\tLineNumber\tGrammarRule\tDepthOfNode\tTypeExpression\n\n");
    printTraverse(root, 0);
    printf("\n----------------------------------------------------PRINTING PARSE TREE COMPLETED---------------------------------------------\n\n");

    return;
}