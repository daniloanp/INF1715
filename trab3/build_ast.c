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
static AST _lastOp = NULL;
static AST _parentExp = NULL;


static char* str = NULL;
static unsigned long dimension = 0;

int priorityLevel( AST node ) {
	ASTNodeType tp = AST_GetType( node );
	switch ( tp ) {
		case AST_EXPRESSION:
			return -1;
		break;

		case AST_OR: 
			return 0; 
		break;
		case AST_AND:
			return 1;
		break;
		case AST_DIFFERENT:
		case AST_EQUAL:
		case AST_LESS:
		case AST_LESS_EQUAL:
		case AST_GREATER_EQUAL:
		case AST_GREATER:
			return 2;
		break;

		case AST_PLUS:
		case AST_MINUS:
			return 3;
		break;

		case AST_MUL:
		case AST_DIV:
			return 4;
		break;

		case AST_NOT:
		case AST_UNARYMINUS:
			return 5;
		break;

		case AST_INT_VAL:
		case AST_STRING_VAL:
		case AST_BOOL_VAL:
		case AST_CALL:
		case AST_NEW:
		case AST_VAR:
			return 6;
		break;

		default:
			printf("\n\nError: Invalid Node Type(%d) at line %d !!!\n\n", tp, AST_GetLine( node ));
			return 0;
		break;
	}
}



bool cmpBinOp( AST op, AST newOp ) {
	return ( priorityLevel( newOp ) > priorityLevel( op ) );
}

void insertChild( AST node ) {
	_currParent = AST_InsertChild( _currParent, node);
}

int handleGetIntoRule( NonTerminal rule,  int line ) {
	AST node;
	switch ( rule ) {
			case NT_PROGRAM:
				_root = AST_New( AST_PROGRAM, line );
				_currParent = _root;
			break;
		
			case NT_PARAMS:
				node = AST_New( AST_PARAM_LIST, line );
				insertChild( node );
			break;
			
			case NT_ATTR:
				node = AST_New( AST_ATTR , line );
				insertChild( node );
			break;
			
			case NT_BLOCK:
				node = AST_New( AST_BLOCK, line );
				insertChild( node );
			break;
			case NT_COMMAND_IF:
				node = AST_New( AST_IF , line );
				insertChild( node );
			break;
			case NT_COMMAND_ELSE:
				node = AST_New( AST_ELSE, line );
				insertChild( node );
			break;
			
			case NT_COMMAND_ELSE_IF:
				node = AST_New( AST_ELSE_IF, line );
				insertChild( node );
				
			break;
			
			case NT_EXPRESSION: 
				switch( AST_GetType( _currParent ) ) {
					case AST_IF:
					case AST_ELSE_IF:
					case AST_CALL:
					case AST_VAR:
					case AST_NEW:
					case AST_WHILE:
					case AST_RETURN:
					case AST_ATTR:
					//case AST_OR:
					node = AST_New( AST_EXPRESSION, line );
					insertChild( node );
					default: 
					break;
				}
			break;

			case NT_DECL_VAR:
				node = AST_NewAsString( AST_DECL_VAR, line, str );
				insertChild( node );
				str = NULL;
			break;

			case NT_VAR_ACCESS:
				node = AST_NewAsString( AST_VAR , line, str ); 
				insertChild( node );
				str = NULL;
			break;

			case NT_ARRAY_ACCESS:
			break;

			case NT_COMMAND_WHILE: 
				node = AST_New( AST_WHILE , line );
				insertChild( node );
			break;

			case NT_COMMAND_RETURN:
				node = AST_New( AST_RETURN , line );
				insertChild( node );
			break;

			case NT_CALL:
				node = AST_NewAsString( AST_CALL, line, str );
				str = NULL;
				insertChild( node );
			break;

			case NT_NEW:
				node = AST_New(AST_NEW, line);
				insertChild( node );
			break;

			default: break;
		}
}

