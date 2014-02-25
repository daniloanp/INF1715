#include "lex.yy.h"
extern int done;

int main( int argc, char **argv ) {
	++argv, --argc;
	if ( argc > 0 )
		yyin = fopen( argv[0], "r" );
	else
		yyin = stdin;
	while (!done) {
		int b =  yylex();
		if(b==999&&b!=0)
			printf("ERROR: ");
		if(b!=0)
		printf("%s::%d\n", yytext, b);
	}

	return 0;
}