#include "lex.yy.h"
#include "token.h"
#include "tokenList.h"
#include "lex.h"
#include <stdlib.h>
#include <stdio.h>


extern int done;
extern int line_num;
extern int nline;
extern  int countCharOccurences(char *s, char c);

int countCharOccurences(char *s, char c) {
	int i=0;
	for( ; *s; s++ )
		if( *s == c )
			i++;
	return i;
}


TokenList generateTokens(FILE* f, int* status) {
	Token t = NULL; 
	TokenList tl = NULL	;
	TokenKind b;
	long aux;
	short error_flag = 0;
	yyin = f;
	*status = 0; //no error;
	while (!done) {
		
		b =  yylex();
		
		switch (b) {
			case NL:
				t = newToken((TokenKind)b, line_num, NULL);
				line_num+=countCharOccurences(yytext,'\n');
				break;
			case IDENTIFIER:
			case STRING_VAL:
				t = newToken((TokenKind)b, line_num, createTokenStringValue(yytext) );
				break;
			case INT_VAL:
				if(strstr(yytext, "0x") != NULL)
					aux = strtol(yytext,NULL,0);
				else
					aux = atol(yytext);
				t = newToken((TokenKind)b, line_num, createTokenNumberValue( aux ));
				break;
			case BOOL_VAL:
				aux = (strcmp(yytext, "false")==0)? 0 : 1;
				t = newToken((TokenKind)b, line_num, createTokenNumberValue(aux));
				break;
			case ERROR:
				//printf("Strange");
				error_flag = 1;
				t=NULL;
				printf("Error at line %d.\n", line_num);
				if( !strcmp("", yytext)) {
					printf("(Unkown token '%s')", yytext);
				}
				break;
			default:
				//printf("oops\n");
				if( (b >=1 && b <= 37) )
					t = newToken((TokenKind)b, line_num, NULL);
				break;
		}
		if(t!=NULL) {
			if(tl!=NULL)
				tokenListInsert(tl,t);
			else
				tl= tokenListCreate(t);
		} 
		else {
			break;
		}

		t = NULL;
	}
	if( error_flag )  {
		tokenListDestroy(tl);
		*status = error_flag;
		return (TokenList)NULL;
	}
	else {
		return tl;
	}
}

