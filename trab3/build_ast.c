#include <stdlib.h>
#include <stdio.h>
#include "AbstractSyntaxTree.h"
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "../trab2/recursiveParser.h"



static int priorityNumber( TokenKind tk) {
	if (tk == AND || tk == OR)
		return 1;
	if (tk == PLUS || tk == MINUS) {
		return 3;
	}
	if (tk == MUL || tk == DIV) {
		return 4;
	}
	if( tk == GREATER || tk == GREATER_EQUAL || tk == LESS || tk == LESS_EQUAL || tk == EQUAL || tk == DIFFERENT ) {
		return 2;
	}
	return -1;//Problem
}

int isbinOpPriorityGreater( TokenKind t1,  TokenKind t2 ) { // t1 é o pai
	int a =0 , b = 0;
	a = priorityNumber(t1);
	b = priorityNumber(t2);
	return a < b;	
}

int isConstant(TokenKind tk) {
	return (tk==INT_VAL || tk==STRING_VAL || tk==BOOL_VAL);
}

int isBinOp(TokenKind tk) {
	return( tk==PLUS || tk == MINUS || tk == MUL || tk == DIV || tk == AND || tk == OR || tk == GREATER || tk == GREATER_EQUAL || tk == LESS || tk == LESS_EQUAL || tk == EQUAL || tk == DIFFERENT );
}

int isBaseType(TokenKind tk) {
	return (tk == CHAR || tk == STRING || tk == INT || tk == BOOL);
}

AST root = NULL;

