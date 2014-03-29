#include "../trab1/token.h"
#include "../trab1/tokenList.h"

static error_flag = 0;
/*

*/


/*TokenList walkTerminals(TokenList tl, int* list) {
	Token t;
	int i;
	for( i=0; list[i]; i++) {
		t = tokenListGetCurrentToken(t);
		if( tokenGetKind(t) != (TokenKind)list[i] ) {
			printf("Syntax error near to %d.\n", tokenGetLine(t));	
			*status = -1;
			return tl;
		}
		tl = tokenListNext(tl);	
	}
	return tl;
}*/


int verifyCurrentToken(TokenList tl, TokenKind tk)
{
	Token t = tokenListGetCurrentToken(tl);
	return tokenGetKind(t) == tk;
}

TokenList processTerminal(TokenList tl, TokenKind tk)
{
	Token t = tokenListGetCurrentToken(tl);
	if( tokenGetKind(t) == tk) {
		return tokenListNext(tl);
	} 
	else{
		error_flag++;
		return NULL;
	}
}

TokenList tipoBase(TokenList tl) {
	return tokenListNext(tl);
}



tokenList tipo(TokenList tl)
{
	Token t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case INT: case STRING: case CHAR: case BOOL:
			return tipoBase(tl);
		case OP_BRACKET:
			tl=tokenListNext(tl);
			t  = tokenListGetCurrentToken(tl);
			if( tokenGetKind(t) != CL_BRACKET ) {
				return NULL;
			}	
			return tipo(tokenListNext(tl));
			break;
		default:
			return tl;
	}
}

TokenList param(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	tl = processTerminal(tl, COLON);
	return tipo(tl);
}

TokenList params(TokenList tl) {
	Token t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case CL_PARANTHESIS:
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
			printf("Error");
			break;
	}
	return tl;
}


TokenList declFunction( TokenList tl ) {
	tl = processTerminal( tl, FUN );
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, OP_PARENTHESIS );
	tl = params(tl);
	tl = processTerminal( tl, CL_PARENTHESIS );
	if( )
	//tl = processTerminal( tl, FUN );
}

TokenList declVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = tipo(tl)
	return processTerminal( tl, NL);
}

TokenList start(TokenList tl) {
	Token t;
	//tl = tokenListGetFirst(tl);
	while(tl) {
		t = tokenListGetCurrentToken(tl);
		switch tokenGetKind(t) {
			case NL:
				break;
			case IDENTIFIER:
				tl = declVar(tl);
				break;
			case: FUN
				tl = declFunction(tl);
				break;
			default:
				printf("Erro");
				tl=NULL;
				continue;
		}
		tl = tokenListNext(tl);
	}

	return tl;
}