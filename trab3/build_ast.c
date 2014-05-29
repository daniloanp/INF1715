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
			printf("\n\nError: %d !!!\n\n", tp);
			return 0;
		break;
	}
}


AST upToExp( AST parent ) {
	AST node;
	for (	
			node = parent; 
			(node!=NULL) && AST_GetType( node ) != AST_EXPRESSION; 
			node = AST_GetParent( node ) 
		);
	if( node == NULL) {
		return parent;
	}
	else {
		return node;
	}
	
}

bool cmpBinOp( AST op, AST newOp ) {
	return ( priorityLevel( newOp ) > priorityLevel( op ) );
}

void insertBinOp( AST _currParent, AST nodeOp  ) {
	AST parent;
	AST parentExp;
	AST child;

	for (	parentExp = AST_GetFirstChild( upToExp( _currParent ) ); 
			cmpBinOp( parentExp, nodeOp );
			parentExp = AST_GetLastChild( parentExp )
		);

	parent = AST_GetParent(parentExp);
	child = AST_RemoveChild( parent , parentExp );
	AST_InsertChild( nodeOp, child );
	AST_InsertChild( parent, nodeOp );
}

void insertUnOp( AST _currParent, AST nodeOp  ) {
	AST node;
	for ( node = upToExp( _currParent );
		AST_GetLastChild( node ) != AST_GetFirstChild( node )
		|| AST_GetLastChild( node ) != NULL && ( AST_GetType( node ) == AST_UNARYMINUS || AST_GetType( node ) == AST_NOT );
		node = AST_GetLastChild( node ) );
	AST_InsertChild( node, nodeOp );
}

void insertVal( AST currParent, AST nodeVal ) {
	AST node;
	for ( node = upToExp( currParent ) ;
		AST_GetLastChild( node ) != AST_GetFirstChild( node )
		|| AST_GetLastChild( node ) != NULL && ( AST_GetType( node ) == AST_UNARYMINUS || AST_GetType( node ) == AST_NOT || AST_GetType( node ) == AST_EXPRESSION );
		node = AST_GetLastChild( node ) );
	AST_InsertChild( node, nodeVal );			
}



