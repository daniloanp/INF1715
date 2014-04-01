#include <stdlib.h>
#include <stdio.h>
#include "../trab1/token.h"
#include "../trab1/tokenList.h"

static error_flag = 0;
/*

*/

static TokenList expression(TokenList tl);
static TokenList expressionList(TokenList tl);
static TokenList commandAttrOrCall( TokenList tl );
static TokenList block( TokenList tl );
static TokenList command( TokenList tl );
static TokenList arrayAccess( TokenList tl );

static  int verifyCurrentToken(TokenList tl, TokenKind tk) {
	if (tl == NULL)
		return 0;
	Token t = tokenListGetCurrentToken( tl );
	return tokenGetKind( t ) == tk;
}

static  TokenList processTerminal( TokenList tl, TokenKind tk ) {
	Token t = tokenListGetCurrentToken(tl);
	if(!error_flag && tl == NULL) {
		printf("Expected <%s> but <EOF>\n", tokenKindToString(tk) );
		error_flag++;
	}
	if(tl == NULL)

		return NULL;
	

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


static TokenList type( TokenList tl ) {
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
			tl = type( tl ) ;
			break;
		default:
			break;
	}
	return tl;
}


static TokenList declGlobalVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = type(tl);
	return processTerminal( tl, NL );
}

static TokenList param(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	tl = processTerminal(tl, COLON);
	return type(tl);
}

static TokenList params(TokenList tl) {
	Token t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case CL_PARENTHESIS: //FOLLOW
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
		tl = type( tl );
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


static TokenList varOrCall(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	if( verifyCurrentToken( tl, OP_PARENTHESIS) ) {
		tl = expressionList(tl);
	}
	else {
		tl = arrayAccess(tl);
	}
}



static TokenList new(TokenList tl) {
	tl = processTerminal(tl, NEW);
	tl = processTerminal(tl, OP_BRACKET);
	tl = expression(tl);
	tl = processTerminal(tl, CL_BRACKET);
	tl = type(tl);
	return tl;
}

static  TokenList F(TokenList tl) {
	Token t;
	int i=0;
	t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case OP_PARENTHESIS:
			/*for(i=0; verifyCurrentToken(tl, OP_PARENTHESIS);
				i++) {
				tl = processTerminal( tl, OP_PARENTHESIS );;
			}*/
			tl = processTerminal( tl, OP_PARENTHESIS );
			tl = expression(tl);
			tl = processTerminal( tl, CL_PARENTHESIS );
			/*for (i;
				i;
				--i) {
					tl = processTerminal( tl, CL_PARENTHESIS );
				}
			
			*/
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
			tl = varOrCall( tl );
			break;
		case NEW:
			tl = new(tl);
			break;
		default:
			error_flag++;
			printf( "Line %d.\nAn expression was expected but got %s.\n", tokenGetLine(t),  tokenToString(t) );
			tl = NULL;
			break;
	}
	return tl;
}

static TokenList U(TokenList tl) {
	Token t;
	if ( verifyCurrentToken( tl, MINUS)) {
		tl = processTerminal(tl, MINUS);
		return U(tl);
	}
	else if ( verifyCurrentToken(tl, NOT) ) {
		tl = processTerminal(tl, NOT);
		return U(tl);
	}
	else {
		tl = F(tl);	
	}
	
	return tl;
			
}

static TokenList T(TokenList tl) {
	Token t;
	tl = U(tl);
	t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case MUL:case DIV:
			tl = processTerminal( tl, tokenGetKind( t ) );
			tl = T( tl );	
			break;
		default:
			 break;
 	}
	return tl;
}


static TokenList E( TokenList tl) {
	Token t;
	tl = T( tl );
	t = tokenListGetCurrentToken(tl);
	switch( tokenGetKind(t) ) {
		case PLUS: case MINUS:
			tl = processTerminal( tl, tokenGetKind( t ) );
			tl = E( tl );
		default: 
			break;
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
			tl = C( tl );
			break;
		default:
			break;
	}
	return tl;
}
	