//this function assumes that the parser are ok;
int buildAst(int nodeType, int line, Token t) {
	static AST currParent = NULL;
	static Token tempId = NULL;
	static int bracketCount = 0; //work around
	ASTNodeType parentType;
	AST node = NULL;

	parentType = AST_GetType(currParent); //First Use

	if(  currParent == NULL ) { //Pode gerar ruido, perigoso
		currParent = AST_NewNode(AST_Program, line, NULL);
		parentType = AST_GetType(currParent);
		root = currParent;
	}
	
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
				AST_UpdateNodeValue(currParent,  AST_NodeValueFromToken( t ) ); //Place Name of Function to Node.
				return 0;//Não altera o Pai
			}
			else {
				node = AST_NewNode( AST_ParamList, line, NULL );
				currParent = AST_InsertChild( currParent, node ); //Agora o pai é a lista de parametros.
				node = AST_NewNode( AST_Param, line, AST_NodeValueFromToken( t ) );
				currParent = AST_InsertChild( currParent, node ); //Agora o Pai é um parametro em especial
				return 0;
			}
		}
	}
	//Resolver Declaração

	//Declara Variavel e Sobe pro bloco(ou program, ou Params)
	if( parentType == AST_DeclVar || parentType == AST_Param || parentType == AST_DeclFunction ) {
		if( isBaseType((TokenKind)nodeType) ) {//BeginType
				node = AST_NewNode((ASTNodeType)nodeType, line, AST_NodeValueFromInt(bracketCount)); //Type
				AST_InsertChild(currParent, node);
				// Tratamento diferenciado pra funções: 
				if(parentType != AST_DeclFunction) {
					currParent = AST_GetParent(currParent);
				} else {
					node = AST_NewNode(AST_Block, line, NULL);
					currParent = AST_InsertChild(currParent, node);
				}
				bracketCount = 0;
				return 0;
		} 
		else if( (TokenKind)nodeType == CL_BRACKET) { //unfortunately the "if" was necessary {
			bracketCount++;
			return 0 ;
		}
	}	
	

	if( parentType == AST_ParamList ) {
		if((TokenKind)nodeType == CL_PARENTHESIS) {//DEve funcionar, achei feio.
			currParent = AST_GetParent(currParent);
			return 0;
		}
		else if ( (TokenKind)nodeType == IDENTIFIER ) {
			node = AST_NewNode( AST_Param, line, AST_NodeValueFromToken( t ) );
			currParent =  AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_Block ) { // agora o - caótico - bloco.
		if( (TokenKind)nodeType == IDENTIFIER ) { //Ao ver um Id, não dá pra saber se é Call, attr ou declvar
			tempId = t;// Armazena o Token na Memória pra informações a posteriori.
			return 0 ;
		}
		if(tempId != NULL) {
			if( (TokenKind)nodeType == OP_PARENTHESIS ) { // Ih, o 'ID' era de uma função. Filho é 'CALL'(Chamada);
				
				node = AST_NewNode(AST_Call, tokenGetLine(tempId), AST_NodeValueFromToken(tempId));
				currParent = AST_InsertChild(currParent, node );  			
				tempId = NULL; //Pai Call
				return 0;

			}
			else if( (TokenKind)nodeType == COLON ) { 
				node = AST_NewNode( AST_DeclVar, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				currParent = AST_InsertChild( currParent, node ); //Pai DeclVar
				tempId = NULL;
				return 0;
			}
			else /*if((TokenKind)nodeType == EQUAL || (TokenKind)nodeType == OP_BRACKET)*/ {
				node = AST_NewNode( AST_Attr, line, NULL );
				currParent = AST_InsertChild( currParent, node ); 
				node = AST_NewNode( AST_Var, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				tempId = NULL;
				AST_InsertChild( currParent, node );
				if( (TokenKind)nodeType != EQUAL ) {
					currParent =  node;//Muda pai pra var		
					buildAst(nodeType, line, t );
					return 0;

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
			else if ((TokenKind)nodeType == RETURN) {
				node = AST_NewNode(AST_Return, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				return 0;
			} else if((TokenKind)nodeType == LOOP ) {
				currParent = AST_GetParent(AST_GetParent(currParent)); //Sai dois niveis.
				return 0;
			}
			else if ( (TokenKind)nodeType == END )  {
				currParent = AST_GetParent(AST_GetParent(currParent)); //Sai dois niveis.
				parentType = AST_GetType(currParent);
				if( parentType == AST_ElseIf || parentType == AST_Else ) {
					currParent = AST_GetParent(currParent); //Terceito
				}
				return 0;
			} else if(( TokenKind)nodeType == ELSE ) {
				//AST_prettyPrint(root, 0);
				//exit(1);
				currParent = AST_GetParent(currParent); //
				parentType = AST_GetType(currParent);
				if(parentType != AST_If) {
					currParent = AST_GetParent(currParent); //
					parentType = AST_GetType(currParent);
				}
				node = AST_NewNode(AST_Else, line, NULL);
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
		}
	}

	if( parentType == AST_Attr ) {
		if( ( TokenKind)nodeType == NL ) {
			currParent = AST_GetParent(currParent);
			return 0;
		} 
		else if( (TokenKind)nodeType == EQUAL ) {
			node = AST_NewNode( AST_Expression, line, NULL );
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_Return) {
		if( ( TokenKind)nodeType == NL ) {
			currParent = AST_GetParent(currParent);
			return 0;
		} else {
			node = AST_NewNode( AST_Expression, line, NULL);
			currParent = AST_InsertChild( currParent, node );
			return 0;
		}
	}

	if( parentType == AST_Var ) {
		if( (TokenKind)nodeType == CL_PARENTHESIS || (TokenKind)nodeType == NL || (TokenKind)nodeType == COMMA  || (TokenKind)nodeType == EQUAL) {
			//do {
			currParent = AST_GetParent(currParent);
			parentType == AST_GetType(currParent);
			//} while(parentType == AST_UnaryMinus || parentType == AST_Not);

			return buildAst(nodeType, line, t);
		}
		else if( (TokenKind)nodeType != CL_BRACKET ){
			node = AST_NewNode( AST_Expression, line, NULL );
			currParent = AST_InsertChild(currParent, node);
			return buildAst(nodeType, line, t);
		}
	}

	if( parentType == AST_New ) {
		if( AST_GetFirstChild( currParent ) == NULL ) {
			if( AST_GetFirstChild(currParent) == NULL ) {
				node = AST_NewNode(AST_Expression, line, NULL);	
				currParent = AST_InsertChild(currParent, node); //Now Parent is an Expression
				return 0;
			}
		} else {
			if( (TokenKind)nodeType == OP_BRACKET ) {
				bracketCount++;
				return 0;
			}
			else if( isBaseType((TokenKind)nodeType) ) {
				node = AST_NewNode((TokenKind)nodeType, line, AST_NodeValueFromInt(bracketCount));	
				AST_InsertChild(currParent, node); //Now Parent is an Type
				currParent = AST_GetParent(currParent);
				bracketCount = 0;
				return 0;
			} 
		}
	}


	if( parentType == AST_Call ) {
		if( (TokenKind)nodeType == NL ) { 
			currParent = AST_GetParent(currParent);
			return 0;
		}
		else if( (TokenKind)nodeType!=COMMA && (TokenKind)nodeType!=CL_PARENTHESIS && (TokenKind)nodeType!=CL_BRACKET) {
			
			node = AST_NewNode(AST_Expression, line, NULL);	
			currParent = AST_InsertChild(currParent, node); //Now Parent is an Expression
			//printf("A: %d", nodeType);
			buildAst(nodeType, line, t);
			return  0;
		}
	}

	if( parentType == AST_ElseIf) {
		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} else {}

	}

	if( parentType == AST_Else ) {
		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} else 	if( (TokenKind)nodeType == IF ) {
			AST_UpdateNodeType(currParent, AST_ElseIf);
			node = AST_NewNode( AST_Expression, line, NULL);
			currParent = AST_InsertChild( currParent, node );
			return 0;
		}
	}

	if( parentType == AST_If ) {

		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 

	}

	if( parentType == AST_While ) {
		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		}
	}

	if( parentType == AST_ElseIf ) {
		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		}
	}

	if( parentType == AST_Else ) {
		if((TokenKind)nodeType == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	//Constants & BinOp
	if( parentType == AST_Expression || parentType == AST_UnaryMinus || parentType == AST_Not|| isBinOp((TokenKind)parentType )) {
		if( (TokenKind)nodeType == IDENTIFIER ) { //VarORCall
			tempId = t;
			return 0;
		}
		else if( tempId !=NULL ) {
			if( (TokenKind)nodeType == OP_PARENTHESIS ) { //É chamada
				node = AST_NewNode( AST_Call, line, AST_NodeValueFromToken( tempId ) );
				tempId = NULL;
				currParent = AST_InsertChild(currParent, node);
				
			}
			else {

				node = AST_NewNode( AST_Var, line, AST_NodeValueFromToken( tempId ) );
				tempId = NULL;
				AST_InsertChild(currParent, node);
				
				if( nodeType == OP_BRACKET ) {
					currParent = node;
					return buildAst (nodeType, line, t);
				} else {
					return buildAst( nodeType, line, t );
				}

				while(parentType == AST_Not || parentType == AST_UnaryMinus) {
					currParent = AST_GetParent( currParent );
					parentType = AST_GetType( currParent );
				}
				
			}
			return 0;
		}
		else if( (TokenKind)nodeType == NEW ) {
			node = AST_NewNode(AST_New, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
		else if( isConstant((TokenKind)nodeType) ) { //Constant
			node = AST_NewNode((ASTNodeType)nodeType, line, AST_NodeValueFromToken(t));
			node = AST_InsertChild(currParent, node);
			while(parentType == AST_Not || parentType == AST_UnaryMinus) {
				currParent = AST_GetParent( currParent );
				parentType = AST_GetType( currParent );
			}
			return 0;
		}
		else if( (TokenKind)nodeType == NOT ) {
			node = AST_NewNode(AST_Not, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
			return 0;
		}
		else if( (TokenKind)nodeType == MINUS && ( AST_GetFirstChild(currParent) == NULL || (isBinOp((TokenKind)parentType) && AST_GetFirstChild(currParent) == AST_GetLastChild(currParent) )  ) ) { //É unário ou binário??? Resolvendo.
			node = AST_NewNode(AST_UnaryMinus, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
			return 0;
		}
		else if( isBinOp((TokenKind)nodeType) ) {
			//AST_prettyPrint(root,0);
			if(isBinOp((TokenKind)parentType)) { //Pai também é bin Op

				//Se não é. insere embaixo meesmo.
				if( isbinOpPriorityGreater((TokenKind)parentType, (TokenKind)nodeType) ) {

					
					node = AST_GetLastChild(currParent);
					node = AST_RemoveChild(currParent, node);
					currParent = AST_InsertChild( currParent, AST_NewNode((ASTNodeType)nodeType, line, NULL) );
					node = AST_InsertChild( currParent, node );
					parentType = AST_GetType(currParent);
				
					return 0;
				}
				else {
					node = AST_GetParent(currParent); //vovo
					currParent = AST_RemoveChild(node, currParent);
					node = AST_InsertChild( node, AST_NewNode((ASTNodeType)nodeType, line, NULL) );
					AST_InsertChild( node, currParent );
					currParent = node;
					return 0;
				}
				return 0;
			}
			node = AST_NewNode((ASTNodeType)nodeType, line, NULL);
			currParent = AST_InsertNewChildParentOfChildren(currParent, node); // Abaixa a rapaziada
			return 0;
		} else {
			
			if( (TokenKind)nodeType == NL || (TokenKind)nodeType == CL_BRACKET || (TokenKind)nodeType == COMMA ) {
				do {
					currParent = AST_GetParent(currParent);
					parentType = AST_GetType(currParent);
				} while( parentType == AST_Expression || parentType == AST_UnaryMinus || parentType == AST_Not|| isBinOp((TokenKind)parentType ));
				
				
				return buildAst( nodeType, line, t );
			}
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
		if(!ret) {
			ret = parser(tl, buildAst);
			AST_prettyPrint(root, 1);
		}

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