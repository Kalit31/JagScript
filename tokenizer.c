#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

TokenName findCorrectToken(char *token);

Token *tokeniseSourcecode(char *fileName, Token *s)
{
    FILE *fp = fopen(fileName, "r");

    // Exit if file could not be opened
    if (fp == NULL)
    {
        printf("Could not read source code.txt\n");
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

//check if the token is an integer
int isNum(char *token)
{
    char *ptr = token;
    for (; *ptr != '\0'; ptr++)
    {
        char ch = *ptr;
        if (!(ch >= '0' && ch <= '9'))
        {
            return 0;
        }
    }
    return 1;
}

//check if the token id an ID
int isID(char *token)
{
    char *ptr = token;
    char ch = *ptr;
    if (!(ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')))
    {
        return 0;
    }
    ptr++;
    for (; *ptr != '\0'; ptr++)
    {
        ch = *ptr;
        if (!(ch == '_' || (ch >= 'a' && ch <= 'z') ||
              (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9')))
        {
            return 0;
        }
    }
    return 1;
}

// Takes input as a token and returns the appropriate TokenName
TokenName findCorrectToken(char *token)
{
    if (strcmp(token, "program") == 0)
    {
        return PROGRAM;
    }
    else if (strcmp(token, "(") == 0)
    {
        return RBOP;
    }
    else if (strcmp(token, ")") == 0)
    {
        return RBCL;
    }
    else if (strcmp(token, "{") == 0)
    {
        return CBOP;
    }
    else if (strcmp(token, "}") == 0)
    {
        return CBCL;
    }
    else if (strcmp(token, "[") == 0)
    {
        return SBOP;
    }
    else if (strcmp(token, "]") == 0)
    {
        return SBCL;
    }
    else if (strcmp(token, "declare") == 0)
    {
        return DECLARE;
    }
    else if (strcmp(token, "list") == 0)
    {
        return LIST;
    }
    else if (strcmp(token, "of") == 0)
    {
        return OF;
    }
    else if (strcmp(token, "variables") == 0)
    {
        return VARIABLES;
    }
    else if (strcmp(token, "array") == 0)
    {
        return ARRAY;
    }
    else if (strcmp(token, "jagged") == 0)
    {
        return JAGGED;
    }
    else if (strcmp(token, "size") == 0)
    {
        return SIZE;
    }
    else if (strcmp(token, "values") == 0)
    {
        return VALUES;
    }
    else if (strcmp(token, "integer") == 0)
    {
        return INTEGER;
    }
    else if (strcmp(token, "real") == 0)
    {
        return REAL;
    }
    else if (strcmp(token, "boolean") == 0)
    {
        return BOOLEAN;
    }
    else if (strcmp(token, ":") == 0)
    {
        return COLON;
    }
    else if (strcmp(token, ";") == 0)
    {
        return SEMICOLON;
    }
    else if (strcmp(token, "+") == 0)
    {
        return PLUS;
    }
    else if (strcmp(token, "-") == 0)
    {
        return MINUS;
    }
    else if (strcmp(token, "*") == 0)
    {
        return MULT;
    }
    else if (strcmp(token, "/") == 0)
    {
        return DIV;
    }
    else if (strcmp(token, "&&&") == 0)
    {
        return AND;
    }
    else if (strcmp(token, "|||") == 0)
    {
        return OR;
    }
    else if (strcmp(token, "..") == 0)
    {
        return TWODOT;
    }
    else if (strcmp(token, "R1") == 0)
    {
        return R;
    }
    else if (strcmp(token, "=") == 0)
    {
        return EQUALS;
    }
    else if (isNum(token))
    {
        return NUM;
    }
    else if (isID(token))
    {
        return ID;
    }
    else
    {
        return ERR;
    }
}