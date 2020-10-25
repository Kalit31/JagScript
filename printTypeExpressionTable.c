#include <stdio.h>
#include "typeExpression.h"

void printTypeExpressionTable(TypeExprEntry *t, int totalentries)
{
    printf("Field1\t\t\tField2\t\t\tField3\t\t\tField4\n");
    for (int i = 0; i < totalentries; i++)
    {
        printf("%-20s\t%d\t\t\t%d\t\t\tTODO\n", t[i].name, t[i].type, t[i].rectArrayType);
    }
    printf("\n------------------------------------------------------------------------------------------------------------------------------\n\n");
}