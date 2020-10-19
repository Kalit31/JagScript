#include <stdio.h>
#include <stdlib.h>

#include "ReadGrammar.c"
#include "tokenizer.c"
#define NUM_RULES 50

int main()
{
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *fileName = "grammar.txt";
    readGrammar(fileName, arr);
    Node *curr;
    for (int i = 0; i < 2; i++)
    {
        curr = &arr[i];
        while (curr != NULL)
        {
            printf("%s ", curr->element);
            curr = curr->nxt;
        }
        printf("\n");
    }
    printf("------------READING GRAMMER COMPLETED-------------\n");

    Token *ts = NULL;
    ts = tokeniseSourcecode("sourcecodetest.txt", ts);
    Token *tokCurr = ts;
    while (tokCurr != NULL)
    {
        printf("%s-%d\n", tokCurr->lexeme, tokCurr->tokenName);
        tokCurr = tokCurr->next;
    }
    printf("-------------TOKENISING SOURCE CODE COMPLETED-------------\n");

    return 0;
}
