#include <stdlib.h>
#include <stdio.h>
#include "../trab1/token.h"
#include "../trab1/tokenList.h"


//global_variable is more intuitive;
static error_flag = 0;

//it's a bit graceless, but I think that is more efficient because it allow decrease the stack use;
static int count_parenthesis = 0;


static TokenList expression(TokenList tl);
static TokenList expressionList(TokenList tl);
static TokenList commandAttrOrCall( TokenList tl );
static TokenList block( TokenList tl );
static TokenList command( TokenList tl );
static TokenList arrayAccess( TokenList tl );
static TokenList call( TokenList tl );
static TokenList U(TokenList tl);



// -- Auxiliar Functions:
static  int verifyCurrentToken(TokenList tl, TokenKind tk) {
	if (tl == NULL)
		return 0;
	Token t = tokenListGetCurrentToken( tl );
	return tokenGetKind( t ) == tk;
}

static void printError(int line, char *expected, char *got){
	error_flag++;
	printf("Error at Line %d: The expected was %s but got %s", line, expected, got);	

}

static  TokenList processTerminal( TokenList tl, TokenKind tk ) {
	Token t = tokenListGetCurrentToken(tl);
	if(!error_flag && tl == NULL) {
		error_flag++;
		printf("Unexpected End of File. The expected was <%s>\n", tokenKindToString(tk) );
		
	}
	if(tl == NULL) {
		error_flag++;
		return NULL;
	}	
	if( t!= NULL && tokenGetKind(t) == tk ) {
		printf("Token: %s, Line: %d\n", tokenToString(t), tokenGetLine(t));
		return tokenListNext( tl );
	}
	else {
		error_flag++;
		printError( tokenGetLine(t), tokenKindToString(tk),  tokenToString(t));
		return NULL;
	}
}


// end_auxiliar.


/*
type ->  'CHAR'
type -> 'STRING'
type -> 'INT'
type -> 'BOOL'
type -> '[' ']' type
*/
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
			printError(tokenGetLine(t), "a type", tokenToString(t));
			tl = NULL;
			break;
	}
	return tl;
}

/*
declGlobalVar -> 'ID' ':' type 'NL'
*/
static TokenList declGlobalVar(TokenList tl) {	
	tl = processTerminal( tl, IDENTIFIER );
	tl = processTerminal( tl, COLON );
	tl = type(tl);
	return processTerminal( tl, NL );
}
/*
param -> 'ID' ':' type
*/
static TokenList param(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	tl = processTerminal(tl, COLON);
	return type(tl);
}

/*
params -> param { ','' param }
*/
static TokenList params(TokenList tl) {
	tl = param(tl);
	//pay attention, loop
	while( verifyCurrentToken(tl, COMMA) ) {
		tl = processTerminal(tl, COMMA);
		tl = param(tl);
	}
	return tl;
}


/*
declOrCommand -> ':' type 'NL'
declOrCommand -> ':' type 'NL' 'ID' declOrCommand
declOrCommand ->  commandAttrOrCall 'NL'
*/
static  TokenList declOrCommand( TokenList tl ) {
	if( verifyCurrentToken( tl, COLON ) )	{
		tl = processTerminal( tl, COLON );
		tl = type( tl );
		tl = processTerminal( tl, NL );
		if( verifyCurrentToken( tl, IDENTIFIER ) ) {		
			tl = processTerminal( tl, IDENTIFIER );
			tl = declOrCommand( tl );
		}	
		return tl;
	}
	else {
		tl = commandAttrOrCall( tl );	
		return processTerminal( tl, NL );
	}
	return tl;
}

/*
varOrCall -> 'ID' '(' expressionList ')'
varOrCall -> 'ID' {[exp]}
*/
static TokenList varOrCall(TokenList tl) {
	tl = processTerminal(tl, IDENTIFIER);
	if( verifyCurrentToken( tl, OP_PARENTHESIS) ) {
		tl = call(tl);
		/*tl = processTerminal( tl, OP_PARENTHESIS);
		tl = expressionList(tl);
		tl = processTerminal( tl, CL_PARENTHESIS);*/
	}
	else {
		tl = arrayAccess(tl);
	}
	return tl;
}

/*
new -> 'new' '[' expression ']' type
*/
static TokenList new(TokenList tl) {
	tl = processTerminal(tl, NEW);
	tl = processTerminal(tl, OP_BRACKET);
	tl = expression(tl);
	tl = processTerminal(tl, CL_BRACKET);
	tl = type(tl);
	return tl;
}

/*
F -> '(' expression ')'
F -> 'BOOL_VAL'
F -> 'INT_VAL'
F -> 'STRING_VAL'
F -> varOrCall
F-> new
*/
static  TokenList F(TokenList tl) {
	Token t;
	int exp_ret=1;

	while( tl && exp_ret ) {
		t = tokenListGetCurrentToken(tl);
		switch ( tokenGetKind(t) ) {
		case MINUS: case NOT:
			tl = processTerminal( tl, tokenGetKind( t ) );
			break;
		case OP_PARENTHESIS:
			tl = processTerminal( tl, OP_PARENTHESIS );
			count_parenthesis ++;
			break;
		default:
			exp_ret = 0;
			break;
		}
	}

	t = tokenListGetCurrentToken(tl);

	switch( tokenGetKind(t) ) {
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
		
	while( tl!=NULL && count_parenthesis > 0 && verifyCurrentToken(tl, CL_PARENTHESIS) ) {
		tl = processTerminal(tl, CL_PARENTHESIS);
		count_parenthesis--;
	}

	return tl;
}

static TokenList T(TokenList tl) {
	Token t;
	tl = F(tl);
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
	if( count_parenthesis != 0 )
	{	
		error_flag++;
		tl = NULL;
		printf("Error: unbalanced parenthesis;");
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
	return tl;
}

/*
attr -> 	arrayAccess '=' expression
*/
static TokenList attr( TokenList tl ) {
	tl = arrayAccess(tl);
	tl = processTerminal( tl, EQUAL );
	tl = expression( tl );
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
		printf("Expected a function call or attribution, but got %s\n", tokenToString(tokenListGetCurrentToken(tl)));
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
commandIf->'IF' expression 'NL'
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
	if(verifyCurrentToken(tl, IDENTIFIER)) {
		tl = params( tl );
	}
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
	if(tl == NULL) {
		printf("Error Empty Program.");
		return tl;
	}

	// "{NL}"
	while( verifyCurrentToken(tl, NL) ) {
			tl = processTerminal( tl, NL );
	}

	do{
		
		t = tokenListGetCurrentToken( tl );
		switch ( tokenGetKind( t ) ) {
			case IDENTIFIER: case FUN:
				tl = decl( tl );
				break;
			default:
				error_flag++;
				printf("Error at line %d. Expected 'fun' or identifier but got %s\n", tokenGetLine(t), tokenToString(t));
				tl=NULL; // loop protection
				break;
			}
	} while(tl);
	
	return tl;
}



int parser( TokenList tl )
{
	program(tl);
	return error_flag;
}