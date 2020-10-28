/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#ifndef TYPE_EXPR_TABLE
#define TYPE_EXPR_TABLE
#include "naryTree.h"

typedef enum Rect_Array_Type
{
    STATIC,
    DYNAMIC,
    NOT_APPLICABLE
} Rect_Array_Type;

struct typeexprentry
{
    //Field1
    char *name;
    //Field2
    Type_Var type;
    //Field3
    Rect_Array_Type rectArrayType;
    //Field4
    TypeExpression typeExpr;
};

typedef struct typeexprentry TypeExprEntry;

#endif
