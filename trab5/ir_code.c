#ifndef IR_CODE_C
#define IR_CODE_C
#include "ir_code.h"

struct _ir_code {
	List strings; //Lista de Declarações, Simples
	List globals; //Lista de Globais, Complexo.
	Function functions; // Lista de Programs
};

IRCode _code;
Function _func;


IRCode IRCode_New( ) {
	IRCode ir = (IRCode) malloc(sizeof (struct ir_code) );
	ir->strings = NULL;
	ir->globals = NULL;
	ir->functions = NULL
	return ir;

}
//void args( )
