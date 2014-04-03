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
/* /* */

TokenList generateTokens(FILE* f, int* status) {
	Token t = NULL; 
	TokenList tl = NULL	;
	TokenKind b;
	long aux;
	char c;
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
				error_flag = 1;
				t=NULL;
				printf("    Error at line %d near to %s\n", line_num, yytext);
				break;
			default:
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

