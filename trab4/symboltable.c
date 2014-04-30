#ifndef SYMBOLTABLE_C
#define SYMBOLTABLE_C
#include <string.h>
#include "symboltable.h"


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
	memset ( st->symbols, NULL, sizeof(Symbol) * HASH_SIZE );
	return st;
}

bool SymbolTable_add(SymbolTable* st, const char* name, SymbolType type, int line) {
	Symbol* sym; 
	Symbol* prevSym; 
	unsigned long i;

	assert(st);

	i = hash(name);

	for( sym = st->symbols[i]; sym ; sym = sym->next ) {
		if( !strcmp(sym->name, name)) {
			printf ("Error: Symbol already declared for his scope");
			return false; 
		}

		prevSym = sym;
	}
	sym = prevSym->next = ( Symbol* )malloc( sizeof(Symbol) );
	if( !sym ) {
		printf("Malloc has failed at SymbolTable_new!");
		return NULL;
	}


	sym->name = name; //dangerous;
	sym->type = type; //dangerous;
	sym->line = line;	
	return true;
}

Symbol* SymbolTable_get(SymbolTable* st, const char* name) {
	unsigned long i;
	Symbol* sym; 
	assert(st);
	i = hash(name);
	
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


void SymbolTable_delete(SymbolTable* st) {
	Symbol* sym; 
	Symbol* prevSym;
	for( int i = 0; i < HASH_SIZE; i++ ) {
		sym = st->symbols[i];
		while(  sym ; ) { 
			prevSym = sym;
			sym = sym->next;
			free(prevSym);
		}
	}
	free(st);
}



#endif