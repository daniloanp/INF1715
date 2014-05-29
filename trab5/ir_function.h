#ifndef IR_FUNCTION_H
#define IR_FUNCTION_H
#include "ir_cte.h"
#include <stdio.h>

typedef struct _function* Function;
typedef struct _args* Args;
struct _function {
	char *id;
	Args args;
	CTE commands;
	struct _function* next;
};

Function Function_New( char *name );

void Function_AddArg( Function func, char* s );
void Function_AddCTE( Function func, CTE cte );
CTE Function_GetLastCTE( Function func );

void Function_DumpToFile( Function func, FILE * f);

#endif