#include "lex.yy.h"
#include "token.h"
#include "tokenList.h"
#include <stdlib.h>
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

int printToken(Token t)
{
	static int line=-1;
	int tline = tokenGetLine(t);
	
	if( line != tline )
	{
		printf("\n%3d| ",tline);
		line = tline;
	}
	if(tline == 1)
		line = 1;
	
	switch (tokenGetKind(t)) {
		
		case NUMBER:
			printf("%d ", tokenGetNumberValue(t));
			break;
		case STRING:
			printf("\"%s\" ", tokenGetStringValue(t));
			break;
		case IDENTIFIER:
			printf("%s " , tokenGetStringValue(t));
			break;
		case CHARACTER:
			printf("%c", tokenGetCharValue(t));
			break;
		default:
		break;	
	}
	printf("(%d)", tokenGetKind(t));

	//printf("%s::%d ", tokenGetNumberValue, t->kind);
}

int main( int argc, char **argv ) {
	Token t = NULL; 
	TokenList tl = NULL	;
	long aux;
	char c;
	const char *hexstring = "0xabcdef0";


	++argv, --argc;
	if ( argc > 0 )
		yyin = fopen( argv[0], "r" );
	else
		yyin = stdin;

	while (!done) {
		int b =  yylex();
		switch (b) {
			case NL:
				t = newToken((TokenKind)b, line_num, NULL);
				line_num+=countCharOccurences(yytext,b);
				break;
			case IDENTIFIER:
			case STRING:
				t = newToken((TokenKind)b, line_num, createTokenStringValue(yytext) );
				break;
			case NUMBER:
				if(strstr(yytext, "0x") != NULL)
					aux = strtol(yytext,NULL,0);
				else
					aux = atol(yytext);
				t = newToken((TokenKind)b, line_num, createTokenNumberValue( aux ));
				break;
			case BOOLEAN:
				aux = (strcmp(yytext, "false")==0)? 0 : 1;
				t = newToken((TokenKind)b, line_num, createTokenNumberValue(aux));
				break;
			case CHARACTER:
				c = *(yytext+1);
				if(c == '\\')
					switch (*(yytext+2)) {
						case 'n':
							c = '\n'; break;
						case 't':
							c = 't'; break;
						case '\\':
							c = '\\'; break;
						case '\"':
							c = '\"'; break;
						default: break;
					}
				t = newToken((TokenKind)b, line_num, createTokenCharValue(c) );
				break;
			case ERROR:
				t=NULL;
				printf("Error at line %d near to %s\n", line_num, yytext);
				break;

			default:
				if(b)
					t = newToken((TokenKind)b, line_num, NULL);
		}
		if(t!=NULL)
			if(tl!=NULL)
				tokenListInsert(tl,t);
			else
				tl= tokenListCreate(t);
		t= NULL;
	}
	tokenListExecuteForEach(tl, printToken);
	tokenListDestroy(tl);
	return 0;
}


/*{

			line_num+=countCharOccurences(yytext,b);
			printf("\n%3d|",line_num);
		}
		else if(b==999)
			printf("ERROR:(%s)", yytext);
		else if(b!=0)
		{
			
			printf(" %s::%d ", yytext, b);
		}*/