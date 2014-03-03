#include "lex.yy.h"
#include "token.h"
#include "tokenList.h"
#include <stdlib.h>

extern int done;
extern int line_num;
extern int nline;

char header[] ="  Passou na Análise Léxica Sem Erros!!! \n\t"; 

char footer[] ="\n\n\t*Constantes(Identificadores, Strings, Inteiros, Caracteres e Booleanos) são impressos como seus valores armazenados\n\t**Aspas e apóstrofos não fazem parte de Strings ou caracteres,respectivamente. Estas são impressas somente pra melhorar a visualização.\n\t***Booleanos são prefixados de $ para melhoras a visualização\n\t"; 

//Auxiliar Function that count how many times a char exists into a string
int countCharOccurences(char *s, char c)
{
	int i=0;
	for(;*s;s++)
		if(*s==c)
			i++;
	return i;
}



//Funtion that print a Token
int printToken(Token t)
{
	static int line=-1;
	int tline = tokenGetLine(t);
	
	if( line != tline )
	{
		printf("\n%5d| ",tline);
		line = tline;
	}
	
	switch ( tokenGetKind(t) ) {	
		case INT_VAL:
			printf("%d", tokenGetNumberValue(t));
			break;
		case STRING_VAL:
			printf("\"%s\"", tokenGetStringValue(t));
			break;
		case IDENTIFIER:
			printf("%s" , tokenGetStringValue(t));
			break;
		case CHAR_VAL:
			printf("'%c'", tokenGetCharValue(t));
			break;
		case BOOL_VAL:
			if(tokenGetNumberValue(t))
				printf("$true");
			else
				printf("$false");
			break;
		default:
			printf("%s", tokenToString(t));
			break;
	}
	printf(" ");
	

	//printf("%s::%d ", tokenGetNumberValue, t->kind);
}

int main( int argc, char **argv ) {
	Token t = NULL; 
	TokenList tl = NULL	;
	TokenKind b;
	long aux;
	char c;
	short error_flag = 0;
	++argv, --argc;
	if ( argc > 0 )
		yyin = fopen( argv[0], "r" );
	else
		yyin = stdin;

	while (!done) {
		b =  yylex();
		switch (b) {
			case NL:
				t = newToken((TokenKind)b, line_num, NULL);
				line_num+=countCharOccurences(yytext,b);
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
			case CHAR_VAL:
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
						case '\'':
							c = '\''; break;
						default: 
							exit(1); break;
					}
				t = newToken((TokenKind)b, line_num, createTokenCharValue(c) );
				break;
			case ERROR:
				error_flag = 1;
				t=NULL;
				printf("    Error at line %d near to %s\n", line_num, yytext);
				break;
			default:
				if(b)
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
	
	if( tl != NULL ) {
		if(!error_flag)
		{
			printf("%s",header);
			tokenListExecuteForEach(tl, printToken);
			printf("%s",footer);
		}
		tokenListDestroy(tl);
	}
	
	if( yyin != stdin)
		fclose(yyin);
	
	return 0;
}