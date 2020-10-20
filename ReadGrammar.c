#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#define ARROW "=>"

// This function populates the llist by reading the grammer text file
void readGrammar(char *fileName, Node *llist)
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

    // Read the file line by line
    while (fscanf(fp, " %[^\n]", eachLine) != EOF)
    {
        char *delim = " ";
        char *token;

        // Extract the leftmost non-terminal of the rule
        token = strtok(eachLine, delim);

        llist[line].element = (char *)malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(llist[line].element, token);
        llist[line].nxt = NULL;

        Node *curr = &llist[line];

        token = strtok(NULL, delim);
        int ruleSize = 0;
        while (token != NULL)
        {
            if (strcmp(ARROW, token) != 0)
            {
                Node *next = (Node *)malloc(sizeof(Node));
                next->element = (char *)malloc(sizeof(char) * (strlen(token) + 1));
                strcpy(next->element, token);
                next->ruleSize = 0;
                next->nxt = NULL;
                curr->nxt = next;
                curr = next;
                ruleSize++;
            }
            token = strtok(NULL, delim);
        }

        curr = &llist[line];
        curr->ruleSize = ruleSize;
        line++;
    }

    fclose(fp);
}
