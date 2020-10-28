/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define ARROW "=>"
#include "readGrammar.h"

// This function populates the llist by reading the grammer text file
void readGrammar(char *fileName, Node *llist)
{
    FILE *fp = fopen(fileName, "r");

    // Exit if file could not be opened
    if (fp == NULL)
    {
        printf("Could not read %s\n", fileName);
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
