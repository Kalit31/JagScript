#ifndef TOKENIZER_h
#define TOKENIZER_h
#include "token.h"

Token *tokeniseSourcecode(char *fileName, Token *s);

TokenName findCorrectToken(char *token);

#endif