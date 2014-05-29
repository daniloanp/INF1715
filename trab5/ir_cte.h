#ifndef IR_CTE_H
#define IR_CTE_H
#include <stdio.h>

typedef enum {
	ENDR_WORD,
	ENDR_BYTE,
	ENDR_STR,
	ENDR_VAR,
	ENDR_TEMP,
	ENDR_LABEL,
	ENDR_RETEXP,
	ENDR_FUNC,
	ENDR_CONST
} EndrType;

typedef struct endr_ Endr; //passed by copy!

struct endr_ {
	EndrType type;
	unsigned long val;
	char *str;//caso seja uma VAR
};

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

struct _cte {
	Instr cmd;
	Endr args[3];
	CTE next;
};


CTE CTE_New( Instr in, Endr* args );
Endr Endr_New( EndrType tp, unsigned long val );
Endr Endr_NewAsString( EndrType tp, char* s );
void CTE_Print( CTE cmds, FILE* f );
#endif