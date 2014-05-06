#ifndef BUILD_AST_H
#define BUILD_AST_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/token.h"
#include "ast.h"


//this function assumes that the parser are ok;
AST BuildAst( FILE* input, bool *res ); 

#endif //AST_H