#include <stdlib.h>
#include <stdio.h>
#include "AbstractSyntaxTree.h"
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "../trab2/recursiveParser.h"



AST root = NULL;
//this function assumes that the parser are ok;
int buildAst(int nodeType, int line, Token t) {
	static AST currParent = NULL;
	static AST currSibling = NULL;
	static Token tempId = NULL;
	static AST tempVar = NULL;
	static int bracketCount = 0; //work around

	static int status = 0;
	AST node = NULL;

	if((ASTNodeType)nodeType == AST_Program) {
		currParent = AST_NewNode(AST_Program, line, NULL);
		root = currParent;
		return 0 ;
	}

	ASTNodeType parentType = AST_GetType(currParent);
	
	if( (ASTNodeType)nodeType == AST_DeclFunction || (ASTNodeType)nodeType == AST_DeclGlobalVar ) {
		node = AST_NewNode((ASTNodeType)nodeType, line, NULL);
		AST_InsertChild(currParent, node);
		currParent=node;
		return 0 ;
	}
	
	//END
	/*if( parentType == AST_CommandWhile || parentType == AST_CommandIf || parentType == AST_DeclFunction ) {
		if((ASTNodeType)nodeType == AST_END) {
			currParent=AST_GetParent(currParent);
			return 0;
		} else	{

		}
	}*/

	if( parentType == AST_DeclGlobalVar && (ASTNodeType)nodeType == AST_NL ) {
			currParent=AST_GetParent(currParent);
			return 0;
	}

	if( parentType == AST_Param && (ASTNodeType)nodeType == AST_COMMA ){
			currParent=AST_GetParent(currParent);
			return 0;
	}

	if( parentType == AST_Param && (ASTNodeType)nodeType == AST_CL_PARENTHESIS ) {
			currParent=AST_GetParent(AST_GetParent(currParent));
			return 0;
	}

	if( parentType == AST_DeclGlobalVar || parentType == AST_Param ) {
		
		if( (TokenKind)nodeType == IDENTIFIER ) {
			node = AST_NewNode((ASTNodeType)nodeType, line, AST_NodeValueFromToken( t ));
			AST_InsertChild(currParent, node);
			//currParent = node;
		}
		else if( (TokenKind)nodeType == COLON ) {//BeginType
				node = AST_NewNode(AST_Type, line, NULL);
				AST_InsertChild(currParent, node);
				currParent = node;
				bracketCount = 0;
		}
	}

	if( parentType == AST_Type) {
		if( (TokenKind)nodeType == CL_BRACKET ) {
				bracketCount++;
		}
		else if( (TokenKind)nodeType == INT || (TokenKind)nodeType == CHAR || (TokenKind)nodeType == BOOL || (TokenKind)nodeType == STRING ) {
			node = AST_NewNode((ASTNodeType)nodeType,  line, AST_NodeValueFromInt(bracketCount));
			AST_InsertChild( currSibling, node );
			currParent = AST_GetParent(currParent);//End IT;
			if(AST_GetType(currParent) == AST_DeclVar) {
				currParent = AST_GetParent(currParent);
			}
			bracketCount = 0;
		}
	}	

	if( parentType == AST_DeclFunction ) {
		if( (TokenKind)nodeType == IDENTIFIER ) {
			node = AST_NewNode(AST_IDENTIFIER, line, AST_NodeValueFromToken( t ));
			AST_InsertChild(currParent, node);
		} 
		else if( (TokenKind)nodeType == OP_PARENTHESIS ) {
			node = AST_NewNode(AST_Params, line, NULL);
			AST_InsertChild(currParent, node);
			currParent = node;
		}
		else if( (TokenKind)nodeType == COLON) {
			node = AST_NewNode(AST_Type, line, NULL);
			AST_InsertChild(currParent, node);
			currParent = node;
		}
		else if((ASTNodeType)nodeType == AST_Block) {
			node = AST_NewNode(AST_Block, line, NULL);
			AST_InsertChild(currParent, node);
			currParent = node;
		}
	}

	if( parentType == AST_Params ) {
		if((ASTNodeType)nodeType == AST_CL_PARENTHESIS) {
			currParent = AST_GetParent(currParent);
		}
		else if ( (ASTNodeType)nodeType == AST_Param ) {
			node = AST_NewNode(AST_Param, line, NULL);
			AST_InsertChild(currParent, node);
			currParent = node;
		}
	}

	if( parentType == AST_Block) {
		if((TokenKind)nodeType == IDENTIFIER) {
			tempId = t;//AST_NewNode(AST_IDENTIFIER, line, AST_NodeValueFromToken( t ));
			return 0 ;
		}
		if((ASTNodeType)nodeType == AST_COLON) {
			if(tempId == NULL) {
				printf("PRoblema \n\n");
				return 0;
			}
			if(AST_GetFirstChild(currParent) == NULL) {
				currParent = AST_InsertChild(currParent, AST_NewNode(AST_DeclVars, line, NULL));
			}

			node = AST_NewNode(AST_DeclVar, tokenGetLine(tempId), AST_NodeValueFromToken(tempId));
			currParent = AST_InsertChild(currParent, node ); 
			node = AST_NewNode(AST_Type, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			
			tempId = NULL;
		}
		else if((TokenKind)nodeType == OP_PARENTHESIS) { //CALL
			if(tempId == NULL) {
				printf("PRoblema \n\n");
				return 0;
			}
			if( AST_GetLastChild(currParent) == NULL ) {
				currParent = AST_InsertChild( currParent, AST_NewNode(AST_Commands, line, ) );
			}
			node = AST_NewNode( AST_Call, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
			currParent = AST_InsertChild( currParent, node ); 
			//INCOMPLETE
		}
		else if((TokenKind)nodeType == EQUAL || (TokenKind)nodeType == OP_BRACKET) {
			if(tempId == NULL) {
				printf("PRoblema \n\n");
				return 0;
			}
			//ATTR
			if( AST_GetLastChild(currParent) == NULL ) {
				currParent = 	AST_InsertChild( currParent, AST_NewNode(AST_Commands, line, ) );
			}

			node = AST_NewNode(AST_Attr, line, NULL);
			currParent = AST_InsertChild( currParent, node );

		}
		

		
		//else if(AST_GetFirstChild(currParent)) {
	}

	/*if( parentType == AST_DeclFunction) {

	}*/


	return 1;
}

int main( int argc, char **argv ) {	
	int ret;
	TokenList tl;
	FILE *input;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tl = generateTokens(input, &ret);
		if(!ret)
			ret = parser(tl, buildAst);

		if( input != stdin )
			fclose(input);
	}
	else {
		"Cannot open file";
		return 1;
	}

	if(ret == 0 )
		printf("Correct Syntax!!!\n");


	return ret;
}