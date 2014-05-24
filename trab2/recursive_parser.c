#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "../trab1/token.h"
#include "../trab1/token_list.h"
//#include "../trab3/ast.h"
#include "recursive_parser.h"

//global_variable is more intuitive;

// Functions that are associated to a grammar non-terminal:
static TokenList program( TokenList tl );
static TokenList declFunction( TokenList tl );
static TokenList declVar(TokenList tl);
static TokenList type( TokenList tl );
static TokenList block( TokenList tl );
static TokenList declOrCommand( TokenList tl );
static TokenList command( TokenList tl );
static TokenList commandAttrOrCall( TokenList tl );
static TokenList commandWhile( TokenList tl );
static TokenList commandReturn( TokenList tl );
static TokenList commandIf( TokenList tl );
static TokenList decl( TokenList tl );
static TokenList attr( TokenList tl );
static TokenList arrayAccess( TokenList tl );
static TokenList new(TokenList tl);
static TokenList call( TokenList tl );
static TokenList params(TokenList tl);
static TokenList param(TokenList tl);
static TokenList expression( TokenList tl );
static TokenList expressionList( TokenList tl );
static TokenList val(TokenList tl);
static TokenList unaryOp( TokenList tl );
static TokenList mulDivOp(TokenList tl);
static TokenList minAddOp( TokenList tl);
static TokenList comparisonOp( TokenList tl);
static TokenList and( TokenList tl );

static unsigned long line = 0;





static bool hasErrors = false;

static callbackOnDerivation actinOnRules = NULL;

static int callOnConsume( NonTerminal rule, Token t, int line ) {
	if( actinOnRules != NULL && !hasErrors ) {
		return actinOnRules( rule, t, line );
	} else
		return -1;
}

// -- Auxiliar Functions:
static int verifyCurrentToken(TokenList tl, TokenKind tk) {
	if (tl == NULL)
		return 0;
	Token t = TokenList_GetCurrentToken( tl );
	return Token_GetKind( t ) == tk;
}

static void printError(int line, char *expected, char *got) {
	hasErrors = true;
	printf("Error at Line %d.\nThe expected was '%s' but got '%s'", line, expected, got);	

}

static TokenList processTerminal( NonTerminal rule, TokenList tl, TokenKind tk ) {
	Token t = TokenList_GetCurrentToken( tl );
	if ( tl == NULL ) {
		if ( !hasErrors ) {
		
			printf("Error: Unexpected End of File. The expected was '%s>'\n", TokenKind_ToString(tk) );
		}
		hasErrors = true;
		return NULL;
	}
	else if ( t!= NULL && Token_GetKind( t ) == tk ) {
		line = Token_GetLine( t );
		callOnConsume( rule, t, line );
		return TokenList_Next( tl );
	}
	else {
		hasErrors = true;
		printError( Token_GetLine(t), TokenKind_ToString(tk), Token_GetStringForKind(t));
		return NULL;
	}
}

static void processNonTerminal( NonTerminal rule ) {
	callOnConsume( rule, NULL, line );
}

/*
type -> 'CHAR'
type -> 'STRING'
type -> 'INT'
type -> 'BOOL'
type -> '[' ']' type
*/
static TokenList type( TokenList tl ) {
	Token t;
	processNonTerminal( NT_TYPE );
	t = TokenList_GetCurrentToken( tl );
	
	switch( Token_GetKind( t ) ) {
		case TK_INT:
		case TK_BOOL:
		case TK_CHAR:
		case TK_STRING:
			tl = processTerminal(NT_TYPE, tl, Token_GetKind( t ) );
		break;

		case TK_OP_BRACKET:
			while( verifyCurrentToken( tl, TK_OP_BRACKET ) ) {
				tl = processTerminal(NT_TYPE, tl, TK_OP_BRACKET );
				tl = processTerminal(NT_TYPE, tl, TK_CL_BRACKET );	
			}
			tl = type( tl );
		break;

		default:
			if(tl != NULL) {
				printError( Token_GetLine( t ) , "a type", Token_GetStringForKind( t ) );
				tl = NULL;
			}
		break;
	}
	return tl;
}

