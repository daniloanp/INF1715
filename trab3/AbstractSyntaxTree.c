#include "AbstractSyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>

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
	if(sibling!=NULL) {
		sibling->nextSibling = child;
	}
	else {
		parent->firstChild = child;
		child->prevSibling = sibling;
	}
	child->parent = parent;
	parent->lastChild = child;
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


ASTNodeValue AST_GetNodeValue(AST node) {
	if(node == NULL)
		return NULL;
	return node->value;
}





