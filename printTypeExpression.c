#include "naryTree.h"
#include <stdio.h>
#include <stdlib.h>
#include "token.h"

void printTypeExpression(int type, TypeExpression typeExpr)
{
    printf("< type = ");

    switch (type)
    {
    case PRIMITIVE:
    {
        int primType = typeExpr.primitiveType;
        switch (primType)
        {
        case PRIM_INTEGER:
        {
            printf("integer ");
            break;
        }
        case PRIM_REAL:
        {
            printf("real ");
            break;
        }
        case PRIM_BOOLEAN:
        {
            printf("boolean ");
            break;
        }
        }
        break;
    }
    case RECTANGULAR_ARRAY:
    {
        RectangularArray raExp = typeExpr.rectType;
        printf("rectangularArray, dimensions=%d,", raExp.currDimensions);
        for (int i = 1; i <= raExp.currDimensions; i++)
        {
            printf("range_R%d=(", (i - 1));
            if (raExp.dimenArray[i][0] != -1)
            {
                printf("%d,", raExp.dimenArray[i][0]);
            }
            else
            {
                printf("%s,", raExp.dynamicDimenArray[i][0]);
            }
            if (raExp.dimenArray[i][1] != -1)
            {
                printf("%d),", raExp.dimenArray[i][1]);
            }
            else
            {
                printf("%s),", raExp.dynamicDimenArray[i][1]);
            }
        }
        printf("basicElementType = integer");
        break;
    }
    case JAGGED_ARRAY:
    {
        break;
    }
    }
    printf(">");
}
