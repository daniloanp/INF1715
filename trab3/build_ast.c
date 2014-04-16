#include <stdlib.h>
#include <stdio.h>
#include "AbstractSyntaxTree.h"
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "../trab2/recursiveParser.h"


int isConstant(TokenKind tk) {
	return (tk==INT_VAL || tk==STRING_VAL || tk==BOOL_VAL);
}

int isBinOp(TokenKind tk) {
	return( tk==PLUS || tk == MINUS || tk == MUL || tk == DIV || tk == AND || tk == OR || tk == GREATER || tk == GREATER_EQUAL || tk == LESS || tk == LESS_EQUAL || tk == EQUAL || tk == DIFFERENT );
}

int isBaseType(Token tk) {
	return (tk == CHAR || tk == STRING || tk == INT || tk == BOOL);
}

AST root = NULL;
//this function assumes that the parser are ok;
int buildAst(int nodeType, int line, Token t) {
	static AST currParent = NULL;
	static AST currSibling = NULL;
	static Token tempId = NULL;
	static AST tempVar = NULL;
	static int bracketCount = 0; //work around
	static int status = 0;
	ASTNodeType parentType;
	AST node = NULL;

	if((ASTNodeType)nodeType == AST_Program) {
		currParent = AST_NewNode(AST_Program, line, NULL);
		root = currParent;
		return 0 ;
	}

	parentType = AST_GetType(currParent); //First Use
	
	
	//Tô em Program
	if(parentType == AST_Program) {
		if( (TokenKind)nodeType == FUN ) {
			node = AST_NewNode(AST_DeclFunction, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 
		else if ( (TokenKind)nodeType == IDENTIFIER ) {
			node = AST_NewNode(AST_DeclVar, line, AST_NodeValueFromToken(t));//Already Get Id
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_DeclFunction ) {
		if( (TokenKind)nodeType == IDENTIFIER ) {
			if( AST_GetNodeValue( currParent ) == NULL) {
				node = AST_UpdateNodeValue(currParent,  AST_NodeValueFromToken( t ) ); //Place Name of Function to Node.
				return 0;//Não altera o Pai
			}
			else {
				node = AST_NewNode( AST_ParamList, line, NULL );
				currParent = AST_InsertChild( currParent, node ); //Agora o pai é a lista de parametros.
				node = AST_NewNode( AST_Param, line, AST_NodeValueFromToken( t ) );
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
		}
		else if((TokenKind)nodeType == NL) {
			node = AST_NewNode(AST_Block, line, NULL);
			AST_InsertChild(currParent, node);
			currParent = node;
			return 0 ;
		}
	}



	//Resolver Declaração

	//Declara Variavel e Sobe pro bloco(ou program, ou Params)
	if( parentType == AST_DeclVar || parentType == AST_Param || parentType == AST_DeclFunction ) {
		if( isBaseType((TokenKind)nodeType) ) {//BeginType
				node = AST_NewNode((ASTNodeType)nodeType, line, bracketCount); //Type
				AST_InsertChild(currParent, node);
				// Tratamento diferenciado pra funções: 
				if(parentType!= AST_DeclFunction) {
					currParent = AST_GetParent(currParent);
				}
				bracketCount = 0;
				return 0;
		} 
		else if( (TokenKind)nodeType == CL_BRACKET) //unfortunately the "if" was necessary {
			bracketCount++;
			return 0 ;
		}
	}	

	

	if( parentType == AST_ParamList ) {
		if((TokenKind)nodeType == CL_PARENTHESIS) {//DEve funcionar, achei feio.
			currParent = AST_GetParent(currParent);
		}
		else if ( (TokenKind)nodeType == IDENTIFIER ) {
			node = AST_NewNode(AST_Param, line, AST_NodeValueFromToken( t ));
			currParent =  AST_InsertChild(currParent, node);
		}
	}

	if( parentType == AST_Block) { // agora o - caótico - bloco.
		if((TokenKind)nodeType == IDENTIFIER) { //Ao ver um Id, não dá pra saber se é Call, attr ou declvar
			tempId = t;// Armazena o Token na Memória pra informações a posteriori.
			return 0 ;
		}
		if(tempId != NULL) {
			if((TokenKind)nodeType == OP_PARENTHESIS) { // Ih, o 'ID' era de uma função. Filho é 'CALL'(Chamada);
				
				node = AST_NewNode(AST_Call, tokenGetLine(tempId), AST_NodeValueFromToken(tempId));
				currParent = AST_InsertChild(currParent, node );  			
				tempId = NULL; //Pai Call
				return 0;
			}
			else if((TokenKind)nodeType == COLON) { // Feio, mas tbm necessario
				node = AST_NewNode( AST_DeclVar, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				currParent = AST_InsertChild( currParent, node ); //Pai DeclVar
				return 0;
			}
			else /*if((TokenKind)nodeType == EQUAL || (TokenKind)nodeType == OP_BRACKET)*/ {
				node = AST_NewNode( AST_Attr, line, NULL );
				currParent = AST_InsertChild( currParent, node ); 
				node = AST_NewNode( AST_Var, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				AST_InsertChild( currParent, node );
				if( (TokenKind)nodeType != EQUAL ) {
					currParent =  node//Muda pai pra var					
				}
				return 0;
			}
		} else {
			if( (TokenKind)nodeType == WHILE ) {
				node = AST_NewNode(AST_While, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				node = AST_NewNode( AST_Expression, line, NULL);
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
			else if ((TokenKind)nodeType == IF) {
				node = AST_NewNode(AST_If, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				node = AST_NewNode( AST_Expression, line, NULL);
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
		}
	}


	if( parentType == AST_Var ) {
		if((TokenKind)nodeType == EQUAL) {
			currParent = AST_GetParent(currParent);
			return 0;
		} 
		else {
			node = AST_NewNode(AST_Expression, line, NULL);
			currParent = AST_InsertChild(currParent, node);
		}
	}


/*	if( parentType == AST_Expression && (TokenKind)nodeType == CL_BRACKET) {
		currParent = AST_GetParent(currParent);
		return 0;
	}*/

	if( parentType == AST_NEW ) {
		if( (TokenKind)nodeType == OP_BRACKET ) {
			if( AST_GetFirstChild(currParent) == NULL ) {
				node = AST_NewNode(AST_Expression, line, NULL);	
				currParent = AST_InsertChild(currParent, node); //Now Parent is an Expression
			} 
			else {
				node = AST_NewNode(AST_Type, line, NULL);	
				currParent = AST_InsertChild(currParent, node); //Now Parent is an Type
			}
		}
	}

	//Constants & BinOp
	if( parentType == AST_Expression || parentType == AST_UNARY_MINUS || parentType == AST_NOT|| isBinOp((TokenKind)parentType )) {

		if( (TokenKind)nodeType == IDENTIFIER ) { //VarORCall
			tempId = t;
			return 0;
		}
		else if( tempId !=NULL ) {
			if( (TokenKind)nodeType == OP_PARENTHESIS ) { //É chamada
				node = AST_NewNode( AST_Call, line, AST_NodeValueFromToken( tempId ) );
				currParent = AST_InsertChild(currParent, node);
				
			}
			else {
				node = AST_NewNode( AST_Var, line, AST_NodeValueFromToken( tempId ) );
				AST_InsertChild(currParent, node);
				if( nodeType == OP_BRACKET ) {
					currParent = node;
				}
			}
			tempId = NULL;
			return 0;
		}
		else if( (TokenKind)nodeType == NEW ) {
			node = AST_NewNode(AST_NEW, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
		else if( isConstant((TokenKind)nodeType) ) { //Constant
			node = AST_NewNode((ASTNodeType)nodeType, line, AST_NodeValueFromToken(t));
			node = AST_InsertChild(currParent, node);
		}
		else if( (TokenKind)nodeType == NOT ) {
			node = AST_NewNode(AST_NOT, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
		}
		else if( (TokenKind)nodeType == MINUS ) { //É unário ou binário??? Resolvendo.
			node = AST_NewNode(AST_UNARY_MINUS, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
		}
		else if( isBinOp((TokenKind)nodeType) ) {
			if(AST_GetFirstChild(currParent) == NULL) {
				return 0;
			}
			node = AST_NewNode((ASTNodeType)nodeType, line, NULL);
			currParent = AST_InsertNewChildParentOfChildren(currParent, node);
		}


	}

	

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