int handleGetOutRule( NonTerminal rule, int line ) {
	switch( rule ) {
			case NT_COMMAND_ELSE:
			case NT_COMMAND_ELSE_IF:
			case NT_DECL_VAR:
			case NT_VAR_ACCESS:
			case NT_COMMAND_WHILE:
			case NT_COMMAND_RETURN:
			case NT_CALL:
			case NT_NEW:
			case NT_PROGRAM:
			case NT_PARAMS:
			case NT_ATTR:
			case NT_BLOCK:
			case NT_DECL_FUNCTION:
			case NT_TYPE:
			case NT_PARAM:
				_currParent = AST_GetParent( _currParent );	
			break;

			case NT_EXPRESSION:
				if( AST_GetType( _currParent ) == AST_EXPRESSION || AST_GetType( _currParent ) == AST_OR ) {
					_currParent = AST_GetParent( _currParent );	
				}
			break;

			case NT_VAL:
				switch( AST_GetType( _currParent ) ) {
					case TK_BOOL_VAL:
					case TK_INT_VAL:
					case TK_STRING_VAL:
						_currParent = AST_GetParent( _currParent );	
					break;

					default:
					break;
				}
			break;

			case NT_UNARY_OP:
				switch( AST_GetType( _currParent ) ) {
					case AST_UNARYMINUS:
					case AST_NOT:
						_currParent = AST_GetParent( _currParent );	
					break;
					default: break;
				}
			break;
			case NT_MUL_DIV_OP:
				switch( AST_GetType( _currParent ) ) {
					case AST_MUL:
					case AST_DIV:
						_currParent = AST_GetParent( _currParent );	
					break;
					default: break;
				}
			break;
			case NT_MIN_ADD_OP:
				switch( AST_GetType( _currParent ) ) {
					case AST_PLUS:
					case AST_MINUS:
						_currParent = AST_GetParent( _currParent );	
					default: break;
				}
			break;
			case NT_COMPARISON_OP:
				switch( AST_GetType( _currParent ) ) {
					case AST_LESS_EQUAL:
					case AST_LESS:
					case AST_EQUAL:
					case AST_DIFFERENT:
					case AST_GREATER:
					case AST_GREATER_EQUAL:
						_currParent = AST_GetParent( _currParent );	
					default: break;		
				}
			break;
			case NT_AND:
				switch( AST_GetType( _currParent ) ) {
					case AST_AND:
						_currParent = AST_GetParent( _currParent );	
					break;
					default: break;
				}
			break;

			case NT_COMMAND_IF: 
				_currParent = AST_GetParent( _currParent );
			break;

			default: 
			break;
	}
}

void insertBinOp( AST node ) {
	AST parent;
	AST child;

	if( AST_GetType( _currParent ) == AST_EXPRESSION ) {
		parent = _currParent;
		child = AST_GetFirstChild( parent );
		AST_RemoveChild( _currParent, child );
		_currParent = AST_InsertChild( _currParent, node );
		AST_InsertChild( _currParent, child );	
	}
	else if( !cmpBinOp( _currParent, node ) ) {
		parent = AST_GetParent( _currParent ); //vovo
		_currParent = parent;//now vovo is parent
		child  = AST_GetFirstChild( parent );			
		AST_RemoveChild( parent, child );
		_currParent = AST_InsertChild( _currParent, node );
		AST_InsertChild( _currParent, child );		
	} else {
		parent =  _currParent; //vovo
		child  = AST_GetLastChild( parent );	
		AST_RemoveChild( parent, child );
		_currParent = AST_InsertChild( _currParent, node );
		AST_InsertChild( _currParent, child );		
	}
	
}

