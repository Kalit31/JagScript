#ifndef LEX_DEF_
#define LEX_DEF_

typedef enum TokenName{
    // Add tokens here
    TQ_SQOP,
    TQ_SQCL
} TokenName;

typedef union Value {
    int INT_VALUE;
    float FLOAT_VALUE;
} Value;

typedef struct Token {
    TokenName TOKEN_NAME;
    char* LEXEME;
    int LINE_NO;

    // Stores 0 if integer 1 if an boolean
    // 2 to denote an unknown symbol
    int IS_NUMBER;

    Value* VALUE; // Stores NULL if the Token is not a number
    struct Token* next;
} Token;

typedef Token* tokenStream; //tokenStream is a linkedlist of tokens

#endif
