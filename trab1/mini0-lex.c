#include "lex.yy.h"
#include "token.h"
#include "token_list.h"
#include "mini0-lex.h"
#include <stdlib.h>
#include <stdio.h>

extern int done;
extern int line_num;
extern int nline;

static unsigned int countCharOccurences(char *s, char c) {
	unsigned int i=0;
	for ( ; *s; s++ ) {
		if( *s == c ) {
			i++;
		}
	}
	return i;
}

TokenList generateTokens(FILE* f, bool* errors) {
	Token t = NULL; 
	TokenList tl = NULL	;
	unsigned int b;
	unsigned long int_val;
	bool bool_val;
	bool hasErrors = false;
	yyin = f; // Set the yyin, whom it's the GNU Flex input file.

	while (!done) {
		b =  yylex();
		switch (b) {
			case 0: break; //Just escape
			case TK_NL:
				t = Token_New( TK_NL, line_num );
				line_num += countCharOccurences( yytext,'\n' );
			break;

			case TK_IDENTIFIER:
			case TK_STRING_VAL:
				t = Token_NewAsString( (TokenKind)b, line_num, yytext );
			break;

			case TK_INT_VAL:
				if( strstr(yytext, "0x") != NULL ) {
					int_val = strtol(yytext,NULL,0);
				}
				else {
					int_val = atol(yytext);
				}
				t = Token_NewAsInteger( TK_INT_VAL, line_num,  int_val );
			break;

			case TK_BOOL_VAL:
				bool_val = (strcmp(yytext, "false")==0)? false : true;
				t = Token_NewAsBool( (TokenKind)b, line_num, bool_val );
				// - createTokenNumberValue(aux)
			break;

			case TK_ERROR:
				hasErrors = true;
				t=NULL;
				printf( "Lexical Error at line %d.\n", line_num );
			break;

			default:
				if( b > 0 && b < 37) { //Valid Token
					t = Token_New( (TokenKind)b, line_num );
				}
				else { 
					printf("The value %d doesn't represent any valid Token", b);
					hasErrors = true;
				}
			break;

		}
		if( t != NULL ) {
			tl = TokenList_Insert( tl, t );
		}
		else {
			break;
		}
		t = NULL;
	}
	if( hasErrors  )  {
		TokenList_Destroy( tl );
		*errors = true;
		return (TokenList)NULL;
	}
	else {
		errors = false;
		tl = TokenList_GetFirst( tl );
		return tl;
	}
}