//this function assumes that the parser are ok;
int handleTerminal( NonTerminal rule, Token t, int line ) {
	AST node = NULL;
	TokenKind tk;
	
	tk = Token_GetKind( t );
	switch ( rule ) {
		case NT_TYPE:
			switch ( tk ) {
				case TK_INT:
					node = AST_NewAsInt( AST_INT, line, dimension );
					insertChild( node );
					dimension = 0;
				break;
				
				case TK_BOOL:
					node = AST_NewAsInt( AST_BOOL, line, dimension );
					insertChild( node );
					dimension = 0;
				break;
				
				case TK_CHAR:
					node = AST_NewAsInt( AST_CHAR, line, dimension );
					insertChild( node );
					dimension = 0;
				break;
				
				case TK_STRING:
					node = AST_NewAsInt( AST_CHAR, line, dimension+1 );
					insertChild( node );
					dimension = 0;					
				break;

				case TK_CL_BRACKET:
					dimension = dimension + 1;
				break;
				
				default: break;
			}
		break;

		case NT_PARAM:
			switch ( tk ) {
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_PARAM, line, Token_GetStringValue( t ) );
					insertChild( node );
				break;
				default: break;
			}
		break;

		case NT_DECL_OR_COMMAND:
			switch ( tk ) {
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;
				
				default: break;
			}
		break;

		case NT_VAL:
			switch ( tk ) {
				case TK_OP_PARENTHESIS:
				break;
				case TK_CL_PARENTHESIS:	
				break;
				
				case TK_BOOL_VAL:
					node = AST_NewAsBool( AST_BOOL_VAL, line, Token_GetBoolValue( t ) );
					insertChild( node );
				break;
				
				case TK_INT_VAL:
					node = AST_NewAsInt( AST_INT_VAL, line, Token_GetIntValue( t ) );
					insertChild( node );
				break;
				
				case TK_STRING_VAL:
					node = AST_NewAsString( AST_STRING_VAL, line, Token_GetStringValue( t ) );
					insertChild( node );
				break;
				
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;
				
				default: break;
			}
		break;

		case NT_UNARY_OP:
			switch ( tk ) {
				case TK_MINUS:
					node  = AST_New( AST_UNARYMINUS, line );
					insertChild( node );
				break;
				
				case TK_NOT:
					node  = AST_New( AST_NOT, line );
					insertChild( node );
				break;
				
				default: break;
			}
		break;

		case NT_MUL_DIV_OP:
			switch ( tk ) {
				case TK_MUL:
					node = AST_New( AST_MUL , line );
					insertBinOp( node );
				break;
				
				case TK_DIV:
					node = AST_New( AST_DIV , line );
					insertBinOp( node );
				break;
				
				default: break;
			}
		break;

		case NT_MIN_ADD_OP:
			switch ( tk ) {
				case TK_PLUS:
					node = AST_New( AST_PLUS , line );
					insertBinOp( node );
				break;
				
				case TK_MINUS:
					node = AST_New( AST_MINUS , line );
					insertBinOp( node );
				break;
				
				default: break;
			}
		break;

		case NT_COMPARISON_OP:
			switch ( tk ) {
				case TK_GREATER:
					node = AST_New( AST_GREATER, line );
					insertBinOp( node );
				break;
				
				case TK_GREATER_EQUAL:
					node = AST_New( AST_GREATER_EQUAL, line );
					insertBinOp( node );
				break;
				
				case TK_LESS:
					node = AST_New( AST_LESS , line );
					insertBinOp( node );
				break;
				
				case TK_LESS_EQUAL:
					node = AST_New( AST_LESS_EQUAL , line );
					insertBinOp( node );
				break;
				
				case TK_EQUAL:
					node = AST_New( AST_EQUAL, line );
					insertBinOp( node );
				break;
				
				case TK_DIFFERENT:
					node = AST_New( AST_DIFFERENT, line );
					insertBinOp( node );
				break;
				
				default: break;
			}
		break;
		

		case NT_AND:
			switch ( tk ) {
				case TK_AND:
					node = AST_New( AST_AND , line );
					insertBinOp( node );
				break;
				
				default: break;
			}
		break;
		
		case NT_EXPRESSION:
			switch ( tk ) {
				case TK_OR:
					node = AST_New( AST_OR, line );
					insertBinOp( node );
				break;
				
				default: break;
			}
		break;

		
		

		case NT_COMMAND:
			switch ( tk ) {
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t ); //For attr or call
				break;
				
				default: break;
			}
		break;

		case NT_BLOCK:
			switch ( tk ) {
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t ); //For declaration, attr or call
				break;
				
				default: break;
			}
		break;

		case NT_DECL_FUNCTION:
			switch ( tk ) {
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_DECL_FUNCTION, line, Token_GetStringValue( t ) );
					insertChild( node );
				break;
								
				default: break;
			}
		break;

		case NT_DECL:
			switch( tk ) {
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;
				default: break;
			}
		break;

		default: break;

	}	
	return 1;
}

AST BuildAst( FILE* input, bool* hasErrors ) {
	_root = NULL;
	_currParent = NULL;
	TokenList tl;
	assert(input);
	tl = generateTokens( input, hasErrors );
	if ( ! *hasErrors ) {
		*hasErrors = parser( tl, handleTerminal, handleGetIntoRule, handleGetOutRule );
	}
	TokenList_Free( tl );
	
	return _root;
}

#endif //BUILD_AST_C