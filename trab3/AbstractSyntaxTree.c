#include "AbstractSyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

union ast_val {
	unsigned long number;
	char* string;
};

struct struct_AST {
   ASTNodeType type;
   int line;
   ASTNodeValue value;

   AST firstChild;
   AST lastChild;
   AST parent;
   AST nextSibling;
   AST prevSibling;
};

AST AST_NewNode( ASTNodeType type , int line, ASTNodeValue value ) {
	AST node  = (AST)malloc(sizeof(struct struct_AST));
	node->type = type;
	node->line = line;
	node->value = value;
	node->parent = NULL;
	node->lastChild = NULL;
	node->firstChild = NULL;
	node->nextSibling = NULL;
	node->prevSibling = NULL;
	return node;
}


AST AST_InsertChild( AST parent, AST child ) {

	AST sibling = parent->lastChild;
	child->parent = parent;
	parent->lastChild = child;
	if(sibling!=NULL) {
		sibling->nextSibling = child;
	}
	else {
		parent->firstChild = child;
	}
	child->parent = parent;
	child->prevSibling = sibling;
	
	return child;


}

AST AST_InsertSibling( AST sibling, AST newSibling ) {
	AST parent = sibling->parent;
	if(newSibling != NULL) {
		sibling = sibling->parent->lastChild;
		/*while(sibling->newSibling != NULL) {
			sibling = sibling->nextSibling;
		}*/

		sibling->nextSibling = newSibling;
		newSibling->prevSibling = sibling;
		newSibling->parent = sibling->parent;
		parent->lastChild = newSibling;
	}
	return newSibling;
}



AST AST_InsertNewChildParentOfChildren( AST curr, AST newParent ) {
	AST node;
	for( node = curr->firstChild; node; node = node->nextSibling) {
		AST_InsertChild(newParent, node);
	}
	curr->firstChild = curr->lastChild = newParent;
	newParent->parent = curr;
	return newParent;
}


ASTNodeType AST_GetType(AST node) {
	if(!node)
		return (ASTNodeType)0;
	return node->type;
}


AST AST_GetParent(AST node) {
	return node->parent;
}

AST AST_GetFirstChild(AST node) {
	return node->firstChild;
}

AST AST_GetLastChild(AST node) {
	return node->lastChild;
}

AST AST_GetNextSibling(AST node) {
	return node->nextSibling;
}

AST AST_GetPrevSibling(AST node) {
	return node->prevSibling;
}

ASTNodeValue AST_NodeValueFromToken( Token t ) {
	//TokenValue val = NULL;
	ASTNodeValue nodeVal = NULL;
	if (t!=NULL && tokenGetVal( t )!=NULL) {
		nodeVal = (ASTNodeValue)malloc(sizeof(union ast_val));
		if(nodeVal == NULL) {
			printf("Malloc problem");
			return NULL;
		}
		if( tokenGetKind( t ) == IDENTIFIER ||  tokenGetKind( t ) == STRING_VAL ) {
			nodeVal->string = tokenGetStringValue(t);
		}
		else {
			nodeVal->number = tokenGetNumberValue(t);
		}
		return nodeVal;
	}
	return NULL;
}

ASTNodeValue AST_NodeValueFromInt( unsigned int v ) {
	ASTNodeValue nodeVal = NULL;
	nodeVal = (ASTNodeValue)malloc(sizeof(union ast_val));
	if(nodeVal == NULL) {
		printf("Malloc problem");
	} 
	else {
		nodeVal->number = v;
		return nodeVal;
	}

	return NULL;
}

AST AST_UpdateNodeValue( AST node, ASTNodeValue val ) {
	if(node==NULL)
		return node;
	node->value = val;
	return node;
}

AST AST_UpdateNodeType( AST node, ASTNodeType tp ) {
	if(node==NULL)
		return node;
	node->type = tp;
	return node;
}


ASTNodeValue AST_GetNodeValue(AST node) {
	if(node == NULL)
		return NULL;
	return node->value;
}