static TokenList expression( TokenList tl ) {
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

/*
expressionList -> expression { ',' expression }
*/
static TokenList expressionList( TokenList tl ) {
	tl = expression( tl );
	while(verifyCurrentToken(tl, COMMA)) {
		tl = processTerminal(tl, COMMA);
		tl = expression(tl);
	}

	return tl;
}

/*
call -> 	'(' expressionList ')'
*/
static TokenList call( TokenList tl ) {
	tl = processTerminal(tl, OP_PARENTHESIS );
	tl = expressionList( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	//tl = processTerminal( tl, NL );
	return tl;
}

/*
attr -> 	arrayAccess '=' expression
*/
static TokenList attr( TokenList tl ) {
	tl = arrayAccess(tl);
	tl = processTerminal( tl, EQUAL );
	tl = expression( tl );

	//tl = processTerminal( tl, NL );
	return tl;
}

/*
arrayAccess -> 	{ '['  expression']' }
*/
static TokenList arrayAccess( TokenList tl) {
	while( verifyCurrentToken( tl, OP_BRACKET ) ) {
		tl = processTerminal( tl, OP_BRACKET );
		tl = expression( tl );
		tl = processTerminal( tl, CL_BRACKET );
	}
	return tl;
}

/*
commandAttrOrCall -> attr | call
*/
static TokenList commandAttrOrCall( TokenList tl ) {
	if( verifyCurrentToken(tl, EQUAL) || verifyCurrentToken(tl, OP_BRACKET) ) {
		tl = attr( tl );	
	}
	else if ( verifyCurrentToken(tl, OP_PARENTHESIS) ) {
		tl = call(tl);
	}
	else {
		error_flag++;
		printf("Expected a function call or a variable declaration, but got %s", tokenToString(tokenListGetCurrentToken(tl)));
		tl = NULL;
	}
	return tl;
}
/*
commandWhile -> 'WHILE' expression'NL'
					block
				'LOOP'
*/
static TokenList commandWhile( TokenList tl ) {
	tl = processTerminal(tl, WHILE);
	tl = expression(tl);
	tl = processTerminal(tl, NL);
	tl = block(tl);
	tl = processTerminal(tl, LOOP);
	return tl;
}

/*
commandReturn -> 'RETURN' [ 'expression' ]
*/
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


/*
commandIF->'IF' expression 'NL'
				block	
			{ 
			'ELSE' 'IF' expression
				block }
			[
			'ELSE'
				block
			]
			'END'
*/
static TokenList commandIf( TokenList tl ) {
	tl = processTerminal( tl, IF );
	tl = expression( tl );
	tl =  processTerminal( tl, NL );
	tl = block( tl );
	// PROBLEM
	while( verifyCurrentToken( tl, ELSE ) ) {
		tl = processTerminal( tl, ELSE);
		if(verifyCurrentToken (tl, IF)) {
			tl = processTerminal( tl, IF );
			tl = expression( tl );
			tl = processTerminal( tl, NL);
			tl = block(tl);
		}
		else{
			tl = processTerminal( tl, NL);
			tl = block(tl);
			break;
		}
		
	}
	
	tl = processTerminal(tl, END);
	return tl;

}

/*

command -> 'ID' commandAttrOrCall 'NL'
command -> commandWhile 'NL'
command -> commandIf 'NL'
command -> commandReturn 'NL'

*/
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
	return processTerminal( tl, NL );
};


/*block -> [ 'ID' declOrCommand ]
		 { command }*/
static TokenList block( TokenList tl ) {
	TokenList tlf;
	Token t;
	if ( verifyCurrentToken( tl, IDENTIFIER ) ) {
		tl = processTerminal( tl, IDENTIFIER );
		tl = declOrCommand( tl );//Recursividade
	}
	tlf = NULL;
	while( tlf!=tl && tl ) {
		t = tokenListGetCurrentToken( tl );	
		tlf = tl;
		switch( tokenGetKind(t) ) {
			case ELSE: case END: case LOOP:
				break;
			default:
				tl = command( tl );
				break;
		}	
	}
	return tl;
}
/*
declFunction -> 'FUN' 'ID' '(' params ')' [ ':' type ] 'NL'
					block
				'END' 'NL'

*/
static TokenList declFunction( TokenList tl ) {
	tl = processTerminal( tl, FUN );
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, OP_PARENTHESIS );
	tl = params( tl );
	tl = processTerminal( tl, CL_PARENTHESIS );
	if( verifyCurrentToken( tl, COLON ) ) {
		tl = processTerminal( tl, COLON );
		tl = type( tl );
	}
	tl = processTerminal(tl, NL);
	tl = block( tl );
	tl = processTerminal(tl, END);
	tl = processTerminal(tl, NL);
	//Temporary, we need make decision about that
	//tl = processTerminalIfCurrent(tl, NL);
	
	return tl;
}



/*
decl -> declGlobalVar start 
decl -> declFunction start
*/
static TokenList decl( TokenList tl ) {
	Token t;
	t = tokenListGetCurrentToken( tl );
	switch ( tokenGetKind(t) ) {
		case IDENTIFIER:
			tl = declGlobalVar( tl );
			break;
		case FUN: 
			tl = declFunction( tl );
			break;
		default:
			error_flag++;
			printf("Error at line %d. Expected 'fun' or identifier but got %s", tokenGetLine(t), tokenToString(t));
			break;
			
	}
	//}
	return tl;
}


/*
program -> {NL} decl {decl};

*/
TokenList program( TokenList tl ) {
	Token t;
	while( tl ) {
		// "{NL}"
		while( verifyCurrentToken(tl, NL)) {
			tl = processTerminal( tl, NL );
		}

		t = tokenListGetCurrentToken( tl );
		switch ( tokenGetKind( t ) ) {
			case IDENTIFIER: case FUN:
				tl = decl( tl );
				break;
			default:
				tl=NULL; // loop protection
				break;
		}
	}
	return tl;
}