/*
declVar -> 'ID' ':' type 'NL'
*/
static TokenList declVar( TokenList tl ) {	
	processNonTerminal( NT_DECL_VAR );
	//tl = processTerminal(NT_DECL_VAR, tl, TK_IDENTIFIER );
	tl = processTerminal(NT_DECL_VAR, tl, TK_COLON );
	tl = type( tl );
	return processTerminal(NT_DECL_VAR, tl, TK_NL );
}
/*
param -> 'ID' ':' type
*/
static TokenList param( TokenList tl ) {
	processNonTerminal( NT_PARAM );
	tl = processTerminal(NT_PARAM, tl, TK_IDENTIFIER );
	tl = processTerminal(NT_PARAM, tl, TK_COLON );
	return type( tl );
}
/*
params -> param { ','' param }
*/
static TokenList params( TokenList tl ) {
	processNonTerminal( NT_PARAMS );
	tl = param( tl );
	//pay attention, loop
	while( verifyCurrentToken( tl, TK_COMMA ) ) {
		tl = processTerminal(NT_PARAMS, tl, TK_COMMA );
		tl = param( tl );
	}
	return tl;
}

/*
declOrCommand -> ':' type 'NL'
declOrCommand -> ':' type 'NL' 'ID' declOrCommand
declOrCommand -> commandAttrOrCall 'NL'
*/
static TokenList declOrCommand( TokenList tl ) {
	processNonTerminal( NT_DECL_OR_COMMAND );
	if( verifyCurrentToken( tl, TK_COLON ) ) {
		tl = declVar( tl );
		if( verifyCurrentToken( tl, TK_IDENTIFIER ) ) {		
			tl = processTerminal( NT_DECL_OR_COMMAND, tl, TK_IDENTIFIER );
			tl = declOrCommand( tl );
		}
		return tl;
	}
	else {
		tl = commandAttrOrCall( tl );	
		return processTerminal( NT_DECL_OR_COMMAND, tl, TK_NL );
	}
	return tl;
}

/*
new -> 'new' '[' expression ']' type
*/
static TokenList new(TokenList tl) {	
	processNonTerminal( NT_NEW );
	tl = processTerminal( NT_NEW, tl, TK_NEW );
	tl = processTerminal( NT_NEW, tl, TK_OP_BRACKET );
	tl = expression( tl );
	tl = processTerminal( NT_NEW, tl, TK_CL_BRACKET );
	tl = type( tl );
	return tl;
}

/*
F -> '(' expression ')' -- it isn't very clear at code, because I didn't use recursion.
F -> - F
F -> NOT F
F -> 'BOOL_VAL'
F -> 'INT_VAL'
F -> 'STRING_VAL'
F -> varOrCall
F-> new
*/
static TokenList val(TokenList tl) {
	Token t;
	processNonTerminal( NT_VAL );
	t = TokenList_GetCurrentToken( tl );	
	switch( Token_GetKind( t ) ) {

		case TK_OP_PARENTHESIS:
			tl = processTerminal( NT_VAL, tl, TK_OP_PARENTHESIS );
			tl = expression( tl );
			tl = processTerminal( NT_VAL, tl, TK_CL_PARENTHESIS );
		break;

		case TK_BOOL_VAL:
			tl = processTerminal( NT_VAL, tl, TK_BOOL_VAL );
		break;

		case TK_INT_VAL:
			tl = processTerminal( NT_VAL, tl, TK_INT_VAL );
		break;
		
		case TK_STRING_VAL:
			tl = processTerminal( NT_VAL, tl, TK_STRING_VAL );
		break;

		case TK_IDENTIFIER:
			tl = processTerminal( NT_VAL, tl, TK_IDENTIFIER );
			if ( verifyCurrentToken( tl, TK_OP_PARENTHESIS) ) {
				tl = call( tl );
			}
			else {
				tl = arrayAccess( tl );
			}
		break;

		case TK_NEW:
			tl = new( tl );
		break;

		default:
			if ( tl != NULL ) {
				hasErrors = true;
				printf( "Error at Line %d.\nAn expression was expected but got '%s'.\n", Token_GetLine( t ), Token_GetStringForKind( t ) );
				tl = NULL;		
			}
		break;
	}
		
	return tl;
}


