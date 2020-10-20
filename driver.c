#include <stdio.h>
#include <stdlib.h>
#include "ReadGrammar.c"
#include "tokenizer.c"
#include "parsetree.c"
#define NUM_RULES 65

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
    return 0;
}
