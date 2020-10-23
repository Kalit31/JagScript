#include <stdio.h>
#include <stdlib.h>
#include "naryTree.h"

TreeNode *initialiseParseTree()
{
    TreeNode *node = createNonLeafNode(s);
    node->parent = NULL;
    return node;
}
TreeNode *createLeafNode(Terminal terminal)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->isLeaf = 1;
    node->nodeType.leafNode.terminal = terminal;
    node->next = NULL;
    return node;
}

TreeNode *createNonLeafNode(NonTerminal nonterminal)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    node->isLeaf = 0;
    node->nodeType.nonLeafNode.nonterminal = nonterminal;
    node->nodeType.nonLeafNode.child = NULL;
    node->next = NULL;
    return node;
}

TreeNode *createNode(int isLeafNode, int enumid, TreeNode *parent)
{
    TreeNode *node;
    if (isLeafNode)
    {
        node = createLeafNode(enumid);
    }
    else
    {
        node = createNonLeafNode(enumid);
    }
    return node;
}

TreeNode *nthSibling(TreeNode* child, int n){
    TreeNode *curr = child;
    for(int i=0; i<n; i++){
        if(curr==NULL){
            printf("Index out of Bounds Error for N-Ary Tree\n");
            exit(1);
        }
        curr=curr->next;
    }
    return curr;
}

TreeNode *nthChild(TreeNode* root, int n){
    TreeNode *child = root->nodeType.nonLeafNode.child;
    return nthSibling(child, n);
}

void addRuleToParseTree(TreeNode *n);
