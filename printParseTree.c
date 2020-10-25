#include "naryTree.h"
#include <stdio.h>
#include <stdlib.h>
#include "token.h"




void printTerminal(TreeNode * node, int level){
    //printing leaf node in pretty columns
    printf("%-25s%-25s%-25s%-25s%-25d%-25s%-25d\n",TOKENS[node->terminal], "Terminal","-----", node->tok->lexeme, node->tok->lineNo,"-----",level);
}

void printNonTerminal(TreeNode * node,int level){
    //printing non-leaf node
    printf("%-25s%-25s%-25s%-25s%-25s%-25s%-25d\n",NONTERMINALS[node->nonterminal],"Non-Terminal",node->expression,"-----","-----",node->type, level);
}


void printTraverse(TreeNode * root, int level){
    // preorder traversal
    if(root == NULL){
        return;
    }
    
    if(root->isLeaf){
        // is a leaf node
        printTerminal(root,level);
    }
    else{
        printNonTerminal(root,level);
    }

    TreeNode* iter = root->child;
    while(iter!=NULL){
        printTraverse(iter,level+1);
        iter=iter->next;
    }
}


void printParseTree(TreeNode * root){
    printTraverse(root,0);
    return;
}