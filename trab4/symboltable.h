#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdbool.h>
#include "../trab3/ast.h"
#include "symboltype.h"


#define HASH_SIZE 256

typedef struct Symbol_ Symbol;

struct Symbol_ {
   char* name;
   SymbolType type;
   int line;
   AST node;
   Symbol* next;
};


typedef struct SymbolTable_ SymbolTable;

struct SymbolTable_ {
	SymbolTable* parent;
	Symbol* symbols[HASH_SIZE];
};


SymbolTable* SymbolTable_new( SymbolTable* parent );

void SymbolTable_delete(SymbolTable* st);
bool SymbolTable_add( SymbolTable* st, const char* name, SymbolType type, AST node );

Symbol* SymbolTable_getInScope(SymbolTable* st, const char* name);
Symbol* SymbolTable_get(SymbolTable* st, const char* name);



#endif 