/*
U-> '-' F
U-> 'not' F
U-> F 
*/
static TokenList unaryOp( TokenList tl ) {
	Token t;
	processNonTerminal( NT_UNARY_OP );
	t = TokenList_GetCurrentToken( tl );
	switch ( Token_GetKind( t ) ) {
		case TK_MINUS: case TK_NOT:
			tl = processTerminal( NT_UNARY_OP, tl, Token_GetKind( t ) );
			tl = unaryOp( tl );
		break;

		default:
			tl = val( tl );
		break;
	}
	return tl;
}


/*
T -> U
T -> U '*' E
T -> U '/' E
First(U) u First(F)
*/
static TokenList mulDivOp(TokenList tl) {
	Token t;
	processNonTerminal( NT_MUL_DIV_OP );
	tl = unaryOp( tl );
	t = TokenList_GetCurrentToken( tl );
	switch( Token_GetKind( t ) ) {

		case TK_MUL:case TK_DIV:
			tl = processTerminal( NT_MUL_DIV_OP, tl, Token_GetKind( t ) );
			tl = mulDivOp( tl );	
		break;

		default:	break;
 	}
	return tl;
}


/*
E -> T
E -> T '+' E
E -> T '-' E
-- First(E) == First(U) u First(F)
*/
static TokenList minAddOp( TokenList tl) {
	Token t;
	processNonTerminal( NT_MIN_ADD_OP );
	tl = mulDivOp( tl );
	t = TokenList_GetCurrentToken( tl );	
	switch( Token_GetKind( t ) ) {

		case TK_PLUS: case TK_MINUS:
			tl = processTerminal( NT_MIN_ADD_OP, tl, Token_GetKind( t ) );
			tl = minAddOp( tl );
		break;

		default:	break;
	}
	return tl;
}

/*
C -> E 
C -> E '>' C
C -> E '>=' C
C -> E '<' C
C -> E '<=' C
C -> E '=' C
C -> E '<>' C
*/

static TokenList comparisonOp( TokenList tl) {
	Token t;
	processNonTerminal( NT_COMPARISON_OP );
	tl = minAddOp( tl );
	t = TokenList_GetCurrentToken( tl );
	switch( Token_GetKind( t ) ) {

		case TK_GREATER:
		case TK_GREATER_EQUAL:
		case TK_LESS:
		case TK_LESS_EQUAL:
		case TK_EQUAL:
		case TK_DIFFERENT:
			tl = processTerminal( NT_COMPARISON_OP, tl, Token_GetKind( t ) );
			tl = comparisonOp( tl );
		break;

		default:	break;
	}
	return tl;
}

static TokenList and( TokenList tl ) {
	Token t;
	processNonTerminal( NT_AND );
	tl = comparisonOp( tl );
	t = TokenList_GetCurrentToken( tl );
	switch( Token_GetKind( t ) ) {
		case TK_AND:
			tl = processTerminal(NT_AND, tl, TK_AND );
			tl = and( tl );
		break;

		default:	break;
	}
	return tl;
}
	
/*
expression -> And 'AND' expression
expression -> C*/

static TokenList expression( TokenList tl ) {
	Token t;
	processNonTerminal( NT_EXPRESSION );
	tl = and( tl );
	t = TokenList_GetCurrentToken( tl );
	switch( Token_GetKind( t ) ) {
		case TK_OR:
			tl = processTerminal( NT_EXPRESSION, tl, TK_OR );
			tl = expression( tl );
		break;

		default: break;
	}
	return tl;
}

/*
expressionList -> expression { ',' expression }
*/
static TokenList expressionList( TokenList tl ) {
	processNonTerminal( NT_EXPRESSION_LIST );
	tl = expression( tl );
	while(verifyCurrentToken(tl, TK_COMMA)) {
		tl = processTerminal( NT_EXPRESSION_LIST, tl, TK_COMMA );
		tl = expression( tl );
	}
	return tl;
}

/*
call -> 	'(' [expressionList] ')'
*/
static TokenList call( TokenList tl ) {
	Token t;	
	processNonTerminal( NT_CALL );
	tl = processTerminal( NT_CALL, tl, TK_OP_PARENTHESIS );
	t = TokenList_GetCurrentToken( tl );
	switch ( Token_GetKind( t )) {
		case TK_IDENTIFIER:
		case TK_INT_VAL:
		case TK_STRING_VAL:
		case TK_BOOL_VAL:
		case TK_NEW:
		case TK_OP_PARENTHESIS:
		case TK_NOT:
		case TK_MINUS:
			tl = expressionList( tl );
		break;

		default:	break;

	}
	
	tl = processTerminal( NT_CALL, tl, TK_CL_PARENTHESIS );
	return tl;
}

