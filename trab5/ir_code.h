#ifndef IR_CODE_H
#define IR_CODE_H
#include "ir_function.h"
#include "ir_cte.h"
#include <string.h>
#include <stdbool.h>
typedef struct _ir_code* IRCode;
typedef struct _strings* StringList;
struct _ir_code {
	StringList strings; //Lista de Declarações, Simples
	CTE globals; //Lista de Globais, Complexo.
	Function functions; // Lista de Programs
};

IRCode IRCode_New( );
void IRCode_AddString( IRCode code, Endr label, char* s );
bool IRCode_FindString( IRCode code, char* s, Endr *e );
void IRCode_AddGlobal( IRCode code, CTE cte );
void IRCode_AddFunction( IRCode code, Function func );
void IRCode_DumpToFile( IRCode code, FILE * f);

#endif