#ifndef IR_CTE_C
#define IR_CTE_C
#include "ir_cte.h"
#include <malloc.h>
#include <assert.h>

void handleCmd( CTE cte, Endr* endr ) {
	assert( cte );
	switch ( cte->cmd ) {
		case ATTR_ADD:
		case ATTR_MIN:
		case ATTR_DIV:
		case ATTR_MUL:
		case ATTR_GTE:
		case ATTR_GT:
		case ATTR_LTE:
		case ATTR_LE:
		case ATTR_EQ:
		case ATTR_NE:
		case ATTR_FROM_ARR:
		case ATTR_BYTE_FROM_ARR:
		case ATTR_TO_ARR:
		case ATTR_BYTE_TO_ARR:
			//assert( endr[2] );
			cte->args[2] = endr[2];
		//SEM BREAK!!!;
		case ATTR_SIMPLE:
		case ATTR_BYTE:
		case ATTR_NOT:
		case ATTR_UN_MINUS:
		case ATTR_NEW:
		case ATTR_NEW_BYTE:
		case GOTO_IF:
		case GOTO_IF_FALSE:
			//assert( endr[1] );
			cte->args[1] = endr[1];
		// DE NOVO, no break;
		case GOTO:
		case CALL:
		case PARAM:
		case RET_VAL:
			//assert( endr[0] );
			cte->args[0] = endr[0];
		default: break;
	}
}

Endr Endr_New( EndrType tp, unsigned long val ) {
	Endr endr;// = (Endr)malloc( sizeof( struct endr_ ));
	endr.type = tp;
	endr.val = val;
	endr.str = 0;
	return endr;
}


Endr Endr_NewAsString( EndrType tp, char* s ) {
	Endr endr;
	endr.type = tp;
	endr.val = 0;
	endr.str = s;
	return endr;
}

CTE CTE_New( Instr in, Endr* endr ) {
	CTE cte = (CTE)malloc( sizeof( struct _cte ));
	cte->next = NULL;
	cte->cmd = in;
	handleCmd( cte, endr );
	return cte;
}



#endif


