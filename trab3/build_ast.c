#include <stdlib.h>
#include <stdio.h>
#include "AbstractSyntaxTree.h"
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "../trab2/recursiveParser.h"



static int priorityNumber( TokenKind tk) {
	if (tk == OR) {
		return 1;
	}
	if (tk == AND) {
		return 2;
	}
	if (tk == PLUS || tk == MINUS) {
		return 4;
	}
	if (tk == MUL || tk == DIV) {
		return 5;
	}
	if( tk == GREATER || tk == GREATER_EQUAL || tk == LESS || tk == LESS_EQUAL || tk == EQUAL || tk == DIFFERENT ) {
		return 3;
	}
	return -1;//Problem
}

int isbinOpPriorityGreater( TokenKind t1,  TokenKind t2 ) { // t1 é o pai
	int a =0 , b = 0;	
	a = priorityNumber(t1);
	b = priorityNumber(t2);
	return a <= b;	
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

static AST root = NULL;

//this function assumes that the parser are ok;
int buildAst( Token t, int line ) {
	static AST currParent = NULL;
	static Token tempId = NULL;
	static int bracketCount = 0; //work around
	TokenKind tk = tokenGetKind( t );
	ASTNodeType parentType;
	AST node = NULL;

	parentType = AST_GetType(currParent); //First Use

	if( currParent == NULL ) { //Pode gerar ruido, perigoso
		currParent = AST_NewNode(AST_Program, line, NULL);
		parentType = AST_GetType(currParent);
		root = currParent;
	}
	
	if( parentType == AST_Program ) {
		if( tk == FUN ) {
			node = AST_NewNode(AST_DeclFunction, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 
		else if ( tk == IDENTIFIER ) {
			node = AST_NewNode(AST_DeclVar, line, AST_NodeValueFromToken(t));//Already Get Id
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_DeclFunction ) {
		if( tk == IDENTIFIER ) {
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
		else if( tk == NL ) {
				node = AST_NewNode(AST_Block, line, NULL);
				currParent = AST_InsertChild(currParent, node);
				return 0;
		}
		else if( tk == END ) {
			currParent = AST_GetParent(currParent);
			return 0;
		}
	}

	//Declara Variavel e Sobe pro bloco(ou program, ou Params)
	if( parentType == AST_DeclVar || parentType == AST_Param || parentType == AST_DeclFunction ) {
		if( isBaseType(tk) ) {//BeginType
				node = AST_NewNode((ASTNodeType)tk, line, AST_NodeValueFromInt(bracketCount)); //Type
				AST_InsertChild(currParent, node);
				// Tratamento diferenciado pra funções: 
				if(parentType != AST_DeclFunction) {
					currParent = AST_GetParent(currParent);
				} 
				bracketCount = 0;
				return 0;
		} 
		else if( tk == CL_BRACKET) { 
			bracketCount++;
			return 0 ;
		}
	}	
	

	if( parentType == AST_ParamList ) {
		if(tk == CL_PARENTHESIS) {
			currParent = AST_GetParent(currParent);
			return 0;
		}
		else if ( tk == IDENTIFIER ) {
			node = AST_NewNode( AST_Param, line, AST_NodeValueFromToken( t ) );
			currParent =  AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_Block ) { // agora o - caótico - bloco.
		if( tk == IDENTIFIER ) { //Ao ver um Id, não dá pra saber se é Call, attr ou declvar
			tempId = t;// Armazena o Token na Memória pra informações a posteriori.
			return 0 ;
		}
		if(tempId != NULL) {
			if( tk == OP_PARENTHESIS ) { // Ih, o 'ID' era de uma função. Filho é 'CALL'(Chamada);
				
				node = AST_NewNode(AST_Call, tokenGetLine(tempId), AST_NodeValueFromToken(tempId));
				currParent = AST_InsertChild(currParent, node );  			
				tempId = NULL; //Pai Call
				return 0;

			}
			else if( tk == COLON ) { 
				node = AST_NewNode( AST_DeclVar, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				currParent = AST_InsertChild( currParent, node ); //Pai DeclVar
				tempId = NULL;
				return 0;
			}
			else /*if(tk == EQUAL || tk == OP_BRACKET)*/ {
				node = AST_NewNode( AST_Attr, line, NULL );
				currParent = AST_InsertChild( currParent, node ); 
				node = AST_NewNode( AST_Var, tokenGetLine(tempId), AST_NodeValueFromToken(tempId) );
				tempId = NULL;
				AST_InsertChild( currParent, node );
				if( tk != EQUAL ) {
					currParent =  node;//Muda pai pra var		
				}
				return buildAst( t, line );
			}
		} else {
			if( tk == WHILE ) {
				node = AST_NewNode(AST_While, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				node = AST_NewNode( AST_Expression, line, NULL);
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
			else if (tk == IF) {
				node = AST_NewNode(AST_If, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				node = AST_NewNode( AST_Expression, line, NULL);
				currParent = AST_InsertChild( currParent, node );
				return 0;
			}
			else if (tk == RETURN) {
				node = AST_NewNode(AST_Return, line, NULL);
				currParent = AST_InsertChild( currParent, node ); //Mas a Expressão é obrigatória. Então: 
				return 0;
			} else if(tk == LOOP ) {
				currParent = AST_GetParent(currParent); //Sai do Bloco.
				return buildAst( t, line );
			}
			else if ( tk == END )  {
				currParent = AST_GetParent(currParent); //Sai do Bloco
				return buildAst( t, line );
			} else if( tk == ELSE ) {
				currParent = AST_GetParent(currParent); //Sai do Bloco
				return buildAst( t, line );
			}
		}
	}

	if( parentType == AST_Attr ) {
		if( tk == NL ) {
			currParent = AST_GetParent(currParent);
			return 0;
		} 
		else if( tk == EQUAL ) {
			node = AST_NewNode( AST_Expression, line, NULL );
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	if( parentType == AST_Return) {
		if( tk == NL ) {
			currParent = AST_GetParent(currParent);
			return 0;
		} else {
			node = AST_NewNode( AST_Expression, line, NULL);
			currParent = AST_InsertChild( currParent, node );
			return 0;
		}
	}

	if( parentType == AST_Var ) {
		if( tk == CL_PARENTHESIS || tk == NL || tk == COMMA  || tk == EQUAL) {
			currParent = AST_GetParent(currParent);
			//parentType == AST_GetType(currParent);//Warning, but still important
			return buildAst( t, line );
		}
		else if( tk != CL_BRACKET ){
			node = AST_NewNode( AST_Expression, line, NULL );
			currParent = AST_InsertChild(currParent, node);
			return buildAst( t, line );
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
			if( tk == OP_BRACKET ) {
				bracketCount++;
				return 0;
			}
			else if( isBaseType(tk) ) {
				node = AST_NewNode(tk, line, AST_NodeValueFromInt(bracketCount));	
				AST_InsertChild(currParent, node); //Now Parent is an Type
				currParent = AST_GetParent(currParent);
				bracketCount = 0;
				return 0;
			} 
		}
	}


	if( parentType == AST_Call ) {
		if( tk == NL ) { 
			currParent = AST_GetParent(currParent);
			return 0;
		}
		else if( tk != COMMA && tk != CL_PARENTHESIS && tk != CL_BRACKET) {
			
			node = AST_NewNode(AST_Expression, line, NULL);
			currParent = AST_InsertChild(currParent, node); //Now Parent is an Expression
			buildAst( t, line );
			return  0;
		}
	}

	if( parentType == AST_ElseIf) {
		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 
		else if(tk == ELSE ) {
			currParent = AST_GetParent(currParent);
			return buildAst( t, line );
		} 
		else if( tk == END ) {
			currParent = AST_GetParent(currParent);
			return buildAst( t, line );
		}

	}

	if( parentType == AST_Else ) {
		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} else 	if( tk == IF ) {
			AST_UpdateNodeType(currParent, AST_ElseIf);
			node = AST_NewNode( AST_Expression, line, NULL);
			currParent = AST_InsertChild( currParent, node );
			return 0;
		}
		else if(tk == END ) {
			currParent = AST_GetParent(currParent);
			return buildAst( t, line );
		}
	}

	if( parentType == AST_If ) {

		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 
		else if(tk == ELSE ) {
			node = AST_NewNode(AST_Else, line, NULL);
			currParent = AST_InsertChild( currParent, node );
			return 0;
		} 
		else if(tk == END ) {
			currParent = AST_GetParent(currParent);
			return 0;
		}

	}

	if( parentType == AST_While ) {
		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		} 
		else if ( tk == LOOP ) {
			currParent = AST_GetParent(currParent);
			return 0;
		}
	}

	if( parentType == AST_ElseIf ) {
		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0 ;
		}
	}

	if( parentType == AST_Else ) {
		if(tk == NL) { //COmeça o Bloco
			node = AST_NewNode(AST_Block, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
	}

	//Constants & BinOp
	if( parentType == AST_Expression || parentType == AST_UnaryMinus || parentType == AST_Not|| isBinOp((TokenKind)parentType )) {
		if( tk == IDENTIFIER ) { //VarORCall
			tempId = t;
			return 0;
		}
		else if( tempId !=NULL ) {
			if( tk == OP_PARENTHESIS ) { //É chamada
				node = AST_NewNode( AST_Call, line, AST_NodeValueFromToken( tempId ) );
				tempId = NULL;
				currParent = AST_InsertChild(currParent, node);
				
			}
			else {

				node = AST_NewNode( AST_Var, line, AST_NodeValueFromToken( tempId ) );
				tempId = NULL;
				AST_InsertChild(currParent, node);
				
				if( tk == OP_BRACKET ) {
					currParent = node;
					return buildAst( t, line );
				} else {
					return buildAst( t, line );
				}

				while(parentType == AST_Not || parentType == AST_UnaryMinus) {
					currParent = AST_GetParent( currParent );
					parentType = AST_GetType( currParent );
				}
				
			}
			return 0;
		}
		else if( tk == NEW ) {
			node = AST_NewNode(AST_New, line, NULL);
			currParent = AST_InsertChild(currParent, node);
			return 0;
		}
		else if( isConstant(tk) ) { //Constant
			node = AST_NewNode((ASTNodeType)tk, line, AST_NodeValueFromToken(t));
			node = AST_InsertChild(currParent, node);
			while(parentType == AST_Not || parentType == AST_UnaryMinus) {
				currParent = AST_GetParent( currParent );
				parentType = AST_GetType( currParent );
			}
			return 0;
		}
		else if( tk == NOT ) {
			node = AST_NewNode(AST_Not, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
			return 0;
		}
		else if( tk == MINUS && ( AST_GetFirstChild(currParent) == NULL || (isBinOp((TokenKind)parentType) && AST_GetFirstChild(currParent) == AST_GetLastChild(currParent) )  ) ) { //É unário ou binário??? Resolvendo.
			node = AST_NewNode(AST_UnaryMinus, line, NULL);
			currParent = AST_InsertChild(currParent, node);	
			return 0;
		}
		else if( isBinOp(tk) ) {
			if(isBinOp((TokenKind)parentType)) { //Pai também é bin Op
				if( ! isbinOpPriorityGreater((TokenKind)parentType, tk) ) {
					node = AST_GetLastChild(currParent);
					node = AST_RemoveChild(currParent, node);
					currParent = AST_InsertChild( currParent, AST_NewNode((ASTNodeType)tk, line, NULL) );
					node = AST_InsertChild( currParent, node );
					parentType = AST_GetType(currParent);
					return 0;
				}
				else {
					node = AST_GetParent(currParent); //vovo
					currParent = AST_RemoveChild(node, currParent);
					node = AST_InsertChild( node, AST_NewNode((ASTNodeType)tk, line, NULL) );
					AST_InsertChild( node, currParent );
					currParent = node;
					return 0;
				}
				return 0;
			}
			node = AST_NewNode((ASTNodeType)tk, line, NULL);
			currParent = AST_InsertNewChildParentOfChildren(currParent, node); // Abaixa a rapaziada
			return 0;
		} else {
			if( tk == NL || tk == CL_BRACKET || tk == COMMA ) {
				do {
					currParent = AST_GetParent(currParent);
					parentType = AST_GetType(currParent);
				} while( parentType == AST_Expression || parentType == AST_UnaryMinus || parentType == AST_Not|| isBinOp((TokenKind)parentType ));
				return buildAst( t, line );
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

		}
		if( input != stdin )
			fclose(input);
	}
	else {
		printf("\nError: Cannot open file.\n");
		return 1;
	}
	if(ret == 0 ) {
		printf("Correct Syntax!!!\n");
		AST_PrettyPrint(root, 1);
	}


	return ret;
}