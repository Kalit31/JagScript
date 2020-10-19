#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "findCorrectToken.c"

Token *tokeniseSourcecode(char *fileName, Token *s)
{
    FILE *fp = fopen(fileName, "r");

    // Exit if file could not be opened
    if (fp == NULL)
    {
        printf("Could not read grammer.txt\n");
        exit(0);
    }

    int line = 0;
    char eachLine[1000];

    Token *curr = s;
    Token *next = NULL;

    // Read the file line by line
    while (fscanf(fp, " %[^\n]", eachLine) != EOF)
    {
        char *delim = " ";
        char *token;

        // Extract the leftmost non-terminal of the rule
        token = strtok(eachLine, delim);

        if (s == NULL)
        {
            s = (Token *)malloc(sizeof(Token));
            s->lexeme = (char *)malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(s->lexeme, token);
            s->tokenName = findCorrectToken(token);
            s->lineNo = line + 1;
            s->next = NULL;
            curr = s;
        }
        else
        {
            next = (Token *)malloc(sizeof(Token));
            next->lexeme = (char *)malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(next->lexeme, token);
            next->tokenName = findCorrectToken(token);
            next->lineNo = line + 1;
            curr->next = next;
            curr = next;
        }
        token = strtok(NULL, delim);
        while (token != NULL)
        {
            next = (Token *)malloc(sizeof(Token));
            next->lexeme = (char *)malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(next->lexeme, token);
            next->tokenName = findCorrectToken(token);
            next->lineNo = line + 1;
            curr->next = next;
            curr = next;
            token = strtok(NULL, delim);
        }
        line++;
    }
    return s;
    fclose(fp);
}
