#include <stdlib.h>
#include <stdio.h>
#include "../trab1/token.h"
#include "../trab1/tokenList.h"

static error_flag = 0;
static int b = 0;
/*

*/

static TokenList expression(TokenList tl);
static TokenList commandAttrOrCall( TokenList tl );
static TokenList block( TokenList tl );
static TokenList command( TokenList tl );
static TokenList arrayBrackets( TokenList tl);

static  int verifyCurrentToken(TokenList tl, TokenKind tk) {
	if (tl == NULL)
		return 0;
	Token t = tokenListGetCurrentToken( tl );
	return tokenGetKind( t ) == tk;
}

static  TokenList processTerminal( TokenList tl, TokenKind tk ) {
	if(tl == NULL)
		return NULL;
	Token t = tokenListGetCurrentToken(tl);

	if( tokenGetKind(t) == tk ) {
		return tokenListNext( tl );
	}
	else {
		error_flag++;
		printf("Line %d.\nExpected <%s> but got <%s>\n", tokenGetLine(t), tokenKindToString(tk),  tokenToString(t));
		return NULL;
	}
}

static  TokenList processTerminalIfCurrent( TokenList tl, TokenKind tk) {
	if( verifyCurrentToken(tl, tk) ) {
		processTerminal(tl, tk);
	}	
	return tl;
}


static TokenList tipo( TokenList tl ) {
	Token t = tokenListGetCurrentToken( tl );
	switch( tokenGetKind(t) ) {
		case INT: case STRING: case CHAR: case BOOL:
			tl = processTerminal(tl, tokenGetKind(t));
			break;
		case OP_BRACKET:
			while( verifyCurrentToken( tl, OP_BRACKET ) ) {
				tl = processTerminal( tl, OP_BRACKET );
				tl = processTerminal( tl, CL_BRACKET );	
			}
			tl = tipo( tl ) ;
			break;
		default:
			break;
	}
	return tl;
}


static TokenList declVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = tipo(tl);
	return processTerminal( tl, NL );
}

static TokenList param(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	tl = processTerminal(tl, COLON);
	return tipo(tl);
}

static TokenList params(TokenList tl) {
	Token t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case CL_PARENTHESIS:
			return tl;
			break;
		case  IDENTIFIER:
			tl = param(tl);
			//pay attention, loop
			while( verifyCurrentToken(tl, COMMA) ) {
				tl = processTerminal(tl, COMMA);
				tl = param(tl);
			}
			break;
		default:
			printf("Error(83)\n");
			break;
	}
	return tl;
}

static  TokenList declOrCommand( TokenList tl ) {
	if( verifyCurrentToken( tl, COLON ) )	{
		tl = processTerminal( tl, COLON );
		tl = tipo( tl );
		tl = processTerminal( tl, NL );
		if( verifyCurrentToken( tl, IDENTIFIER ) ) {		
			tl = processTerminal( tl, IDENTIFIER );
			tl = declOrCommand(tl);
		}	
		return tl;
	}
	else {
		tl = commandAttrOrCall(tl);	
		return processTerminal(tl, NL);
	}
}

static  TokenList F(TokenList tl) {
	Token t;
	t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case OP_PARENTHESIS:
			tl = processTerminal( tl, OP_PARENTHESIS );
			tl = expression(tl);
			tl = processTerminal( tl, CL_PARENTHESIS );
			break;
		case BOOL_VAL:
			tl = processTerminal(tl, BOOL_VAL);
			break;
		case INT_VAL:
			tl = processTerminal(tl, INT_VAL);
			break;
		case STRING_VAL:
			tl = processTerminal(tl, STRING_VAL);
			break;
		case IDENTIFIER:
			tl = processTerminal(tl, IDENTIFIER);
			tl = arrayBrackets(tl);
			break;
		case NEW:
			tl = processTerminal(tl, IDENTIFIER);
			tl = processTerminal(tl, OP_BRACKET);
			tl = expression(tl);
			tl = processTerminal(tl, CL_BRACKET);
			tl = tipo(tl);
			break;
		case MINUS:
			tl = processTerminal(tl, MINUS);
			tl = expression(tl);
			break;
		case NOT:
			tl = processTerminal(tl, NOT);
			tl = expression(tl);
			break;
		default:
			error_flag++;
			printf("Error(156)\n");
			tl = NULL;
			break;
	}
	return tl;
}

static TokenList T(TokenList tl) {
	tl = F(tl);
	if ( verifyCurrentToken( tl, MUL ) ) {
		tl = processTerminal( tl, MUL );
		tl = expression( tl );
	}
	else if ( verifyCurrentToken( tl, DIV ) ) {
		tl = processTerminal( tl, DIV );
		tl = expression( tl );
	}
	return tl;
}


static TokenList E( TokenList tl) {
	tl = T( tl );
	if ( verifyCurrentToken( tl, PLUS ) ) {
		tl = processTerminal( tl, PLUS );
		tl = expression(tl);
	}
	else if ( verifyCurrentToken( tl, MINUS ) ) {
		tl = processTerminal( tl, MINUS );
		tl = expression( tl );
	}

	return tl;
}

static TokenList C( TokenList tl) {
	Token t;
	tl = E( tl );
	t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case GREATER:
		case GREATER_EQUAL:
		case LESS:
		case LESS_EQUAL:
		case EQUAL:
		case DIFFERENT:
			tl = processTerminal( tl, tokenGetKind(t) );
			tl = expression( tl );
			break;
		default:
			break;
	}
	return tl;
}
	

