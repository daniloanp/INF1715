#ifndef MAIN_C
#define MAIN_C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/token.h"
#include "../trab1/token_list.h"
#include "../trab1/lex.h"
#include "ast.h"
#include "build_ast.h"


//extern AST AST_ROOT ;


int main( int argc, char **argv ) {	
	int ret;
	FILE *input;
	AST tree;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tree = BuildAst(input, &ret);
		if(ret == 0 ) {
			printf("Correct Syntax!!!\n");
			AST_PrettyPrint(tree, 1);
		}
	}
	else {
		printf("\nError: Cannot open file.\n");
		return 1;
	}

	if( input != stdin ) {
			fclose(input);
	}

	return ret;
}


#endif //AST_H