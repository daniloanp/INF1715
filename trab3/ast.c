#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

union ast_val {
	bool boolean;
	unsigned long number;
	char* string;
};

struct struct_AST {
   ASTNodeType type;
   unsigned int line;
   ASTNodeValue value;
   SymbolType symbolType;
   AST firstChild;
   AST lastChild;
   AST parent;
   AST nextSibling;
   AST prevSibling;
};

unsigned int AST_GetLine( AST node ) {
	assert(node);
	return node->line;
}


AST AST_New( ASTNodeType type , int line ) {
	AST node  = (AST)malloc(sizeof(struct struct_AST));
	node->type = type;
	node->line = line;
	node->value.string = NULL;
	node->parent = NULL;
	node->lastChild = NULL;
	node->firstChild = NULL;
	node->nextSibling = NULL;
	node->prevSibling = NULL;
	return node;
}
AST AST_NewAsBool( ASTNodeType type , int line, bool val ) {
	AST node = AST_New( type, line );
	node->value.boolean = val;
	return node;
}
AST AST_NewAsString( ASTNodeType type , int line, char* val ) {
	AST node = AST_New( type, line );
	node->value.string = val;
	return node;
}
AST AST_NewAsInt( ASTNodeType type , int line, unsigned long val ) { 
	AST node = AST_New( type, line );
	node->value.number = val;
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


AST AST_UpdateNodeValue( AST node, ASTNodeValue val ) {
	if(node==NULL)
		return node;
	node->value = val;
	return node;
}

AST AST_UpdateNodeType( AST node, ASTNodeType tp ) {
	assert( node );
	node->type = tp;
	return node;
}


ASTNodeValue AST_GetNodeValue(AST node) {
	assert( node );
		
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
	return node->value.string;
}

unsigned long AST_GetIntValue(AST node) {
	assert(node != NULL);
	return node->value.number;
}


static char buff[1000];

static char* AST_nodeToString(AST node) {
	ASTNodeType tp = AST_GetType(node);
	switch( tp ) {
		case AST_IF: return "If";break;
		case AST_ELSE: return "Else";break;
		case AST_WHILE: return "While";break;
		case AST_RETURN: return "Return";break;
		case AST_NEW: return "New";break;
		case AST_INT: 
			sprintf(buff, "tp_int(%lu)", AST_GetIntValue(node));
			return buff;
		break;
		case AST_CHAR: 
			sprintf(buff, "tp_char(%lu)", AST_GetIntValue(node));
			return buff;
		break;
		case AST_BOOL: 
			sprintf(buff, "tp_bool(%lu)", AST_GetIntValue(node));
			return buff;
		break;
		case AST_STRING:
			sprintf(buff, "tp_string(%s)", AST_GetIntValue(node));
			return buff;
		break;
		case AST_AND: return "And";break;
		case AST_OR: return "Or";break;
		case AST_NOT: return "Not";break;
		case AST_BOOL_VAL:
			if( AST_GetIntValue(node) ) {
				return "bool: True";
			}
			else {
				return "bool: False";
			}	
		break;
		case AST_INT_VAL: 
			sprintf(buff, "int: '(%lu)'", AST_GetIntValue(node));
			return buff;
		break;
		case AST_STRING_VAL: 
			sprintf(buff, "strVal: \"%s\"", AST_GetStringValue(node));
			return buff;
		break;
		case AST_PLUS: return "+";break;
		case AST_MINUS: return "-";break;
		case AST_MUL: return "*";break;
		case AST_DIV: return "/";break;
		case AST_GREATER: return ">";break;
		case AST_GREATER_EQUAL: return ">=";break;
		case AST_LESS: return "<";break;
		case AST_LESS_EQUAL: return "<=";break;
		case AST_EQUAL: return "=";break;
		case AST_DIFFERENT: return "<>";break;
		case AST_DECL_FUNCTION: 
			sprintf(buff, "fun '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_PARAM: 
			sprintf(buff, "Param '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_PARAM_LIST: return "ParamList";break;
		case AST_EXPRESSION: return "Exp";break;
		case AST_CALL: 
			sprintf(buff, "Call '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_ATTR: return "Attr";break;
		case AST_BLOCK: return "Block";break;
		case AST_PROGRAM: return "Program";break;
		case AST_DECL_VAR:
			sprintf(buff, "DeclVar '(%s)'", AST_GetStringValue(node));
			return buff;
	 	break;
		case AST_VAR: 
			sprintf(buff, "Var '(%s)'", AST_GetStringValue(node));
			return buff;
		break;
		case AST_UNARYMINUS: return "(-)";break;
		case AST_ELSE_IF: return "ElseIf";break;
		default: break;
	}
	return NULL;
}

bool AST_IsNodeAConstant( AST node ) {
	ASTNodeType tk = node->type;
	return (tk == AST_INT_VAL || tk == AST_STRING_VAL || tk == AST_BOOL_VAL );
}

bool AST_IsNodeABinOp( AST node ) {
	ASTNodeType tk = node->type;
	return( tk==AST_PLUS || tk == AST_MINUS || tk == AST_MUL || tk == AST_DIV || tk == AST_AND || tk == AST_OR || tk == AST_GREATER || tk == AST_GREATER_EQUAL || tk == AST_LESS || tk == AST_LESS_EQUAL || tk == AST_EQUAL || tk == AST_DIFFERENT );
}

bool AST_IsBaseNodeType( AST node ) {
	ASTNodeType tk = node->type;
	return (tk == AST_CHAR || tk == AST_INT || tk == AST_BOOL);
}


void AST_PrettyPrint( AST t, int level ) {
	AST c; int i,j;
	if(t == NULL) {
		return;
	}

	for( i=level; i; i-- ) {
		printf("   ");
	}

	printf("%s",AST_nodeToString(t));
	/*//if( t->symbolType.type ) {
		j = t->symbolType.type;
		printf( "  TYPE: ");
		if( j == SYM_INT)
			printf("INT");
		else if( j == SYM_BOOL) {->
			printf("BOOL");
		}
		else if( j == SYM_CHAR) {
			printf("CHAR");
		}
		else if( j == SYM_FUN_BOOL) {
			printf("FUN BOOL");
		}
		else if( j == SYM_FUN_CHAR) {
			printf("FUN CHAR");
		}
		else if( j == SYM_FUN_INT) {
			printf("FUN INT");
		}
		else if( j == SYM_FUN_VOID) {
			printf("FUN VOID");
		}
		else 
			printf("VOID");
		i = t->symbolType.dimension;
		for(i;i;i--) printf("[]");
	//}*/
	
	printf("\n");
	c = AST_GetFirstChild(t);

	while( c != NULL ) {
		AST_PrettyPrint( c, level+1 );
		c = AST_GetNextSibling(c);
	}
};


SymbolType AST_GetSymType( AST node ) {
	return node->symbolType;
}

void AST_SetSymType( AST node, SymbolType tp ) {
	node->symbolType = tp;
}

bool AST_IsOperatorNode( AST node ) {
	switch( AST_GetType(node) ) {
		case AST_PLUS: case  AST_MINUS : case  AST_MUL : case  AST_DIV : case  AST_GREATER : case  AST_GREATER_EQUAL : case  AST_LESS: case  AST_LESS_EQUAL: case  	AST_EQUAL : case  AST_DIFFERENT : case  AST_UNARYMINUS: case AST_AND: case AST_OR:
		return true;
	default:
		return false;
	break;
	}
}

void AST_Free( AST t ) {
	AST node, next;
	if( t == NULL)  {
		return;
	}
	node = t->firstChild;
	while( node ) {
		next = node->nextSibling;
		AST_Free( node );
		node = next;
	}

	
	if( t->value.string != NULL && t->type  == AST_VAR || t->type ==  AST_STRING_VAL || t->type == AST_CALL || t->type == AST_DECL_FUNCTION || t->type == AST_DECL_VAR || t->type == AST_PARAM ) {
			free(t->value.string);			
	}
	free(t);
}