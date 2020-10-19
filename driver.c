#include <stdio.h>
#include <stdlib.h>

#include "ReadGrammar.c"
#define NUM_RULES 50

int main()
{
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *fileName = "grammar.txt";
    readGrammar(fileName, arr);
    printf("------------READING GRAMMER COMPLETED-------------\n");
    return 0;
}