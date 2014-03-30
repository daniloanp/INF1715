#include <stdlib.h>
#include <stdio.h>
#include "../trab1/token.h"
#include "../trab1/tokenList.h"

static error_flag = 0;
/*

*/

TokenList expression(TokenList tl);
TokenList commandAttrOrCall( TokenList tl );
TokenList block( TokenList tl );



int verifyCurrentToken(TokenList tl, TokenKind tk) {
	Token t = tokenListGetCurrentToken( tl );
	return tokenGetKind( t ) == tk;
}

TokenList processTerminal( TokenList tl, TokenKind tk ) {
	Token t = tokenListGetCurrentToken(tl);

	if( tokenGetKind(t) == tk ) {
		printf("\nToken: %s\n", tokenToString(t));
		return tokenListNext( tl );
	}
	else {
		error_flag++;

		printf("Error(27): ");
		printf("<%s> but <%s>\n", tokenToString(t), tokenKindToString(tk));
		return NULL;
	}
}

TokenList processTerminalIfCurrent( TokenList tl, TokenKind tk) {
	if( verifyCurrentToken(tl, tk) ) {
		processTerminal(tl, tk);
	}	
	return tl;
}

TokenList tipoBase( TokenList tl ) {
	return tokenListNext(tl);
}

TokenList tipo( TokenList tl ) {
	Token t = tokenListGetCurrentToken( tl );
	switch( tokenGetKind(t) ) {
		case INT: case STRING: case CHAR: case BOOL:
			return tipoBase( tl );
		case OP_BRACKET:
			tl=tokenListNext( tl );
			t  = tokenListGetCurrentToken( tl );
			if( tokenGetKind( t ) != CL_BRACKET ) {
				return NULL;
			}	
			return tipo(tokenListNext(tl));
			break;
		default:
			return tl;
	}
}


TokenList declVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = tipo(tl);
	return processTerminal( tl, NL);
}

TokenList param(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	tl = processTerminal(tl, COLON);
	return tipo(tl);
}

TokenList params(TokenList tl) {
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

TokenList declOrCommand( TokenList tl ) {
	if( verifyCurrentToken( tl, COLON ) )	{
		tl = processTerminal( tl, COLON );
		tl = tipo( tl );
		tl = processTerminal( tl, NL );
		if( verifyCurrentToken( tl, IDENTIFIER ) ) {
			tl = processTerminal( tl, IDENTIFIER );
			return declOrCommand(tl);
		}	
		return tl;
	}
	else {
		return commandAttrOrCall(tl);	
	}
}

TokenList F(TokenList tl) {
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
			break;
		default:
			error_flag++;
			printf("Error(141)\n");
			return NULL;
	}
	return tl;
}

TokenList T(TokenList tl) {
	tl = F(tl);
	if ( verifyCurrentToken( tl, MUL ) ) {
		tl = processTerminal( tl, MUL );
		tl = T( tl );
	}
	else if ( verifyCurrentToken( tl, DIV ) ) {
		tl = processTerminal( tl, DIV );
		tl = T( tl );
	}
	return tl;
}


TokenList E( TokenList tl) {
	tl = T( tl );
	if ( verifyCurrentToken( tl, PLUS ) ) {
		processTerminal( tl, PLUS );
		tl = E(tl);
	}
	else if ( verifyCurrentToken( tl, MINUS ) ) {
		processTerminal( tl, MINUS );
		tl = E( tl );
	}

	return tl;
}

TokenList C( TokenList tl) {
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
			tl = C( tl );
			break;
		default:
			break;
	}
	return tl;
}
	

TokenList expression(TokenList tl) {
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

TokenList expressionList( TokenList tl ) {
	tl = expression( tl );
	while(verifyCurrentToken(tl, COMMA)) {
		processTerminal(tl, COMMA);
		tl = expression(tl);
	}

	return tl;
}
TokenList call( TokenList tl ) {
	tl = processTerminal(tl, OP_PARENTHESIS );
	tl = expressionList( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	tl = processTerminal( tl, NL );
	return tl;
}

TokenList arrayBrackets( TokenList tl) {
	while( verifyCurrentToken( tl, OP_BRACKET ) ) {
		tl = processTerminal( tl, OP_BRACKET );
		tl = expression( tl );
		tl = processTerminal( tl, CL_BRACKET );
	}
	return tl;
}

TokenList commandAttrOrCall( TokenList tl ) {
	if( verifyCurrentToken(tl, EQUAL) || verifyCurrentToken(tl, OP_BRACKET) ) {
		tl = arrayBrackets(tl);
		tl = processTerminal( tl, EQUAL );
		tl = expression( tl );
		tl = processTerminal( tl, NL ); 
	}
	else if ( verifyCurrentToken(tl, OP_PARENTHESIS) ) {
		tl = call(tl);
	}
	else {
		error_flag++;
		printf("Error(250)\n");
		return NULL;
	}
	return tl;
}

TokenList commandWhile( TokenList tl ) {
	tl = processTerminal(tl, WHILE);
	tl = expression(tl);
	return processTerminal(tl, NL);
}

TokenList commandReturn( TokenList tl ) {
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
	
	return processTerminal( tl, NL );
}

TokenList commandIf( TokenList tl ) {
	tl = processTerminal( tl, IF );
	tl = expression( tl );
	tl =  processTerminal( tl, NL );
	tl = block( tl );

}
TokenList command( TokenList tl ) {
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
		case NL:
			tl = processTerminal( tl, NL );
		default:
			error_flag++;
			Token t = tokenListGetCurrentToken( tl );
			printf( "\nToken: %s ", tokenToString(t));
			printf( "Error(293)\n" );
			return NULL;
			break;
	}
	return tl;
};

TokenList block( TokenList tl ) {
	Token t = tokenListGetCurrentToken( tl );
	if ( verifyCurrentToken( tl, IDENTIFIER ) ) {
		tl = processTerminal( tl, IDENTIFIER );
		tl = declOrCommand( tl );
	}
	while( !verifyCurrentToken( tl, END ) ) {
		tl = command( tl );
	} 
	return tl;
}

TokenList declFunction( TokenList tl ) {
	tl = processTerminal( tl, FUN );
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, OP_PARENTHESIS );
	tl = params( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	if( verifyCurrentToken( tl, COLON ) ) {
		tl = processTerminal( tl, COLON );
		tl = tipo( tl );
	}
	tl=processTerminal(tl, NL);
	tl = block( tl );
	tl=processTerminal(tl, END);

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
		tl = tokenListNext( tl );
	}
	return tl;
}