#ifndef IR_CODE_H
#define IR_CODE_H
#include "list.h"
#include "ir_function.h"

typedef struct _ir_code* IRCode;

struct _ir_code {
	List strings; //Lista de Declarações, Simples
	List globals; //Lista de Globais, Complexo.
	Function functions; // Lista de Programs
};

IRCode IRCode_New( );
Endr Endr_New( EndrType tp, unsigned long val );
Endr Endr_NewAsString( EndrType tp, char* s );
#endif