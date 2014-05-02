#ifndef BUILD_AST_H
#define BUILD_AST_H
#include <stdlib.h>
#include <stdio.h>
#include "../trab1/token.h"
#include "AbstractSyntaxTree.h"

//this function assumes that the parser are ok;
AST BuildAst( FILE* input, int *res ); 

#endif //AST_H