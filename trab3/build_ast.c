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
static subExp  = false;

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
		
		case TK_OR:
			return AST_OR; //unsafe, someone must tell that string is []char
		break;
		
		case TK_GREATER:
			return AST_GREATER; //unsafe, someone must tell that string is []char
		break;
		
		case TK_GREATER_EQUAL:
			return AST_GREATER_EQUAL; //unsafe, someone must tell that string is []char
		break;
		
		case TK_LESS:
			return AST_LESS; //unsafe, someone must tell that string is []char
		break;
		
		case TK_LESS_EQUAL:
			return AST_LESS_EQUAL; //unsafe, someone must tell that string is []char
		break;
		
		case TK_EQUAL:
			return AST_EQUAL; //unsafe, someone must tell that string is []char
		break;
		
		case TK_DIFFERENT:
			return AST_DIFFERENT; //unsafe, someone must tell that string is []char
		break;
		
		case TK_AND:
			return AST_AND; //unsafe, someone must tell that string is []char
		break;

		default: 
			printf( "Ooops... Isto não Deveria acontecer(%d)\n\n", tk );
			return (ASTNodeType)0;
		break;

	}
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
				node = AST_NewAsString( AST_VAR , line, str ); 
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_BLOCK:
				node = AST_New( AST_BLOCK, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_COMMAND_ELSE:
				_currParent = AST_GetParent( _currParent );
				node = AST_New( AST_ELSE, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_COMMAND_ELSE_IF:
				_currParent = AST_GetParent( _currParent );
				node = AST_New( AST_ELSE_IF, line );
				_currParent = AST_InsertChild( _currParent, node );
			break;
			
			case NT_EXPRESSION:
				if( !subExp) {
					node = AST_New( AST_EXPRESSION , line );
					AST_InsertChild( _currParent, node );
					//subExp = true;
				}
			break;
			
			/*case NT_TYPE:
				node = AST_New( AST_BLOCK, line );
				node = AST_InsertChild( _
			break;*/
			
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
				break;
				
				case TK_BOOL:
					node = AST_NewAsInt( AST_BOOL, line, dimension );
					AST_InsertChild( _currParent, node );
					dimension = 0;
				break;
				
				case TK_CHAR:
					node = AST_NewAsInt( AST_CHAR, line, dimension );
					AST_InsertChild( _currParent, node );
					dimension = 0;
				break;
				
				case TK_STRING:
					node = AST_NewAsInt( AST_CHAR, line, dimension+1 );
					AST_InsertChild( _currParent, node );
					dimension = 0;
				break;
				
				case TK_OP_BRACKET:
				break;
				
				case TK_CL_BRACKET:
					dimension = dimension + 1;
				break;
				
				default: break;
			}
				
		break;

		case NT_DECL_GLOBAL_VAR:
			switch ( tk ) {
				case TK_IDENTIFIER:
				break;
				
				case TK_COLON:
				break;
				
				case TK_NL:
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
				break;
				
				case TK_CL_BRACKET:
					_currParent = AST_GetParent( _currParent );
				break;
				
				default: break;
			}
		break;
/*
		case NT_VAL:
			switch ( tk ) {
				case TK_OP_PARENTHESIS:
				break;
				
				case TK_CL_PARENTHESIS:
				break;
				
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
			switch ( tk ) {
				case TK_MINUS:
					node  = AST_New( AST_UNARYMINUS, line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				case TK_NOT:
					node  = AST_New( AST_NOT, line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_MUL_DIV_OP:
			switch ( tk ) {
				case TK_MUL:
					node = AST_New( TkToASTType( TK_MUL ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_DIV:
					node = AST_New( TkToASTType( TK_DIV ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_MIN_ADD_OP:
			switch ( tk ) {
				case TK_PLUS:
					node = AST_New( TkToASTType( TK_PLUS ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_MINUS:
					node = AST_New( TkToASTType( TK_MINUS ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				default: break;
			}
		break;

		case NT_COMPARISON_OP:
			switch ( tk ) {
				case TK_GREATER:
					node = AST_New( TkToASTType( TK_GREATER ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_GREATER_EQUAL:
					node = AST_New( TkToASTType( TK_GREATER_EQUAL ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_LESS:
					node = AST_New( TkToASTType( TK_LESS ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_LESS_EQUAL:
					node = AST_New( TkToASTType( TK_LESS_EQUAL ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_EQUAL:
					node = AST_New( TkToASTType( TK_EQUAL ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				case TK_DIFFERENT:
					node = AST_New( TkToASTType( TK_DIFFERENT ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
				break;
				
				default: break;
			}
		break;
		

		case NT_AND:
			switch ( tk ) {
				case TK_AND:
					node = AST_New( TkToASTType( TK_AND ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );
					
				break;
				
				default: break;
			}
		break;
		*/
		case NT_EXPRESSION:
			switch ( tk ) {
				case TK_OR:
					/*node = AST_New( TkToASTType( tk ), line );
					_currParent = AST_InsertNewChildParentOfChildren( _currParent, node );*/
				break;
				
				default: break;
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

		case NT_CALL:
			switch ( tk ) {
				case TK_OP_PARENTHESIS:
				break;
				
				case TK_CL_PARENTHESIS:
					if ( AST_GetType( _currParent ) == AST_EXPRESSION ) {
						//_currParent = AST_GetParent( _currParent );	
					}
					_currParent = AST_GetParent( _currParent );
				break;
				
				default: break;
			}
		break;

		case NT_ATTR:
			switch ( tk ) {
				case TK_EQUAL:
					_currParent = AST_GetParent( _currParent );
				break;
				
				default: break;
			}
		break;

		case NT_ARRAY_ACCESS:
			switch ( tk ) {
				case TK_OP_BRACKET:
				break;
				
				case TK_CL_BRACKET:
					if( AST_GetType( _currParent ) == AST_EXPRESSION ) {
						_currParent = AST_GetParent( _currParent );	
					}
					/*_currParent = */AST_GetParent( _currParent );
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
				break;
				
				case TK_NL:
					_currParent = AST_GetParent( _currParent ); 
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
				case TK_IF: break;
				
				case TK_NL: 
					//UPNEXT
				break;
				
				default: break;
			}
		break;
		
		case NT_COMMAND_ELSE:
			switch ( tk ) {
				case TK_NL: 
				break;
			}
		break;
		case NT_COMMAND_IF:
			switch ( tk ) {
				case TK_IF:
					node = AST_New( AST_IF , line );
					_currParent = AST_InsertChild( _currParent, node );
				break;
				
				
				case TK_ELSE:
				break;
				
				case TK_NL:
					//_currParent = AST_GetParent( _currParent );
				break;
				
				
				case TK_END:
					_currParent = AST_GetParent( _currParent );
					if( AST_GetType( _currParent ) == AST_ELSE || AST_GetType( _currParent ) == AST_ELSE_IF ) {
						_currParent = AST_GetParent( _currParent );
					}
					_currParent = AST_GetParent( _currParent );
				break;
				//There are a lot of if, elseif and else questions
				default: break;
			}
		break;
			
			
		case NT_COMMAND:
			switch ( tk ) {
				case TK_NL:
					/*do {*/
					_currParent = AST_GetParent( _currParent );
		//			} while( AST_GetType( _currParent ) != AST_BLOCK );*/
				break;
				
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t ); //Just store it for future usage.
				break;
				
				default: break;
			}
		break; 

		case NT_BLOCK:
			switch ( tk ) {
				case TK_IDENTIFIER:
					str = Token_GetStringValue( t );
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
					_currParent = AST_InsertChild( _currParent, node);
				break;
				
				case TK_OP_PARENTHESIS:
				break;
				
				case TK_CL_PARENTHESIS:
					_currParent = AST_GetParent( _currParent );
				break;
				
				case TK_COLON:
				break;
				
				case TK_END:
					_currParent = AST_GetParent( _currParent );
					_currParent = AST_GetParent( _currParent );
				break;
				
				case TK_NL:
					if( AST_GetType( _currParent ) == AST_PARAM_LIST ) {
						_currParent = AST_GetParent( _currParent );
					}
				break;
				
				default: break;
			}
		break;

		case NT_DECL:
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






#endif //BUILD_AST_C