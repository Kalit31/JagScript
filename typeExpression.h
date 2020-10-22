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
    char *name;
    Type_Var type;
    Rect_Array_Type rectArrayType;
    TypeExpression typeExpr;
};

typedef struct typeexprentry TypeExprEntry;

#endif