static TokenList expression(TokenList tl) {
	if(tl == NULL)
		return NULL;
	Token t;
	tl = C( tl );
	t = tokenListGetCurrentToken( tl );
	switch( tokenGetKind(t) ) {
		case AND:
		case OR:
			tl = processTerminal( tl, tokenGetKind(t) );
			tl = expression( tl );
			break;
		default:
			break;
	}
	return tl;
}

static TokenList expressionList( TokenList tl ) {
	tl = expression( tl );
	while(verifyCurrentToken(tl, COMMA)) {
		tl = processTerminal(tl, COMMA);
		tl = expression(tl);
	}

	return tl;
}
static TokenList call( TokenList tl ) {
	tl = processTerminal(tl, OP_PARENTHESIS );
	tl = expressionList( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	//tl = processTerminal( tl, NL );
	return tl;
}

static TokenList arrayBrackets( TokenList tl) {
	while( verifyCurrentToken( tl, OP_BRACKET ) ) {
		tl = processTerminal( tl, OP_BRACKET );
		tl = expression( tl );
		tl = processTerminal( tl, CL_BRACKET );
	}
	return tl;
}

static TokenList commandAttrOrCall( TokenList tl ) {
	if( verifyCurrentToken(tl, EQUAL) || verifyCurrentToken(tl, OP_BRACKET) ) {
		tl = arrayBrackets(tl);
		tl = processTerminal( tl, EQUAL );
		tl = expression( tl );
	}
	else if ( verifyCurrentToken(tl, OP_PARENTHESIS) ) {
		tl = call(tl);
	}
	else {
		error_flag++;
		printf("Error(250)\n");
	}
	return tl;
}

static TokenList commandWhile( TokenList tl ) {
	printf("\tWHILE\n");
	tl = processTerminal(tl, WHILE);
	tl = expression(tl);
	tl = processTerminal(tl, NL);
	tl = block(tl);
	tl = processTerminal(tl, LOOP);
	printf("\tLOOP\n");
	return tl;
}

static  TokenList commandReturn( TokenList tl ) {
	Token t;
	tl = processTerminal( tl, RETURN );
	t = tokenListGetCurrentToken(tl);		
	switch( tokenGetKind( t ) ) {
		case IDENTIFIER:
		case INT_VAL:
		case STRING_VAL:
		case BOOL_VAL:
		case NEW:
		case OP_PARENTHESIS:
		case NOT:
		case MINUS:
			tl = expression( tl );
			break;
		default:
			break;
	}
	return tl;
}

static TokenList commandIf( TokenList tl ) {
	tl = processTerminal( tl, IF );
	tl = expression( tl );
	tl =  processTerminal( tl, NL );
	tl = block( tl );
	while( verifyCurrentToken( tl, ELSE ) ) {
		tl = processTerminal( tl, ELSE);
		if(verifyCurrentToken (tl, IF)) {
			tl = processTerminal( tl, IF );
			tl = expression( tl );
		}
		tl = processTerminal( tl, NL);
		tl = block(tl);
	}
	
	tl = processTerminal(tl, END);
	
	return tl;

}
static  TokenList command( TokenList tl ) {
	Token t = tokenListGetCurrentToken( tl );
	switch( tokenGetKind( t ) ) {
		case WHILE:
			tl = commandWhile( tl );
			break;
		case RETURN:
			tl = commandReturn( tl );
			break;
		case IF:
			tl = commandIf( tl );
			break;
		case IDENTIFIER:
			tl = processTerminal( tl, IDENTIFIER );
			tl = commandAttrOrCall( tl );
			break;
		default:
			error_flag++;
			Token t = tokenListGetCurrentToken( tl );
			printf( "\nToken: %s ", tokenToString(t));
			printf( "Line %d.\nExpected identifier, if, return or while but got %s.\n", tokenGetLine(t),  tokenToString(t) );
			return NULL;
			break;
	}
	//printf("<-FIM COMANDO->\n");
	return processTerminal( tl, NL );
};



static TokenList block( TokenList tl ) {
	printf("BLOCONIVEL: %d\n", ++b);
	TokenList tlf;
	Token t;
	if ( verifyCurrentToken( tl, IDENTIFIER ) ) {
		tl = processTerminal( tl, IDENTIFIER );
		tl = declOrCommand( tl );
	}
	tlf = NULL;
	t = tokenListGetCurrentToken( tl );
	switch( tokenGetKind(t) ) {
		case ELSE: 
		case END: 
		case LOOP:
			break;
		default:
			while( tlf!=tl && tl ) {
				tlf = tl;
				tl = command( tl );				
			}
		break;
	}
	
	
	--b;
	printf("ENDBLOCO\n");
	return tl;
}

static TokenList declFunction( TokenList tl ) {
	tl = processTerminal( tl, FUN );
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, OP_PARENTHESIS );
	tl = params( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	if( verifyCurrentToken( tl, COLON ) ) {
		tl = processTerminal( tl, COLON );
		tl = tipo( tl );
	}
	tl = processTerminal(tl, NL);
	tl = block( tl );
	tl = processTerminal(tl, END);

	//Temporary, we need make decision about that
	tl = processTerminalIfCurrent(tl, NL);
	return tl;
}


TokenList start( TokenList tl ) {
	Token t;
	//tl = tokenListGetFirst(tl);
	while(tl) {
		t = tokenListGetCurrentToken( tl );
		switch ( tokenGetKind(t) ) {
			case NL:
				processTerminal(tl, NL);
				break;
			case IDENTIFIER:
				tl = declVar( tl );
				break;
			case FUN: 
				tl = declFunction( tl );
				break;
			default:
				printf( "Error(343)\n" );
				tl=NULL;
				continue;
		}
	}
	return tl;
}