#include "lex.yy.h"
extern int done;

int main() {
	while (!done) {
		int b =  yylex();
		if(b==999&&b!=0)
			printf("ERROR: ");
		if(b!=0)
		printf("%s::%d\n", yytext, b);
	}
	return 0;
}