#include "lex.yy.h"
#include "token.h"
extern int done;
extern int line_num;
extern int nline;
int countCharOccurences(char *s, char c)
{
	int i=0;
	for(;*s;s++)
		if(*s==c)
			i++;
	return i;
}


int main( int argc, char **argv ) {
	++argv, --argc;
	if ( argc > 0 )
		yyin = fopen( argv[0], "r" );
	else
		yyin = stdin;
	printf("\n%3d|",line_num);
	while (!done) {

		int b =  yylex();
		if(b==NL)
		{

			line_num+=countCharOccurences(yytext,b);
			printf("\n%3d|",line_num);
		}
		else if(b==999)
			printf("ERROR:(%s)", yytext);
		else if(b!=0)
		{
			
			printf(" %s::%d ", yytext, b);
		}
		
		

	}
	return 0;
}