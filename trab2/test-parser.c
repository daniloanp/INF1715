#include <stdlib.h>
#include <stdio.h>
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "recursiveParser.h"

int main( int argc, char **argv ) {	
	int ret;
	TokenList tl;
	FILE *input = NULL;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tl = generateTokens(input, &ret);
		if(!ret) {
			ret = parser(tl, NULL);
		}

		if( input != stdin ) {
			fclose(input);
		}
	}
	else {
		printf("\nError: Cannot open file.\n");
		return 1;
	}

	if(ret == 0 ) {
		printf("Correct Syntax!!!\n");
		tokenListDestroy(tl);
	}


	return ret;
}