#ifndef IR_CODE_C
#define IR_CODE_C
#include "ir_code.h"
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>


IRCode IRCode_New( ) {
	IRCode ir = (IRCode) malloc(sizeof (struct _ir_code) );
	ir->strings = NULL;
	ir->globals = NULL;
	ir->functions = NULL;
	return ir;
}


struct _strings {
	Endr label;
	char* str;
	struct _strings* next;
};

void IRCode_AddString( IRCode code, Endr label, char* s ) {
	
	StringList l = code->strings;
	StringList string = malloc( sizeof( struct _strings ) );
	string->str = malloc( sizeof(char)*(strlen(s)+1));
	string->next = NULL;
	string->label = label;
	memcpy( string->str, s, strlen( s )+1 );
	if ( !l ) {
		code->strings =  string;
	} 
	else {
		while ( l && l->next ) {
			l = l->next;
		}
		l->next = string;
	}
}

bool IRCode_FindString( IRCode code, char* s, Endr *e ) {
	StringList l = code->strings;
	while ( l ) {
		if( !strcmp( l->str, s ) ) {
			*e = l->label;
			return true;
		}
		l = l->next;
	}
	return false;
}

void IRCode_AddGlobal( IRCode code, CTE cte ) {
	CTE t = code->globals;
	if( !t ) {
		code->globals = cte;
	}
	else {
		while( t->next ) {
			t = t->next;
		}
		t->next = cte;
	}
}

void IRCode_AddFunction( IRCode code, Function func ) {
	Function f = code->functions;
	if( !f ) {
		code->functions = func;
	}
	else {
		while( f->next ) {
			f = f->next;
		}
		f->next = func;
	}
}

void IRCode_DumpToFile( IRCode code, FILE * f) {
	//Primeiro Strings;
	assert( code );
	StringList strl = code->strings;
	while( strl ) {
		fprintf(f, "string $S%d = \"%s\"\n", strl->label.val, strl->str );
		strl = strl->next;
	}
	CTE cte = code->globals;
	while( cte ) {
		fprintf(f, "global %s\n", cte->args[0].str );
		cte = cte->next;
	}

	Function func = code->functions;
	while ( func ) {
		Function_DumpToFile( func , f );
		func = func->next;
	}
}

#endif
