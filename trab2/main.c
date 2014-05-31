#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/mini0-lex.h"
#include "../trab1/token_list.h"
#include "recursive_parser.h"

int main( int argc, char **argv ) {	
	bool hasErrors = false;
	TokenList tl;
	FILE *input = NULL;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tl = generateTokens( input, &hasErrors );
		if ( !hasErrors ) {
			hasErrors = parser(tl, NULL, NULL, NULL);
		}

		if( input != stdin ) {
			fclose(input);
		}
	}
	else {
		printf("\nError: Cannot open file.\n");
		return 1;
	}

	if( !hasErrors ) {
		printf("Correct Syntax!!!\n");
		TokenList_Destroy( tl );
		return 0;
	} else {
		return 1;
	}
}