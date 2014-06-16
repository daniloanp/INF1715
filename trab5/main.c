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


char * getOutPutFileName( char *inputf ) {
	char *s,*c;
	s = (char*)malloc( sizeof(char)*(strlen( inputf )+4));
	strcpy( s, inputf);
	
	for(c = s;*c; c++);
	*c = '.'; c++;
	*c = 'i'; c++;
	*c = 'r'; c++;
	*c = '\0'; c++;
	return s;
}
int main( int argc, char **argv ) {
	bool hasErrors;
	FILE *input;
	FILE *output;
	AST tree;
	char *outputName;

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
	
	hasErrors = hasErrors || (! Symbols_annotate( tree ));

	if ( ! hasErrors ) {
			printf( "oi ");
			IRCode code = buildIRCode( tree );
			outputName = getOutPutFileName( argv[0] );
			output = fopen( outputName, "w" );
			free( outputName );
			IRCode_DumpToFile( code, output );
			fclose( output );
	}

	AST_Free( tree );

	return hasErrors == true;
}




#endif //AST_H