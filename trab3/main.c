#ifndef MAIN_C
#define MAIN_C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
#include "build_ast.h"


//extern AST AST_ROOT ;


int main( int argc, char **argv ) {	
	bool hasErrors;
	FILE* input;
	AST tree;

	hasErrors = false;
	++argv, --argc;

	if ( argc > 0 ) {
		input = fopen( argv[0], "r" );
		if ( !input ) {
			printf("\nError: Cannot open file.\n");
			return 1;
		}
	}
	else {
		input = stdin;
	}
	
	tree = BuildAst(input, &hasErrors);

	if( input != stdin ) {
		fclose(input);
	}

	if( ! hasErrors ) {
		printf("Correct Syntax!!!\n");
		AST_PrettyPrint(tree, 1);
	} 
	AST_Free( tree );

	return hasErrors == true;
}


#endif //AST_H