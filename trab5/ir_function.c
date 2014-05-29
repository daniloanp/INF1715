#ifndef IR_FUNCTION_C
#define IR_FUNCTION_C
#include "ir_function.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>


struct _args {
	char * name;
	Args next;	
};

Function Function_New( void ) {
	Function func;
	func = (Function) malloc( sizeof( struct _function ) );
	func->args = NULL;
	func->commands = NULL;
	func->next = NULL;
	return func;
}


void Function_AddArg( Function func, char* s ) {
	Args l, i;
	l = (Args) malloc( sizeof( struct _args ) );
	l->next = NULL;
	l->name = (char*) malloc( sizeof( strlen( s ) +1 ) );
	memcpy( l->name, s, strlen( s ) +1 );

	if( !func->args ) {
		func->args = l;
	}
	else {
		i = func->args;
		while( i->next ) {
			i = i->next;
		}
		i->next = l;
	}
	
}

void Function_AddCTE( Function func, CTE cte) {
	CTE commands = func->commands;
	if( ! commands )  {
		func->commands = cte;
	}
	else {
		while( commands->next ) {
			commands = commands->next;
		}
		commands->next = cte;
	}
}



#endif