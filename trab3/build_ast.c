#ifndef  BUILD_AST_C
#define  BUILD_AST_C
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
#include "../trab1/mini0-lex.h"
#include "../trab1/token_list.h"
#include "../trab2/recursive_parser.h"
#include "build_ast.h"
#include "assert.h"


static AST _root = NULL;
static AST _currParent = NULL;
static char* str = NULL;
static unsigned long dimension = 0;

static ASTNodeType TkToASTType( TokenKind tk ) {
	switch ( tk ) {
		case TK_INT:
			return AST_INT;
		break;

		case TK_BOOL:
			return AST_BOOL;
		break;

		case TK_CHAR:
			return AST_CHAR;
		break;

		case TK_STRING:
			return AST_CHAR; //unsafe, someone must tell that string is []char
		break;

		case TK_INT_VAL:
			return AST_INT_VAL;
		break;

		case TK_BOOL_VAL:
			return AST_BOOL_VAL;
		break;

		case TK_STRING_VAL:
			return AST_STRING_VAL; //unsafe, someone must tell that string is []char
		break;

		default: 
			printf( "Ooops... Isto não Deveria acontecer" );
			return (ASTNodeType)0;
		break;

	}
}


//this function assumes that the parser are ok;
int buildAst( NonTerminal rule, Token t, int line ) {
	AST node = NULL;
	TokenKind tk = Token_GetKind( t );

	if ( _root == NULL ) {
		_root = AST_New( AST_PROGRAM, line );
	 	_currParent = _root;
	}

	switch ( rule ) {
		case NT_DECL_FUNCTION:
			switch( tk ) {
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_DECL_FUNCTION, line, Token_GetStringValue( t ) );
					_currParent = AST_InsertChild( _currParent, node);
				 	
				break;
				case TK_CL_PARENTHESIS:
				 	_currParent = AST_GetParent( _currParent );
					if( AST_GetType( _currParent ) == AST_DECL_FUNCTION ) {
					 	_currParent = AST_GetParent( _currParent );
					}
				break;
				case TK_END:
				 	_currParent = AST_GetParent( _currParent );
				break;
				default: break;
			}
		break;
		case NT_PARAM:
			if( AST_GetType( _currParent ) != AST_PARAM_LIST ) {
				node = AST_New( AST_PARAM_LIST, line );
			 	_currParent = AST_InsertChild( _currParent, node);
			}
			switch( tk ) {
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_PARAM, line, Token_GetStringValue( t ) );
					AST_InsertChild( _currParent, node );
				break;

				default: break;
			}
		break;
		case NT_TYPE:
			switch ( tk ) {
				case TK_CL_BRACKET:
					dimension++;
				break;
				case TK_INT: case TK_STRING: case TK_CHAR: case TK_BOOL:
					node = AST_NewAsInt( TkToASTType( tk ) , line, dimension );
					AST_InsertChild( _currParent, node );
				break;
			}
		break;

		case NT_BLOCK:
			if( tk == TK_IDENTIFIER ) {
				str = Token_GetStringValue( t );
			}
		break;

		case NT_DECL_OR_COMMAND:
			switch ( tk ) {
				case TK_NL:
				 	_currParent = AST_GetParent( _currParent );
				break;

				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;
				default: break;
			}
		break;

		case NT_COMMAND:
			if( tk == TK_IDENTIFIER) {
				str = Token_GetStringValue( t ); //Just store it for future usage.
			}
			else if( tk == TK_NL) {
			 	_currParent = AST_GetParent( _currParent );
			}
		break;

		case NT_COMMAND_IF:
			switch ( tk ) {

				case TK_IF:
					node = AST_New( AST_IF , line );
				 	_currParent = AST_InsertChild( _currParent, node );
				break;

				case TK_END:
				 	_currParent = AST_GetParent( _currParent );
					if( AST_GetType( _currParent ) == AST_ELSE || AST_GetType( _currParent ) == AST_ELSE_IF ) {
					 	_currParent = AST_GetParent( _currParent );
					}
				 	_currParent = AST_GetParent( _currParent );
				break;

				case TK_NL: 
				 	_currParent = AST_GetParent( _currParent );
				break;

				default: break;
			}
			//IF QUESTIONS
		break;

		case NT_COMMAND_RETURN:
			node = AST_New( AST_RETURN , line );
		 	_currParent = AST_InsertChild( _currParent, node );
		break;

		case NT_COMMAND_WHILE:
			switch ( tk ) {
				case TK_WHILE:
					node = AST_New( AST_WHILE , line );
				 	_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_LOOP:
				 	_currParent = AST_GetParent( _currParent );
				 	_currParent = AST_GetParent( _currParent );
				break;

				case TK_NL:
				 	_currParent = AST_GetParent( _currParent );
				break;

				default: break;
			}
		break;

		case NT_ATTR:
			if( t == NULL ) { //Começo da Regra
				node = AST_New( AST_ATTR , line );
			 	_currParent = AST_InsertChild( _currParent, node );
				node = AST_NewAsString( AST_VAR , line, str ); 
			 	_currParent = AST_InsertChild( _currParent, node );
			}
		break;

		case NT_EXPRESSION:
			node = AST_New( AST_EXPRESSION , line );
		 	/*_currParent = */AST_InsertChild( _currParent, node );
		break;


		case NT_ARRAY_ACCESS:
			if( tk == TK_CL_BRACKET ) {
				if( AST_GetType( _currParent ) == AST_EXPRESSION ) {
				 	_currParent = AST_GetParent( _currParent );	
				}
			 	/*_currParent = */AST_GetParent( _currParent );
			}
		break;

		case NT_EXPRESSION_LIST:
			switch ( tk ) {
				case TK_COMMA:
				 	_currParent = AST_GetParent( _currParent );
				break;

				default: break;
			}
		break;

		case NT_NEW:
			switch ( tk ) {
				case TK_CL_BRACKET:
				 	_currParent = AST_GetParent( _currParent );
				break;

				case TK_NEW:
					node = AST_New( AST_NEW , line );
				 	_currParent = AST_InsertChild( _currParent, node );
				break;

				default: break;
			}
			
		break;

		case NT_CALL:
			switch ( tk ) {
				case TK_CL_PARENTHESIS:
					if ( AST_GetType( _currParent ) == AST_EXPRESSION ) {
					 	/*_currParent = */AST_GetParent( _currParent );	
					}
				 	_currParent = AST_GetParent( _currParent );
				break;

				default: break;
			}
		break;

		/*case NT_VAL:
			switch ( tk ) {
				case TK_BOOL_VAL:
					node = AST_NewAsBool( AST_BOOL_VAL, line, Token_GetBoolValue( t ) );
					AST_InsertChild( _currParent, node );
				break;
				case TK_INT_VAL:
					node = AST_NewAsInt( AST_INT_VAL, line, Token_GetIntValue( t ) );
					AST_InsertChild( _currParent, node );

				break;
				case TK_STRING_VAL:
					node = AST_NewAsString( AST_STRING_VAL, line, Token_GetStringValue( t ) );
					AST_InsertChild( _currParent, node );
				break;

				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;

				default: break;
			}
		break;

		case NT_UNARY_OP:
			if ( t != NULL ) {
				if( tk == TK_MINUS ) {
					node  = AST_New( AST_UNARYMINUS, line );
				} else {
					node  = AST_New( AST_NOT, line );
				}
			 	_currParent = AST_InsertChild( _currParent, node );
			}
		break;

		case NT_MUL_DIV_OP:
		case NT_MIN_ADD_OP:
		case NT_COMPARISON_OP:
		case NT_AND:
			node = AST_New( TkToASTType( tk ), line );
		 	_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
		break;

	*/
		default: break;

	}

	return 1;
}

AST BuildAst( FILE* input, bool* hasErrors 
	) {
	_root = NULL; //Previous I'm be leek
	_currParent = NULL; //Previous I'm be leek
	TokenList tl;
	assert(input);
	tl = generateTokens( input, hasErrors );
	if ( ! *hasErrors ) {
		*hasErrors = parser( tl, buildAst );
	}
	TokenList_Free( tl );
	return _root;
}






#endif //AST_H