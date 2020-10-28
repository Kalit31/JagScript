/* Group 3
Ashrya Agrawal          2018A7PS0210P
Kalit Naresh Inani      2018A7PS0207P
Prajwal Gupta           2018A7PS0231P */


#ifndef TOKENIZER_h
#define TOKENIZER_h
#include "token.h"

Token *tokeniseSourcecode(char *fileName, Token *s);

TokenName findCorrectToken(char *token);

#endif