AST AST_RemoveChild( AST parent, AST child ) {
	AST c ;
	
	if(parent == NULL || child == NULL) {
		return NULL;
	}
	
	

	if( child == parent->firstChild ) {
		parent->firstChild = child->nextSibling; 
		//Não dá pra achar mais.


		if( child == parent->lastChild ) { //Tbm é ultimo.
			parent->lastChild = NULL;
		} else
		{
			child->prevSibling = NULL;
		}
	} 
	else if( child == parent->lastChild ) {
		child->prevSibling->nextSibling == NULL;
		parent->lastChild = child->prevSibling;
	}
	else { //Busca
		c = parent->firstChild;
		do {
			if( c == child) {
				child->nextSibling->prevSibling = child->prevSibling;
				child->prevSibling->nextSibling = child->nextSibling;
				break;
			}
			c = c->nextSibling;
		} while ( c != NULL);
	}

		

	child->parent = NULL;
	child->prevSibling = NULL;
	child->nextSibling = NULL;

	return child;
}

char* AST_GetStringValue(AST node) {
	if(node == NULL)  return NULL;
	if( node->value == NULL) return NULL;
	return node->value->string;
}

unsigned long AST_GetNumberValue(AST node) {
	assert(node != NULL);
	assert( node->value != NULL);
	return node->value->number;
}


static char buff[1000];

static char* AST_nodeToString(AST node) {
	ASTNodeType tp = AST_GetType(node);
	switch( tp ) {
		case AST_If: return "If";break;
		case AST_Else: return "Else";break;
		case AST_While: return "While";break;
		case AST_Return: return "Return";break;
		case AST_New: return "New";break;
		case AST_Int: 
			sprintf(buff, "tp_int(%d)", AST_GetNumberValue(node));
			return buff;
		break;
		case AST_Char: 
			sprintf(buff, "tp_char(%d)", AST_GetNumberValue(node));
			return buff;
		break;
		case AST_Bool: 
			sprintf(buff, "tp_bool(%d)", AST_GetNumberValue(node));
			return buff;
		break;
		case AST_String:
			sprintf(buff, "tp_string(%d)", AST_GetNumberValue(node));
			return buff;
		break;
		case AST_And: return "And";break;
		case AST_Or: return "Or";break;
		case AST_Not: return "Not";break;
		case AST_BoolVal:
			if(AST_GetNumberValue(node))
				return "bool: True";
			else
				return "bool: False";
		break;
		case AST_IntVal: 
			sprintf(buff, "int: '(%d)'", AST_GetNumberValue(node));
			return buff;
		break;
		case AST_StringVal: 
			sprintf(buff, "strVal: '%s'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_Plus: return "+";break;
		case AST_Minus: return "-";break;
		case AST_Mul: return "*";break;
		case AST_Div: return "/";break;
		case AST_Greater: return ">";break;
		case AST_GreaterEqual: return ">=";break;
		case AST_Less: return "<";break;
		case AST_LessEqual: return "<=";break;
		case AST_Equal: return "=";break;
		case AST_Different: return "<>";break;
		case AST_DeclFunction: 
			sprintf(buff, "fun '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_Param: return "Param";break;
		case AST_ParamList: return "ParamList";break;
		case AST_Expression: return "Exp";break;
		case AST_Call: 
			sprintf(buff, "Call '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_Attr: return "Attr";break;
		case AST_Block: return "Block";break;
		case AST_Program: return "Program";break;
		case AST_DeclVar:
			sprintf(buff, "DeclVar '(%s)'", AST_GetStringValue(node));
			return buff;
	 	break;
		case AST_Var: 
			sprintf(buff, "Var '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_UnaryMinus: return "(-)";break;
		case AST_ElseIf: return "ElseIf";break;
		default: break;
	}
	return NULL;
}



void AST_PrettyPrint( AST t, int level ) {
	AST c; int i;
	if(t == NULL)
		return;
	for( i=level;i;i-- ) printf("   ");
	printf(AST_nodeToString(t));
	printf("\n");
	c = AST_GetFirstChild(t);
	while( c != NULL ) {
		AST_PrettyPrint( c, level+1 );
		c = AST_GetNextSibling(c);
	}
	
	//Imprimir filhos
	

};