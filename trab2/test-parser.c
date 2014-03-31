#include <stdlib.h>
#include <stdio.h>
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "recursiveParser.h"

int main( int argc, char **argv ) {	
	TokenList tl;
	FILE *input;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tl = generateTokens(input);
		tl = program(tl);

		if( input != stdin )
			fclose(input);
	}
	else {
		"Cannot open file";
	}
	return 0;
}