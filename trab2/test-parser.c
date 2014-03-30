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
	tl = generateTokens(input);
	tl = start(tl);

	if( input != stdin )
		fclose(input);
	return 0;
}