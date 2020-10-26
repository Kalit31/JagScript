#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ReadGrammar.c"
#include "tokenizer.c"
#include "parsetree.c"
#include "printParseTree.c"
#include "printTypeExpressionTable.c"
//Max to be fixed.
#define MAX 1000

int main()
{
    clock_t begin = clock();
    Node *arr = (Node *)malloc(sizeof(Node) * NUM_RULES);
    char *fileName = "grammar.txt";
    readGrammar(fileName, arr);

    printf("\n----------------------------------------------------READING GRAMMAR COMPLETED-------------------------------------------------\n\n");

    Token *ts = NULL;
    fileName = "sourcecodetest.txt";
    ts = tokeniseSourcecode(fileName, ts);
    printf("\n-------------------------------------------------TOKENISING SOURCE CODE COMPLETED---------------------------------------------\n\n");

    TreeNode *t = NULL;
    t = createParseTree(t, ts, arr);

    TypeExprEntry typeExprTable[MAX];
    currentTableEntry = 0;

    printf("LineNo.\t\t\tStatement Type\t\tOperator\t\tLexeme1\t\tType1\t\tLexeme2\t\tType2\t\tMessage\n\n");

    traverseParseTree(t, typeExprTable);

    printf("\n-------------------------------------------------TRAVERSING PARSE TREE COMPLETED---------------------------------------------\n\n");

    printf("\n----------------------------------------------------PRINTING TYPE ERRORS COMPLETED--------------------------------------------\n\n");

    printParseTree(t);

    printTypeExpressionTable(typeExprTable, currentTableEntry);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("TIME SPENT: %lf\n", time_spent);

    return 0;
}