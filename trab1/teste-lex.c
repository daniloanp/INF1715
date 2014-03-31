#include "token.h"
#include "tokenList.h"
#include <stdlib.h>
#include <stdio.h>
#include "lex.h"


char header[] ="  Passou na Análise Léxica Sem Erros!!! \n\t"; 

char footer[] ="\n\n\t*Constantes(Identificadores, Strings, Inteiros, Caracteres e Booleanos) são impressos como seus valores armazenados\n\t**Aspas e apóstrofos não fazem parte de Strings ou caracteres,respectivamente. Estas são impressas somente pra melhorar a visualização.\n\t***Booleanos são prefixados de $ para melhoras a visualização\n\t"; 

//Funtion that print a Token
int printToken(Token t)
{
	static int line=-1;
	int tline = tokenGetLine(t);
	
	if( line != tline )	{
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
	
}

int main( int argc, char **argv ) {	
	TokenList tl;
	FILE *input;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	tl = generateTokens(input);
	if ( tl != NULL ) {
		printf("%s",header);
		tokenListExecuteForEach(tl, printToken);
		printf("%s",footer);
		tokenListDestroy(tl);
	}
	
	if( input != stdin )
		fclose(input);
	return 0;
}