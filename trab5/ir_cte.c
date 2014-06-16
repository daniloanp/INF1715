#ifndef IR_CTE_C
#define IR_CTE_C
#include "ir_cte.h"
#include <malloc.h>
#include <assert.h>
#include <stdio.h>

void cpy_enr( Endr *dest, Endr source ) {
	dest->type = source.type;
	dest->str = source.str;
	dest->val = source.val;
}

void handleCmd( CTE cte, Endr* endr ) {
	assert( cte );
	switch ( cte->cmd ) {
		case ATTR_ADD:
		case ATTR_MIN:
		case ATTR_DIV:
		case ATTR_MUL:
		case ATTR_GTE:
		case ATTR_GT:
		case ATTR_LTE:
		case ATTR_LE:
		case ATTR_EQ:
		case ATTR_NE:
		case ATTR_FROM_ARR:
		case ATTR_BYTE_FROM_ARR:
		case ATTR_TO_ARR:
		case ATTR_BYTE_TO_ARR:
			cte->args[2] = endr[2];
		case ATTR_SIMPLE:
		case ATTR_BYTE:
		case ATTR_NOT:
		case ATTR_UN_MINUS:
		case ATTR_NEW:
		case ATTR_NEW_BYTE:
		case GOTO_IF:
		case GOTO_IF_FALSE:
			cte->args[1] = endr[1];
		case GOTO:
		case CALL:
		case PARAM:
		case RET_VAL:
		case LABEL:
			cte->args[0] = endr[0];
		default: break;
	}
}

Endr Endr_New( EndrType tp, unsigned long val ) {
	Endr endr;// = (Endr)malloc( sizeof( struct endr_ ));
	endr.type = tp;
	endr.val = val;
	endr.str = 0;
	return endr;
}


Endr Endr_NewAsString( EndrType tp, char* s ) {
	Endr endr;
	endr.type = tp;
	endr.val = 0;
	endr.str = s;
	return endr;
}

CTE CTE_New( Instr in, Endr endr[3] ) {
	CTE cte = (CTE)malloc( sizeof( struct _cte ));
	cte->next = NULL;
	cte->cmd = in;
	handleCmd( cte, endr );
	return cte;
}

static void Endr_DumpToFile( Endr e, FILE *f ) {
	switch( e.type ) {
		case ENDR_CONST:
			fprintf( f, "%lu", e.val);
		break;

		case ENDR_STR:
			fprintf( f,"$S%lu", e.val);
		break;

		case ENDR_FUNC:
		case ENDR_VAR:
			fprintf( f,"%s", e.str );
		break;

		case ENDR_TEMP:
			fprintf( f,"$T%lu", e.val );
		break;

		case ENDR_LABEL:
			fprintf( f,".L%lu", e.val );
		break;

		case ENDR_RETEXP:
			fprintf( f,"$ret" );
		break;
		
		default: break;
	
	
	}
}


static void printCMD( CTE cte, FILE* f ) {
	int len = 0;
	char *s = 0;
	char *op = NULL; //CAN be altered
	switch( cte->cmd ) {
		case ATTR_ADD:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " + ");
			Endr_DumpToFile( cte->args[2], f);
		break;

		case ATTR_MIN:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " - ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_DIV:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " / ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_MUL:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " * ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_GTE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " >= ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_GT:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " > ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_LTE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " <= ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_LE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " < ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_EQ:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " == ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_NE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " <> ");
			Endr_DumpToFile( cte->args[2], f);
		break;

		case ATTR_FROM_ARR:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " [ ");
			Endr_DumpToFile( cte->args[2], f);
			fprintf(f, " ] ");
			
			
		break;

		case ATTR_BYTE_FROM_ARR:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = byte ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " [ ");
			Endr_DumpToFile( cte->args[2], f);
			fprintf(f, " ] ");
		break;

		case ATTR_TO_ARR:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " [ ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " ] ");
			fprintf(f, " = ");
			Endr_DumpToFile( cte->args[2], f);
		break;

		case ATTR_BYTE_TO_ARR:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " [ ");
			Endr_DumpToFile( cte->args[1], f);
			fprintf(f, " ] ");
			fprintf(f, " = byte ");
			Endr_DumpToFile( cte->args[2], f);
		break;
		
		case ATTR_SIMPLE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = ");	
			Endr_DumpToFile( cte->args[1], f);
		break;

		case ATTR_BYTE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = byte ");	
			Endr_DumpToFile( cte->args[1], f);
		break;

		case ATTR_NOT:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = not ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		case ATTR_UN_MINUS:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = - ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		case ATTR_NEW:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = new ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		case ATTR_NEW_BYTE:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " = new byte ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		case GOTO_IF:
			fprintf(f, "if ");	
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " goto ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		case GOTO_IF_FALSE:
			fprintf(f, "ifFalse ");	
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, " goto ");	
			Endr_DumpToFile( cte->args[1], f);
		break;
		// DE NOVO, no break;
		case GOTO:
			fprintf(f, "goto ");	
			Endr_DumpToFile( cte->args[0], f);
		break;
		case CALL:
			fprintf(f, "call ");	
			Endr_DumpToFile( cte->args[0], f);
		break;
		case PARAM:
			fprintf(f, "param ");	
			Endr_DumpToFile( cte->args[0], f);
		break;
		case RET_VAL:
			fprintf(f, "ret ");	
			Endr_DumpToFile( cte->args[0], f);
		break;
			
		case RET:
			fprintf(f, "ret ");	
		break;
		case LABEL:
			Endr_DumpToFile( cte->args[0], f);
			fprintf(f, 	":");
		break;

		default: break;
	}
	fprintf(f, "\n");
}

void CTE_Print( CTE cmds, FILE* f ) {
	while( cmds ) {
		printCMD( cmds, f );
		cmds = cmds->next;
	}
}


#endif


