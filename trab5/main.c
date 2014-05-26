#ifndef MAIN_C
#define MAIN_C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/token.h"
#include "../trab1/token_list.h"
#include "../trab1/lex.h"
#include "../trab3/ast.h"
#include "../trab3/build_ast.h"
#include "../trab4/symboltable.h"
#include "../trab4/symbols.h"

//extern AST AST_ROOT ;


int main( int argc, char **argv ) {	
	int ret;
	FILE *input;
	++argv, --argc;
	AST tree;
	
	if ( argc > 0 ) {
		input = fopen( argv[0], "r" );
	}
	else {
		input = stdin;
	}

	if(input) {
		tree = BuildAst(input, &ret);
		ret = ! Symbols_annotate( tree );

		if(ret == 0 ) {
			printf("Correct Syntax!!!\n");
			AST_PrettyPrint(tree, 1);
		}
		AST_Free(tree);
	}
	else {
		printf("\nError: Cannot open file.\n");
		return 1;
	}

	if( input != stdin )
			fclose(input);

	return ret;
}


#endif //AST_H