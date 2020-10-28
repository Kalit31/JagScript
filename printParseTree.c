/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#include "naryTree.h"
#include <stdio.h>
#include <stdlib.h>
#include "token.h"
#include "printTypeExpression.c"

void printTerminal(TreeNode *node)
{
    //printing leaf node in pretty columns
    if (node->terminal != EPS && node->tok != NULL)
        printf("%-20s\tTERMINAL\t\t%-10s\t%d\t\t--\t\t%d\t\t--\n", TOKENS[node->terminal], node->tok->lexeme, node->tok->lineNo, node->depth);
}

void printNonTerminal(TreeNode *node)
{
    //printing non-leaf node
    printf("%-20s\tNON TERMINAL\t\t--\t\t--\t\t%d\t\t%d\t\t", NONTERMINALS[node->nonterminal], node->ruleNo, node->depth);
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

void printTraverse(TreeNode *root)
{

    // preorder traversal
    if (root == NULL)
    {
        return;
    }

    if (root->isLeaf)
    {
        // is a leaf node
        printTerminal(root);
    }
    else
    {
        printNonTerminal(root);
    }

    TreeNode *iter = root->child;
    while (iter != NULL)
    {
        printTraverse(iter);
        iter = iter->next;
    }
}

void printParseTree(TreeNode *root)
{
    printf("SymbolName\t\tTerminal/NonTerminal\tNameOfLexeme\tLineNumber\tGrammarRule\tDepthOfNode\tTypeExpression\n\n");
    printTraverse(root);
    printf("\n----------------------------------------------------PRINTING PARSE TREE COMPLETED---------------------------------------------\n\n");

    return;
}
