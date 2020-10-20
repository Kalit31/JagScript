#ifndef LEX_DEF_
#define LEX_DEF_

#define NUM_TOKENS 32

static char *TOKENS[NUM_TOKENS] = {
    "PROGRAM",
    "DECLARE",
    "LIST",
    "OF",
    "VARIABLES",
    "ARRAY",
    "SIZE",
    "VALUES",
    "JAGGED",
    "INTEGER",
    "REAL",
    "BOOLEAN",
    "RBOP",
    "RBCL",
    "CBOP",
    "CBCL",
    "SBOP",
    "SBCL",
    "COLON",
    "SEMICOLON",
    "PLUS",
    "MINUS",
    "MULT",
    "DIV",
    "AND",
    "OR",
    "TWODOT",
    "R",
    "EQUALS",
    "ID",
    "NUM",
    "ERR"};

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
    JAGGED,
    INTEGER,
    REAL,
    BOOLEAN,
    RBOP,
    RBCL,
    CBOP,
    CBCL,
    SBOP,
    SBCL,
    COLON,
    SEMICOLON,
    PLUS,
    MINUS,
    MULT,
    DIV,
    AND,
    OR,
    TWODOT,
    R,
    EQUALS,
    ID,
    NUM,
    ERR
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
