#ifndef IR_CODE_C
#define IR_CODE_C

struct _ir_code {
	List strings; //Lista de Declarações, Simples
	List globals; //Lista de Globais, Complexo.
	Function functions; // Lista de Programs
};

IRCode _code;
Function _func;

//void args( )
