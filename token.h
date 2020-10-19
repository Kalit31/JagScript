#ifndef LEX_DEF_
#define LEX_DEF_

typedef enum TokenName
{
    // Add tokens here
    PROGRAM,
    DECLARE,
    LIST,
    OF,
    VARIABLES,
    ARRAY,
    SIZE,
    VALUES,
    JAGARR,
    INTEGER,
    REAL,
    BOOLEAN,
    RBOP,
    RBCL,
    CBOP,
    CBCL,
    SQOP,
    SQCL,
    COLON,
    SEMICOLON,
    PLUS,
    MINUS,
    MULT,
    DIV,
    AND,
    OR,
    TWODOT,
    ID,
    NUM
} TokenName;

typedef union Value
{
    int INT_VALUE;
    float FLOAT_VALUE;
} Value;

struct token
{
    TokenName tokenName;
    char *lexeme;
    int lineNo;

    struct token *next;
};

typedef struct token Token;
#endif
