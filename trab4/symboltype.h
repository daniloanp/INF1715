#ifndef SYM_H
#define SYM_H
#include <stdbool.h>
typedef enum {
   SYM_VOID = 0,
   SYM_INT = 11,
   SYM_CHAR = 12,
   SYM_BOOL = 13,
   SYM_FUNVOID = 1,
   SYM_FUN_INT = 110,
   SYM_FUN_CHAR = 120,
   SYM_FUN_BOOL = 130
} SymbolBaseType;

typedef struct {
	SymbolBaseType type;
	unsigned int dimension;
} SymbolType;


SymbolType SYM_NewSymbolType( SymbolBaseType base, unsigned int dimension );
bool SYM_IsFunctionType( SymbolType tp );
bool SYM_CompatibleTypes( SymbolType t1, SymbolType t2);
#endif