/*
attr -> 	arrayAccess '=' expression
*/
static TokenList attr( TokenList tl ) {
	processNonTerminal( NT_ATTR );
	tl = arrayAccess( tl );
	tl = processTerminal( NT_ATTR, tl, TK_EQUAL );
	tl = expression( tl );
	return tl;
}

/*
arrayAccess -> 	{ '[' expression']' }
*/
static TokenList arrayAccess( TokenList tl) {
	processNonTerminal( NT_ARRAY_ACCESS );
	while( verifyCurrentToken( tl, TK_OP_BRACKET ) ) {
		tl = processTerminal( NT_ARRAY_ACCESS, tl, TK_OP_BRACKET );
		tl = expression( tl );
		tl = processTerminal( NT_ARRAY_ACCESS, tl, TK_CL_BRACKET );
	}
	return tl;
}

/*
commandAttrOrCall -> attr | call
*/
static TokenList commandAttrOrCall( TokenList tl ) {
	Token t;
	processNonTerminal( NT_COMMAND_ATTR_OR_CALL );
	if ( verifyCurrentToken(tl, TK_EQUAL) || verifyCurrentToken(tl, TK_OP_BRACKET) ) {
		tl = attr( tl );	
	}
	else if ( verifyCurrentToken( tl, TK_OP_PARENTHESIS ) ) {
		tl = call( tl );
	}
	else {
		hasErrors = true;
		t = TokenList_GetCurrentToken( tl );
		printf("Error at Line %d.\nExpected a function call or attribution, but got %s\n", Token_GetLine( t ), Token_GetStringForKind( t ));
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
	processNonTerminal( NT_COMMAND_WHILE );
	tl = processTerminal( NT_COMMAND_WHILE, tl, TK_WHILE );
	tl = expression( tl );
	tl = processTerminal( NT_COMMAND_WHILE, tl, TK_NL );
	tl = block( tl );
	tl = processTerminal( NT_COMMAND_WHILE, tl, TK_LOOP );
	return tl;
}

/*
commandReturn -> 'RETURN' [ 'expression' ]
*/
static TokenList commandReturn( TokenList tl ) {
	Token t;
	processNonTerminal( NT_COMMAND_RETURN );
	tl = processTerminal( NT_COMMAND_RETURN, tl, TK_RETURN );
	t = TokenList_GetCurrentToken( tl );

	switch( Token_GetKind( t ) ) {

		case TK_IDENTIFIER:
		case TK_INT_VAL:
		case TK_STRING_VAL:
		case TK_BOOL_VAL:
		case TK_NEW:
		case TK_OP_PARENTHESIS:
		case TK_NOT:
		case TK_MINUS:
			tl = expression( tl );
		break;

		default:	break;
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

static TokenList commandElseIf( TokenList tl ) {
	processNonTerminal( NT_COMMAND_ELSE_IF );	
	tl = processTerminal( NT_COMMAND_ELSE_IF, tl, TK_IF );
	tl = expression( tl );
	tl = processTerminal( NT_COMMAND_ELSE_IF, tl, TK_NL );
	tl = block( tl );
	return tl;
	
}
static TokenList commandElse( TokenList tl ) {
	processNonTerminal( NT_COMMAND_ELSE );
	tl = processTerminal( NT_COMMAND_ELSE, tl, TK_NL );
	tl = block( tl );
	return tl;
}
static TokenList commandIf( TokenList tl ) {
	processNonTerminal( NT_COMMAND_IF );
	tl = processTerminal( NT_COMMAND_IF, tl, TK_IF );
	tl = expression( tl );
	tl = processTerminal( NT_COMMAND_IF, tl, TK_NL );
	tl = block( tl );

	while( verifyCurrentToken( tl, TK_ELSE ) ) {
		tl = processTerminal( NT_COMMAND_IF, tl, TK_ELSE );
		if( verifyCurrentToken (tl, TK_IF) ) {
			tl = commandElseIf( tl );
		}
		else {
			tl = commandElse( tl );	
			break;
		}
	}
	
	tl = processTerminal( NT_COMMAND_IF, tl, TK_END);
	return tl;

}

/*

command -> 'ID' commandAttrOrCall 'NL'
command -> commandWhile 'NL'
command -> commandIf 'NL'
command -> commandReturn 'NL'

*/
static TokenList command( TokenList tl ) {
	Token t;
	processNonTerminal( NT_COMMAND );
	t = TokenList_GetCurrentToken( tl );
	switch( Token_GetKind( t ) ) {
		case TK_WHILE:
			tl = commandWhile( tl );
		break;

		case TK_RETURN:
			tl = commandReturn( tl );
		break;

		case TK_IF:
			tl = commandIf( tl );
		break;

		case TK_IDENTIFIER:
			tl = processTerminal( NT_COMMAND, tl, TK_IDENTIFIER );
			tl = commandAttrOrCall( tl );
		break;

		default:
			if(tl != NULL) {
				hasErrors = true;
				Token t = TokenList_GetCurrentToken( tl );
				//printf( "\nToken: %s ", Token_GetStringForKind(t));
				printf( "Error at Line %d.\nExpected identifier, if, return or while but got %s.\n", Token_GetLine(t), Token_GetStringForKind(t) );
				return NULL;
			}
		break;
	}
	return processTerminal( NT_COMMAND, tl, TK_NL );
};


/*block -> [ 'ID' declOrCommand ]
		 { command }*/
static TokenList block( TokenList tl ) {
	TokenList tlf;
	Token t;

	processNonTerminal( NT_BLOCK );

	t = TokenList_GetCurrentToken( tl );
	if ( verifyCurrentToken( tl, TK_IDENTIFIER ) ) {
		tl = processTerminal( NT_BLOCK, tl, TK_IDENTIFIER );
		tl = declOrCommand( tl );//Recursividade
	}
	tlf = NULL;
	while( tlf!=tl && tl ) {
		t = TokenList_GetCurrentToken( tl );	
		tlf = tl;
		switch( Token_GetKind( t ) ) {
			case TK_ELSE: case TK_END: case TK_LOOP:
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
	processNonTerminal( NT_DECL_FUNCTION );
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_FUN );
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_IDENTIFIER );
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_OP_PARENTHESIS );
	if (verifyCurrentToken( tl, TK_IDENTIFIER ) ) {
		tl = params( tl );
	}
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_CL_PARENTHESIS );
	if ( verifyCurrentToken( tl, TK_COLON ) ) {
		tl = processTerminal( NT_DECL_FUNCTION, tl, TK_COLON );
		tl = type( tl );
	}
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_NL );
	tl = block( tl );
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_END );
	tl = processTerminal( NT_DECL_FUNCTION, tl, TK_NL );
	return tl;
}



