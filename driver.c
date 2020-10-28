#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "readGrammar.h"
#include "tokenizer.h"
#include "parsetree.h"
#include "print.h"

int main(int argc, char * argv[])
{
    if(argc>2) printf("Recieved more command line arguments than expected.\n\n");
    else if(argc==1) printf("You have not passed any Source Code File\n\n");

    clock_t begin = clock();
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *grammarFile = "grammar.txt";
    readGrammar(grammarFile, arr);
    printf("\n----------------------------------------------------READING GRAMMAR COMPLETED-------------------------------------------------\n\n");

    Token *ts = NULL;

    char *fileName = argv[1];
    ts = tokeniseSourcecode(fileName, ts);
    printf("\n-------------------------------------------------TOKENISING SOURCE CODE COMPLETED---------------------------------------------\n\n");

    while (1)
    {
        printf("Option 0: exit\n");
        printf("Option 1: Create parse tree\n");
        printf("Option 2: Traverse the parse tree to construct typeExpressionTable. Also print the type errors while traversing the parse tree and accessing the typeExpressionTable\n");
        printf("Option 3: Print parse tree in the specified format\n");
        printf("Option 4: Print typeExpressionTable in the specified format.\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 0:
        {
            exit(0);
        }
        case 1:
        {
            TreeNode *t = NULL;
            t = createParseTree(t, ts, arr);
            break;
        }
        case 2:
        {
            TreeNode *t = NULL;
            t = createParseTree(t, ts, arr);
            table = NULL;
            currentTableEntry = 0;
            TYPETABLESIZE = 0;
            printf("LineNo.\t\tStatement Type\t\tOperator\tLexeme1\t\t\tType1\t\tLexeme2\t\t\tType2\t\tDepth\t\tMessage\n\n");
            traverseParseTree(t);
            printf("\n-------------------------------------------------TRAVERSING PARSE TREE COMPLETED---------------------------------------------");
            printf("\n----------------------------------------------------PRINTING TYPE ERRORS COMPLETED--------------------------------------------\n\n");
            break;
        }
        case 3:
        {

            TreeNode *t = NULL;
            t = createParseTree(t, ts, arr);
            if (t == NULL)
            {
                printf("There is some syntax error in the program\n");
            }
            else
            {
                printParseTree(t);
            }
            break;
        }
        case 4:
        {
            TreeNode *t = NULL;
            t = createParseTree(t, ts, arr);

            if (t == NULL)
            {
                printf("There is some syntax error in the program\n");
            }
            else
            {
                table = NULL;
                currentTableEntry = 0;
                TYPETABLESIZE = 0;
                printf("LineNo.\t\tStatement Type\t\tOperator\tLexeme1\t\t\tType1\t\tLexeme2\t\t\tType2\t\t\tMessage\n\n");
                traverseParseTree(t);
                printf("\n-------------------------------------------------TRAVERSING PARSE TREE COMPLETED---------------------------------------------");
                printf("\n----------------------------------------------------PRINTING TYPE ERRORS COMPLETED--------------------------------------------\n\n");
                printTypeExpressionTable(table, currentTableEntry);
            }
            break;
        }
        default:
        {
            printf("No such choice available.\n");
        }
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("TIME SPENT: %lf\n", time_spent);
    return 0;
}
