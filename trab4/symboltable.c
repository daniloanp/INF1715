#ifndef SYMBOLTABLE_C
#define SYMBOLTABLE_C
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symboltable.h"
#include <assert.h>
#include "../trab3/AbstractSyntaxTree.h"
static unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return ( hash % HASH_SIZE ); //changed
}


SymbolTable* SymbolTable_new( SymbolTable* parent ) {
	SymbolTable* st = ( SymbolTable* )malloc( sizeof( SymbolTable ) );
	if( !st ) {
		printf("Malloc has failed at SymbolTable_new!");
		return NULL;
	}
	st->parent = parent;
	memset ( st->symbols, 0, sizeof(Symbol*) * HASH_SIZE );
	return st;
}

bool SymbolTable_add(SymbolTable* st, const char* name, SymbolType type, AST node) {
	Symbol* sym = NULL; 
	Symbol* prevSym = NULL; 
	unsigned long i;

	assert(st);
	i = hash((unsigned char*)name);
	
	for( sym = st->symbols[i]; sym ; sym = sym->next ) {
		
		if( !strcmp(sym->name, name)) {
			printf ("Error: Symbol already declared for his scope");
			return false; 
		}

		prevSym = sym;
	}
	sym  = ( Symbol* )malloc( sizeof(Symbol) );
	if( !sym ) {
		printf("Malloc has failed at SymbolTable_new!");
		return NULL;
	}
	
	if( prevSym != NULL ) {
		prevSym->next = sym;
	} else {
		 st->symbols[i] = sym;
	}
	


	sym->name =(char*) name; //dangerous;
	sym->type = type; //dangerous;
	sym->node = node;
	sym->line = AST_GetLine(node);	
	return true;
}

 Symbol* SymbolTable_get(SymbolTable* st, const char* name) {
	unsigned long i;
	Symbol* sym; 
	assert(st);
	i = hash((unsigned char*)name);
	
	for( sym = st->symbols[i]; sym ; sym = sym->next ) {
		if( !strcmp(sym->name, name)) {
			return sym;
		}
	}

	if ( st->parent ) {
		return SymbolTable_get(st->parent, name );	
	}

	return NULL;
}

Symbol* SymbolTable_getInScope ( SymbolTable* st, const char* name ) {
	unsigned long i;
	Symbol* sym;
	i = hash((unsigned char*)name);

	for( sym = st->symbols[i]; sym ; sym = sym->next ) {
		if( !strcmp(sym->name, name)) {
			return sym;
		}
	}

	return NULL;
}


void SymbolTable_delete( SymbolTable* st ) {
	Symbol* sym; 
	Symbol* prevSym;
	int i;
	for( i = 0; i < HASH_SIZE; i++ ) {
		sym = st->symbols[i];
		while(  sym  ) { 
			prevSym = sym;
			sym = sym->next;
			free(prevSym);
		}
	}
	free(st);
}




#endif