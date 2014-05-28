#ifndef IR_CODE_C
#define IR_CODE_C
#include "ir_code.h"
#include "malloc.h"


IRCode IRCode_New( ) {
	IRCode ir = (IRCode) malloc(sizeof (struct _ir_code) );
	ir->strings = NULL;
	ir->globals = NULL;
	ir->functions = NULL;
	return ir;
}
//void args( )

#endif
