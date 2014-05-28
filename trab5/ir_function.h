#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H
#include "list.h"
#include "ir_cte.h"

typedef struct _function* Function;

struct _function {
	char *id;
	List args;//Endrs
	CTE commands;
	struct _function* next;
};


#endif