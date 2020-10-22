#include <stdio.h>
#include <stdlib.h>
#include "ReadGrammar.c"
#include "tokenizer.c"
#include "parsetree.c"
//Max to be fixed.
#define MAX 1000

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

    TypeExprEntry *typeExprTable = (TypeExprEntry *)malloc(sizeof(TypeExprEntry) * MAX);
    traverseParseTree(t, typeExprTable);

    return 0;
}
