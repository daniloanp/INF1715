#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdbool.h>
#include "token_list.h"

TokenList generateTokens( FILE* f, bool* status);

#endif