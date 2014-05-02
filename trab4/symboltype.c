#ifndef SYM_C
#define SYM_C
#include "symboltype.h"
#include <stdio.h>
SymbolType SYM_NewSymbolType( SymbolBaseType base, unsigned int dimension ) {
	SymbolType tp;
	tp.type = base;
	tp.dimension = dimension;
	return (tp);
}

bool SYM_IsFunctionType( SymbolType tp ) {
	return !(tp.type ==SYM_INT || tp.type ==SYM_BOOL || SYM_CHAR || SYM_VOID ) ;
}

bool SYM_CompatibleTypes( SymbolType t1, SymbolType t2) {
	if ( t1.dimension != t2.dimension ) {
		return false;
	} //Tem mesma dimensao
	if( t1.dimension == 0 && (t1.type==SYM_CHAR || t1.type == SYM_INT) && (t2.type==SYM_CHAR || t2.type == SYM_INT) ) {
		return true;
	}

	if( t1.type == t2.type  ) {
		return true;
	}

	return false;
}


#endif