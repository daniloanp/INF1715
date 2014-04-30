
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdbool.h>


#define HASH_SIZE 256;

typedef enum {
   SYM_INT,
   SYM_CHAR,
   SYM_BOOL
} SymbolBaseType;

typedef enum {
	SymbolBaseType type;
	unsigned int dimension;
} SymbolType;

typedef struct Symbol_ Symbol;

struct Symbol_ {
   char* name;
   SymbolType type;
   int line;
   Symbol* next;
};


typedef struct SymbolTable_ SymbolTable;

struct SymbolTable_ {
	SymbolTable* parent;
	Symbol* symbols[HASH_SIZE];
};

SymbolTable* SymbolTable_new( SymbolTable* parent );

void SymbolTable_delete(SymbolTable* st);
void SymbolTable_add(SymbolTable* st, const char* name, SymbolType type, int line);

Symbol* SymbolTable_get(SymbolTable* st, const char* name);




#endif 
