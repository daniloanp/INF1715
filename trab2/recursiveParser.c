#include "../trab1/token.h"
#include "../trab1/tokenList.h"

static error_flag = 0;
/*

*/

int verifyCurrentToken(TokenList tl, TokenKind tk)
{
	Token t = tokenListGetCurrentToken(tl);
	return tokenGetKind(t) == tk;
}

TokenList processTerminal(TokenList tl, TokenKind tk)
{
	Token t = tokenListGetCurrentToken(tl);
	if( tokenGetKind(t) == tk ) {
		return tokenListNext(tl);
	} 
	else {
		error_flag++;
		printf("Error");
		return NULL;
	}
}

TokenList tipoBase(TokenList tl) {
	return tokenListNext(tl);
}

TokenList tipo(TokenList tl)
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


TokenList declVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = tipo(tl)
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

TokenList declOrCommand( TokenList tl ) {
	if( verifyCurrentToken( tl, COLON ) )	{
		tl = processTerminal( tl, COLON );
		tl = tipo( tl )
		tl = processTerminal( tl, NL );
		if( verifyCurrentToken( tl, ID ) ) {
			tl = processTerminal( tl, ID );
			return declOrCommand(tl);
		}	
		return tl;
	}
	else {
		return commandAttrOrCall(tl);	
	}
}

TokenList commandAttrOrCall( TokeList tl ) {

	if( verifyCurrentToken(tl, EQUAL) || verifyCurrentToken(tl, OP_BRACKET) ) {
		while( verifyCurrentToken( tl, OP_BRACKET ) ) {
			tl = processTerminal( tl, OP_BRACKET );
			tl = expression( tl );
			tl = processTerminal( tl, CL_BRACKET );
		}
		tl = processTerminal( tl, EQUAL );
		tl = expression( tl );
		tl = processTerminal( tl, NL ); 
	}
	else if ( verifyCurrentToken(tl, OP_PARENTHESIS) ) {
		tl = processTerminal(tl, OP_PARENTHESIS );
		tl = varList( tl );
		tl = processTerminal( tl, CL_PARENTHESIS );
		tl = processTerminal( tl, NL );
	}
	else {
		error_flag++;
		printf("Error");
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
	tl = processTerminal(tl, RETURN);
	tl = expression(tl);
	return processTerminal(tl, NL);
}

TokenList commandIf( TokenList tl ) {
	tl = processTerminal(tl, IF);
	tl = expression(tl);
	tl =  processTerminal(tl, NL);
	tl = bloco(tl);

}

TokenList command(TokenList tl) {
	Token t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case WHILE:
			tl = commandWhile(tl);
			break;
		case RETURN:
			tl = commandReturn(tl);
			break
		case IF:
			tl = commandIf(tl);
			break;
		case IDENTIFIER:
			tl = processTerminal(tl, IDENTIFIER);
			tl = commandAttrOrCall(tl);
			break;
		default:
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
		tl = command(tl);
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
	tl = block(TL);
	tl=processTerminal(tl, END);
	tl=processTerminal(tl, NL);
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