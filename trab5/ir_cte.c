#ifndef IR_CTE_C
#define IR_CTE_C

typedef enum {
	ENDR_WORD,
	ENDR_BYTE,
	ENDR_STR,
	ENDR_VAR,
	ENDR_TEMP,
	ENDR_LABEL,
	ENDR_RETEXP,
	ENDR_FUNC
}

typedef endr_ ENDR; //passed by copy!

struct endr_ {
	EndrType type;
	unsigned long val;
	char *str;//caso seja uma VAR
} Endr;

typedef enum {
	ATTR_SIMPLE,
	ATTR_BYTE,
	ATTR_ADD,
	ATTR_MIN,
	ATTR_DIV,
	ATTR_MUL,
	ATTR_GTE,
	ATTR_GT,
	ATTR_LTE,
	ATTR_LE,
	ATTR_EQ,
	ATTR_NE,
	ATTR_NOT,
	ATTR_UN_MINUS,
	ATTR_NEW,
	ATTR_NEW_BYTE,
	ATTR_FROM_ARR,
	ATTR_BYTE_FROM_ARR,
	ATTR_TO_ARR,
	ATTR_BYTE_TO_ARR,
	GOTO_IF,
	GOTO_IF_FALSE,
	GOTO,
	CALL,
	PARAM,
	RET_VAL,
	RET,
	LABEL
} Instr;

typedef struct _cte* CTE;

struct cte_ {
	Instr cmd;
	Endr args[3];
	CTE next;
};

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
		case GOTO,
		case CALL,
		case PARAM,
		case RET_VAL,
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
	Endr endr = (Endr)malloc( sizeof( struct endr_ ));
	endr->type = tp;
	endr->val = 0;
	endr->str = s;
	return endr;
}

CTE CTE_New( Instr in, Endr enr[3] ) {
	CTE cte = (CTE)malloc( sizeof( struct cte_ ));
	cte->next = NULL;
	cte->cmd = in;
	handleCmd( cte, endr );
	return cte;
}






