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

   AST* firstChild;
   AST* lastChild;
   AST* parent;
   AST* nextSibling;
   AST* prevSibling;
};

AST AST_NewNode( ASTNodeType type , int line, ASTNodeValue value ) {
	AST node  = (AST)malloc(sizeof(struct_AST));
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


int AST_NewNodeFromToken( Token t ) {
	ASTNodeValue value = t->value;
	if( t->kind >= 1 && t->kind <= 36)
		ASTNodeType type = (ASTNodeType) t->kind;
	else
		assert(0);//problema
	AST node  = AST_NewNode(type, t->line, value );
	return node;;
}

AST AST_insertChild( AST parent, AST child ) {
	AST sibling = parent->lastChild;
	if(lastChild!=NULL) {
		lastChild->nextSibling = child;
	}
	else {
		parent->firstChild = child;
		child->prevSibling = lastChild;
	}
	child->parent = parent;
	parent->lastChild = child;
	return parent;


}

int AST_insertSibling( AST sibling, AST newSibling ) {
	if(newSibling != NULL) {
		sibling->nextSibling = newSibling;
		newSibling->prevSibling = sibling;
		newSibling->parent = sibling->parent;
		parent->lastChild = newSibling;
	}
	return sibling;
}



int AST_insertParentLowering( AST curr, AST newParent ) {
	AST parent = curr->parent;
	curr->parent = newParent;
}


ASTNodeType AST_getType(AST node) {
	if(!node)
		return (ASTNodeType)0;
	return node->type;
}






