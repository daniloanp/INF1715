#ifndef  BUILD_AST_C
#define  BUILD_AST_C
#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "../trab1/lex.h"
#include "../trab1/token_list.h"
#include "../trab2/recursive_parser.h"
#include "build_ast.h"
#include "assert.h"


static AST _root = NULL;
static AST _currParent = NULL;
static char* str = NULL;
static unsigned long dimension = 0;
//this function assumes that the parser are ok;
int buildAst( NonTerminal rule, Token t, int line ) {
	AST node = NULL;
	TokenKind tk = Token_GetKind( t );


	if ( _root == NULL ) {
		root = AST_New( AST_Program, line );
		currParent = root;
	}

	switch ( rule ) {
		case NT_DECL_FUNCTION:
			switch( tk ) {
				case TK_IDENTIFIER:
					node = AST_New( AST_DeclFunction, line, AST_NodeValueFromToken( t ) );
					AST_InsertChild( currParent, node);
					currParent = node;
				break;
				case TK_CL_PARENTHESIS:
					currParent = AST_GetParent( currParent );
					if( AST_GetType( currParent ) == AST_DeclFunction ) {
						currParent = AST_GetParent( currParent );
					}
				case TK_END:
					currParent = AST_GetParent( currParent );
				break;
				default: break;
			}
		break;
		case NT_PARAM:
			if( AST_GetType( currParent ) != AST_ParamList ) {
				node = AST_New( AST_ParamList, line );
				currParent = AST_InsertChild( currParent, node);
			}
			switch( tk ) {
				case TK_IDENTIFIER:
					node = AST_New( AST_Param, line, AST_NodeValueFromToken( t ) );
					AST_InsertChild( currParent, node );
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
					node = AST_New( TkToASTType( tk ) , line, AST_NodeValueFromInt( dimension ) );
					AST_InsertChild( currParent, node );
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
					currParent = AST_GetParent( currParent );
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
			else if( tk == NL) {
				currParent = AST_GetParent( currParent );
			}
		break;

		case NT_COMMAND_IF:
			switch ( tk ) {

				case TK_IF:
					node = AST_New( AST_If , line );
					currParent = AST_InsertChild( currParent, node );
				break;

				case TK_END:
					currParent = AST_GetParent( currParent );
					if( AST_GetType( currParent ) == AST_Else || AST_GetType( currParent ) == AST_ElseIf ) {
						currParent = AST_GetParent( currParent );
					}
					currParent = AST_GetParent( currParent );
				break;

				case TK_NL: 
					currParent = AST_GetParent( currParent );
				break;

				default: break;
			}
			//IF QUESTIONS
		break;

		case NT_COMMAND_RETURN:
			node = AST_New( AST_Return , line );
			currParent = AST_InsertChild( currParent, node );
		break;

		case NT_COMMAND_WHILE:
			switch ( tk ) 
				case TK_WHILE:
					node = AST_New( AST_While , line );
					currParent = AST_InsertChild( currParent, node );
				break;
				
				case TK_LOOP:
					currParent = AST_GetParent( currParent );
					currParent = AST_GetParent( currParent );
				break;

				case NL:
					currParent = AST_GetParent( currParent );
				break;

				default: break;
		break;

		case NT_ATTR:
			if( t == NULL ) { //Começo da Regra
				node = AST_New( AST_Attr , line );
				currParent = AST_InsertChild( currParent, node );
				node = AST_NewAsString( AST_Var , line, str ); 
				currParent = AST_InsertChild( currParent, node );
			}
		break;

		case NT_EXPRESSION:
			node = AST_New( AST_Expression , line );
			currParent = AST_InsertChild( currParent, node );
		break;


		case NT_ARRAY_ACCESS:
			if( tk == TK_CL_BRACKET ) {
				if( AST_GetType( currParent ) == AST_Expression ) {
					currParent = AST_GetParent( currParent );	
				}
				currParent = AST_GetParent( currParent );
			}
		break;

		case NT_EXPRESSION_LIST:
			switch ( tk ) {
				case TK_COMMA:
					currParent = AST_GetParent( currParent );
				break;

				default: break;
			}
		break:

		case NT_NEW:
			switch ( tk ) {
				case TK_CL_BRACKET:
					currParent = AST_GetParent( currParent );
				break;

				case TK_NEW:
					node = AST_New( AST_New , line );
					currParent = AST_InsertChild( currParent, node );
				break;

				default: break;
			}
			
		break;

		case NT_CALL:
			switch ( tk ) {
				case TK_CL_PARENTHESIS:
					if ( AST_GetType(currParent) == AST_Expression ) {
						currParent = AST_GetParent( currParent );	
					}
					currParent = AST_GetParent( currParent );
				break;

				default: break;
			}
		break;

		case NT_VAL:
			switch ( tk ) {
				case TK_BOOL_VAL:
					node = AST_NewAsBool( AST_BoolVal, line, Token_GetBoolValue( t ) );
					AST_InsertChild( currParent, node );
				break;
				case TK_INT_VAL:
					node = AST_NewAsInt( AST_BoolVal, line, Token_GetIntValue( t ) );
					AST_InsertChild( currParent, node );

				break;
				case TK_STRING_VAL:
					node = AST_NewAsString( AST_BoolVal, line, Token_GetStringValue( t ) );
					AST_InsertChild( currParent, node );
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
					node  = AST_New( AST_UnaryMinus, line );
				} else {
					node  = AST_New( AST_Not, line );
				}
				currParent = AST_InsertChild( currParent, node );
			}
		break;

		case NT_MUL_DIV_OP:
		case NT_MIN_ADD_OP:
		case NT_COMPARISON_OP:
		case NT_AND:
			node = AST_New( TkToASTType( tk ), line );
			currParent = AST_InsertNewChildParentOfChildren( currParent, node );
		break;


		default: break;

	}

	return 1;
}

AST BuildAst( FILE* input, int* ret ) {
	_root = NULL; //Previous I'm be leek
	_currParent = NULL; //Previous I'm be leek
	TokenList tl;
	assert(input);
	tl = generateTokens( input, ret );
	if ( !(*ret) ) {
		*ret = parser( tl, buildAst );
	}
	tokenListDestroyKeepingStrings(tl);
	return AST_ROOT;
}






#endif //AST_H