//this function assumes that the parser are ok;
int buildAst( NonTerminal rule, Token t, int line ) {
	AST node = NULL;
	TokenKind tk;
		
	if( t == NULL ) { //NonTerminal 
		switch ( rule ) {
			case NT_PROGRAM:
				_root = AST_New( AST_PROGRAM, line );
				_currParent = _root;
			break;
		
			case NT_PARAMS:
				node = AST_New( AST_PARAM_LIST, line );
				_currParent = AST_InsertChild( _currParent, node);
			break;
			
			case NT_ATTR:
				node = AST_New( AST_ATTR , line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_BLOCK:
				node = AST_New( AST_BLOCK, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_COMMAND_ELSE:
				_currParent = AST_GetParent( _currParent ); //take out from block
				if( AST_GetType( _currParent ) != AST_IF ) { //take out from an elseif
					_currParent = AST_GetParent( _currParent );
				}
				node = AST_New( AST_ELSE, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_COMMAND_ELSE_IF:
				_currParent = AST_GetParent( _currParent ); //take out from block
				if( AST_GetType( _currParent ) != AST_IF ) { //take out from an elseif
					_currParent = AST_GetParent( _currParent );
				}
				node = AST_New( AST_ELSE_IF, line );
				_currParent = AST_InsertChild( _currParent, node );
				node = AST_New( AST_EXPRESSION, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_EXPRESSION:
				if( AST_GetType( _currParent ) == AST_RETURN ) {
					node = AST_New( AST_EXPRESSION , line );
					_currParent = AST_InsertChild( _currParent, node );
				}
			break;

			case NT_EXPRESSION_LIST:
				node = AST_New( AST_EXPRESSION, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;

			case NT_DECL_VAR:
					node = AST_NewAsString( AST_DECL_VAR, line, str );
					_currParent = AST_InsertChild( _currParent, node );
					str = NULL;
			break;

			case NT_ARRAY_ACCESS:
				node = AST_NewAsString( AST_VAR , line, str ); 
				_currParent = upToExp( _currParent );
				if( AST_GetType( _currParent) == AST_EXPRESSION ) {
					insertVal( _currParent, node );
					_currParent = node;
				} 
				else {
					_currParent = AST_InsertChild( _currParent, node );	
				}
				
				str = NULL;
			break;
			default: break;
		}
		return 0;
	}

	tk = Token_GetKind( t );
	switch ( rule ) {
		case NT_TYPE:
			switch ( tk ) {
				case TK_INT:
					node = AST_NewAsInt( AST_INT, line, dimension );
					AST_InsertChild( _currParent, node );
					dimension = 0;
					_currParent = upToExp( _currParent );
				break;
				
				case TK_BOOL:
					node = AST_NewAsInt( AST_BOOL, line, dimension );
					AST_InsertChild( _currParent, node );
					dimension = 0;
					_currParent = upToExp( _currParent );
				break;
				
				case TK_CHAR:
					node = AST_NewAsInt( AST_CHAR, line, dimension );
					AST_InsertChild( _currParent, node );
					dimension = 0;
					_currParent = upToExp( _currParent );
				break;
				
				case TK_STRING:
					node = AST_NewAsInt( AST_CHAR, line, dimension+1 );
					AST_InsertChild( _currParent, node );
					dimension = 0;
					_currParent = upToExp( _currParent );
				break;
				
				case TK_OP_BRACKET:
				break;
				
				case TK_CL_BRACKET:
					dimension = dimension + 1;
				break;
				
				default: break;
			}
		break;

		case NT_DECL_VAR:
			switch ( tk ) {
				case TK_COLON:
				break;
				
				case TK_NL:
					_currParent = AST_GetParent( _currParent );
				break;
				
				default: break;
			}
		break;

		case NT_PARAM:
			switch ( tk ) {
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_PARAM, line, Token_GetStringValue( t ) );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_COLON:
				break;
				
				default: break;
			}
		break;

		case NT_PARAMS:
			switch ( tk ) {
				case TK_COMMA:
					_currParent = AST_GetParent( _currParent );
				break;
				
				default: break;
			}
		break;

		case NT_DECL_OR_COMMAND:
			switch ( tk ) {
				case TK_COLON:
				break;
				
				case TK_NL:
					_currParent = AST_GetParent( _currParent );
					if(AST_GetType( _currParent ) != AST_BLOCK) {
						_currParent = AST_GetParent( _currParent );	
					}
				break;
				
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
				break;
				
				default: break;
			}
		break;

		case NT_NEW:
			switch ( tk ) {
				case TK_NEW:
					node = AST_New( AST_NEW , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_OP_BRACKET:
					node = AST_New( AST_EXPRESSION , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_CL_BRACKET:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent ); // get out from exp?
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
					insertVal( _currParent, node );
				break;
				
				case TK_INT_VAL:
					node = AST_NewAsInt( AST_INT_VAL, line, Token_GetIntValue( t ) );
					insertVal( _currParent, node );
				break;
				
				case TK_STRING_VAL:
					node = AST_NewAsString( AST_STRING_VAL, line, Token_GetStringValue( t ) );
					insertVal( _currParent, node );
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
					insertUnOp( _currParent, node );
				break;
				
				case TK_NOT:
					node  = AST_New( AST_NOT, line );
					insertUnOp( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_MUL_DIV_OP:
			switch ( tk ) {
				case TK_MUL:
					node = AST_New( AST_MUL , line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_DIV:
					node = AST_New( AST_DIV , line );
					insertBinOp( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_MIN_ADD_OP:
			switch ( tk ) {
				case TK_PLUS:
					node = AST_New( AST_PLUS , line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_MINUS:
					node = AST_New( AST_MINUS , line );
					insertBinOp( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_COMPARISON_OP:
			switch ( tk ) {
				case TK_GREATER:
					node = AST_New( AST_GREATER, line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_GREATER_EQUAL:
					node = AST_New( AST_GREATER_EQUAL, line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_LESS:
					node = AST_New( AST_LESS , line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_LESS_EQUAL:
					node = AST_New( AST_LESS_EQUAL , line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_EQUAL:
					node = AST_New( AST_EQUAL, line );
					insertBinOp( _currParent, node );
				break;
				
				case TK_DIFFERENT:
					node = AST_New( AST_DIFFERENT, line );
					insertBinOp( _currParent, node );
				break;
				
				default: break;
			}
		break;
		

		case NT_AND:
			switch ( tk ) {
				case TK_AND:
					node = AST_New( AST_AND , line );
					insertBinOp( _currParent, node );
				break;
				
				default: break;
			}
		break;
		
		case NT_EXPRESSION:
			switch ( tk ) {
				case TK_OR:
					node = AST_New( AST_OR, line );
					insertBinOp( _currParent, node );
				break;
				
				default: break;
			}
		break;
			
		case NT_EXPRESSION_LIST:
			switch ( tk ) {
				case TK_COMMA:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent ); //Get out from exp
					node = AST_New( AST_EXPRESSION, line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_CALL:
			switch ( tk ) {
				case TK_OP_PARENTHESIS:
					node = AST_NewAsString(AST_CALL, line, str);
					str = NULL;
					_currParent = upToExp( _currParent );
					if( AST_GetType( _currParent) == AST_EXPRESSION ) {
						insertVal( _currParent, node );
						_currParent = node;
					} 
					else {
						_currParent = AST_InsertChild( _currParent, node );	
					}

					
					
				break;
				
				case TK_CL_PARENTHESIS:
					_currParent = upToExp( _currParent );
					if( AST_GetType( _currParent ) == AST_EXPRESSION ) {
						_currParent = AST_GetParent( _currParent );//sai da EXP
					}
					if( AST_GetType( AST_GetParent( _currParent ) ) != AST_BLOCK ) {
						_currParent = upToExp( _currParent );
					}
				break;
				
				default: break;
			}
		break;

		case NT_ATTR:
			switch ( tk ) {
				case TK_EQUAL:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent ); // get out from var or exp
					if( AST_GetType( _currParent ) != AST_ATTR ) {
						_currParent = AST_GetParent( _currParent ); // get out from var
					}
					node = AST_New( AST_EXPRESSION, line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_ARRAY_ACCESS:
			switch ( tk ) {
				case TK_OP_BRACKET:
					node = AST_New( AST_EXPRESSION, line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_CL_BRACKET:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent ); // get out from exp
				break;
				
				default: break;
			}
		break;

		case NT_COMMAND_ATTR_OR_CALL:

		break;

		case NT_COMMAND_WHILE:
			switch ( tk ) {
				case TK_WHILE:
					node = AST_New( AST_WHILE , line );
					_currParent = AST_InsertChild( _currParent, node );
					node = AST_New( AST_EXPRESSION , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_NL:
						_currParent = upToExp( _currParent );
						_currParent = AST_GetParent( _currParent ); //Take out from exp
				break;
				
				case TK_LOOP:
					_currParent = AST_GetParent( _currParent ); //Take out from block only
				break;
				
				default: break;
			}
		break;

		case NT_COMMAND_RETURN:
			switch ( tk ) {
				case TK_RETURN:
					node = AST_New( AST_RETURN , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				default: break;
			}
		break;
		case NT_COMMAND_ELSE_IF:
			switch ( tk ) {
				case TK_NL:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent ); //Take out from exp
				break;

				default: break;
			}
		break;
		
		case NT_COMMAND_ELSE:
			switch ( tk ) {
				case TK_NL: //Nothing to do, block is created from another field
				break;
			}
		break;

		case NT_COMMAND_IF:
			switch ( tk ) {
				case TK_IF:
					node = AST_New( AST_IF , line );
					_currParent = AST_InsertChild( _currParent, node );
					node = AST_New( AST_EXPRESSION , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_NL:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent );  //Take out from exp
				break;
				
				case TK_END:
					_currParent = AST_GetParent( _currParent );
					if ( AST_GetType( _currParent ) == AST_ELSE 
						|| AST_GetType( _currParent ) == AST_ELSE_IF 
				 		) {
						_currParent = AST_GetParent( _currParent );
					}
				break;
				
				default: break;
			}
		break;
			
			
		case NT_COMMAND:
			switch ( tk ) {
				case TK_NL:
					_currParent = upToExp( _currParent );
					_currParent = AST_GetParent( _currParent );
					while( AST_GetType( _currParent) != AST_BLOCK ) {
						_currParent = AST_GetParent( _currParent );
					}
				break;
				
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
				case TK_FUN:
				break;
				
				case TK_IDENTIFIER:
					node = AST_NewAsString( AST_DECL_FUNCTION, line, Token_GetStringValue( t ) );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_OP_PARENTHESIS:
				break;
				
				case TK_CL_PARENTHESIS:
					if( AST_GetType( _currParent ) == AST_PARAM ) {
						_currParent = AST_GetParent( _currParent );
					}
					if( AST_GetType( _currParent ) == AST_PARAM_LIST ) {
						_currParent = AST_GetParent( _currParent );
					}
				break;
				
				case TK_COLON:
				break;
				
				case TK_END:

					_currParent = AST_GetParent( _currParent ); //BLOCK?
					_currParent = AST_GetParent( _currParent );//DEclFunction?
					if( AST_GetType(_currParent) != AST_PROGRAM ) {
						_currParent = AST_GetParent( _currParent );//DEclFunction?
					}
				break;
				
				case TK_NL:
					//DOis casos...
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

		case NT_PROGRAM:
			switch ( tk ) {
				case TK_NL:
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
		*hasErrors = parser( tl, buildAst );
	}
	TokenList_Free( tl );
	
	return _root;
}

#endif //BUILD_AST_C