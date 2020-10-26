#include <stdio.h>
#include "typeExpression.h"

char *printArrayType(int n)
{
    switch (n)
    {
    case 0:
    {
        return "STATIC";
    }
    case 1:
    {
        return "DYNAMIC";
    }
    default:
    {
        return "NOT APPLICABLE";
    }
    }
}

char *printType(int type)
{
    switch (type)
    {
    case PRIMITIVE:
    {
        return "Primitive";
    }
    case RECTANGULAR_ARRAY:
    {
        return "Rectangular Array";
    }
    case JAGGED_ARRAY:
    {
        return "Jagged Array";
    }
    }
}

void printTypeExpressionTable(TypeExprEntry *t, int totalentries)
{
    printf("Field1\t\t\tField2\t\tField3\t\t\tField4\n\n");
    for (int i = 0; i < totalentries; i++)
    {
        printf("%-10s%20s\t\t%-20s\t", t[i].name, printType(t[i].type), printArrayType(t[i].rectArrayType));
        printTypeExpression(t[i].type, t[i].typeExpr);
        printf("\n");
    }
    printf("\n-----------------------------------------------PRINTING TYPE EXPRESSION TABLE COMPLETED---------------------------------------\n\n");
}