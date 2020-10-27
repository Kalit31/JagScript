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
        for (int i = 0; i < raExp.currDimensions; i++)
        {
            printf("range_R%d=(", (i + 1));
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
        JaggedArray jaggArr = typeExpr.jaggedType;
        printf("jaggedArray, dimensions=%d, ", jaggArr.is2D ? 2 : 3);
        printf("range_R1 = (%d,%d), range_R2 = (", jaggArr.r1Low, jaggArr.r1High);
        int r1Len = jaggArr.r1High - jaggArr.r1Low + 1;
        for(int i=0; i<r1Len; i++) {
            if (jaggArr.is2D) {
                printf(" %d,", jaggArr.type.twod_array.size[i]);
            } else {
                printf("%d[", jaggArr.type.threed_array.sizeR2[i]);
                for(int j=0; j<jaggArr.type.threed_array.sizeR2[i]; j++){
                    printf("%d,", jaggArr.type.threed_array.size[i][j]);
                }
                printf("], ");
            }
        }
        printf("), basicElementType = integer");
        break;
    }
    }
    printf(">");
}
