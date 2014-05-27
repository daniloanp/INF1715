#include "token.h"
#include "token_list.h"
#include <stdlib.h>
#include <stdio.h>
#include "mini0-lex.h"


char header[] ="  Passou na Análise Léxica Sem Erros!!! \n\t"; 

char footer[] ="\n\n\t*Constantes(Identificadores, Strings, Inteiros, Caracteres e Booleanos) são impressos como seus valores armazenados\n\t**Aspas e apóstrofos não fazem parte de Strings ou caracteres,respectivamente. Estas são impressas somente pra melhorar a visualização.\n\t***Booleanos são prefixados de $ para melhoras a visualização\n\t"; 

//Funtion that print a Token
int printToken(Token t)
{
	static int line=-1;
	int tline = Token_GetLine( t );
	
	if( line != tline )	{
		printf("\n%5d| ", tline);
		line = tline;
	}
	
	switch ( Token_GetKind(t) ) {
		case TK_INT_VAL:
			printf( "%lu", Token_GetIntValue( t ) );
		break;

		case TK_STRING_VAL:
			printf( "\"%s\"", Token_GetStringValue( t ) );
		break;

		case TK_IDENTIFIER:
			printf( "%s" , Token_GetStringValue(t) );
		break;

		case TK_BOOL_VAL:
			if( Token_GetIntValue( t ) ) {
				printf( "$true" );
			}
			else {
				printf( "$false" );
			}
		break;

		default:
			printf( "%s", Token_GetStringForKind( t ) );
		break;
	}
	printf(" ");
	return 1;
}

int main( int argc, char **argv ) {	
	TokenList tl;
	FILE *inputFile;
	bool hasErrors = false;
	++argv, --argc;
	if ( argc > 0 ) {
		inputFile = fopen( argv[0], "r" );
	}
	else {
		inputFile = stdin;
	}
	tl = generateTokens( inputFile, &hasErrors );
	if ( tl != NULL ) {
		printf( "%s", header );
		TokenList_ExecuteForEach( tl, printToken );
		printf( "%s", footer );
		TokenList_Destroy( tl );
	}
	
	if( inputFile != stdin ) {
		fclose(inputFile);
	}
	return hasErrors;
}