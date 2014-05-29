#ifndef MAIN_C
#define MAIN_C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/token.h"
#include "../trab1/token_list.h"
#include "../trab1/mini0-lex.h"
#include "../trab3/ast.h"
#include "../trab3/build_ast.h"
#include "../trab4/symboltable.h"
#include "../trab4/symbols.h"
#include "ir_code.h"
#include "build_ir_code.h"
//extern AST AST_ROOT ;



int main( int argc, char **argv ) {	
	bool hasErrors;
	FILE *input;
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

	tree = BuildAst( input, &hasErrors );

	if ( input != stdin ) {
		fclose(input);
	}
	
	hasErrors = ! Symbols_annotate( tree );

	if ( ! hasErrors ) {
			IRCode code = buildIRCode( tree );
			IRCode_DumpToFile( code, stdout );
			/*printf("Valid Program!!!\n");
			AST_PrettyPrint( tree, 1 );*/

	}

	AST_Free( tree );

	return hasErrors == true;
}




#endif //AST_H