/*
decl -> declVar start 
decl -> declFunction start
*/
static TokenList decl( TokenList tl ) {
	Token t;
	processNonTerminal( NT_DECL );
	t = TokenList_GetCurrentToken( tl );
	switch ( Token_GetKind( t ) ) {
		case TK_IDENTIFIER:
			tl = processTerminal( NT_DECL, tl, TK_IDENTIFIER );
			tl = declVar( tl );
		break;

		case TK_FUN: 
			tl = declFunction( tl );
		break;

		default:	break;
			
	}
	return tl;
}


/*
program -> {NL} decl {decl};

*/
static TokenList program( TokenList tl ) {
	Token t;	
	processNonTerminal( NT_PROGRAM );
	if(tl == NULL) {
		printf("Error. The file is Empty.\n");
		hasErrors = true;
		return tl;
	}

	// "{NL}"
	while( verifyCurrentToken ( tl, TK_NL ) ) {
		tl = processTerminal( NT_PROGRAM, tl, TK_NL );
	}

	do {
		t = TokenList_GetCurrentToken( tl );
		switch ( Token_GetKind( t ) ) {
			case TK_IDENTIFIER: case TK_FUN:
				tl = decl( tl );
				break;
			default:
				if( tl != NULL ) {
					hasErrors = true;
					printf("Error at line %d.\nExpected 'fun' or an 'identifier' but got '%s'\n", Token_GetLine(t), Token_GetStringForKind(t));
					tl=NULL; // loop protection
				}
				break;
			}
	} while( tl && !hasErrors );
	
	return tl;
}

bool parser( TokenList tl, callbackOnDerivation f )
{
	actinOnRules = f;
	program( tl );
	return hasErrors;
}
