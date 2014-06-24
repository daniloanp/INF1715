#ifndef SYMBOLTABLE_C
#define SYMBOLTABLE_C
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "basicblock.h"

static unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    while ( (c = *str++) ) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return ( hash % HASH_SIZE ); //changed
}


SymbolTable* SymbolTable_new( ) {
	SymbolTable* st = ( SymbolTable* )malloc( sizeof( SymbolTable ) );
	memset ( st->info, 0, sizeof(NextUseInfo*) * HASH_SIZE );
	return st;
}

NextUseInfo* SymbolTable_update( SymbolTable* st, char* name, bool liveness, Instr* nextUse ) {
	NextUseInfo* var = NULL; 
	NextUseInfo* prevVar = NULL; 
	unsigned long i;
	assert(st);
	i = hash((unsigned char*)name);
	
	for( var = st->info[i]; var ; var = var->next ) {
		
		if( !strcmp(var->name, name)) {
			var->isALive = liveness;
			var->nextUse = nextUse;
			return var;
		}
		prevVar = var;
	}
	var = ( NextUseInfo* )malloc( sizeof(NextUseInfo) );
	var->isALive = liveness;
	var->nextUse = nextUse;
	var->name = name;
	var->next = NULL;
	
	if( prevVar != NULL ) {
		prevVar->next = var;
	} else {
		st->info[i] = var;
	}
	return var;
}

 NextUseInfo* SymbolTable_get(SymbolTable* st, char* name) {
	unsigned long i;
	NextUseInfo* var;
	NextUseInfo* prevVar = NULL; 
	assert(st);
	i = hash((unsigned char*)name);
	for( var = st->info[i]; var ; var = var->next ) {
		if( !strcmp(var->name, name) ) {
			return var;
		}
		prevVar = var;
	}
	if( prevVar != NULL ) {
		prevVar->next = var;
	} else {
		st->info[i] = var;
	}
	return var;
}



void SymbolTable_delete( SymbolTable* st ) {
	NextUseInfo* var; 
	NextUseInfo* prevVar;
	int i;
	for( i = 0; i < HASH_SIZE; i++ ) {
		var = st->info[i];
		while(  var  ) { 
			prevVar = var;
			var = var->next;
			free(prevVar);
		}
	}
	free(st);
}

static void cpyNextUseInfo( NextUseInfo* src , NextUseInfo** dest ) {
	if( !(*dest) ) {
		*dest = calloc( 1, sizeof( NextUseInfo ));
	}
	**dest = *src;
}


//newVar
//Assumes addr as a var address(that means a.type is equal to AD_LOCAL,AD_GLOBAL or AD_TEMP)
static NextUseInfo* getAddr( SymbolTable* st, Addr addr) {
	NextUseInfo* nx;
	nx = SymbolTable_get( st, addr.str);
	if ( !nx ) {
		if(addr.type == AD_TEMP) {
			nx = SymbolTable_update( st, addr.str, false, NULL );
		}
		else {
			nx = SymbolTable_update( st, addr.str, true, NULL );
		}
	}
	return nx;
}

//#define addIfNULL(a,b,c) if(a==NULL){SymbolTable_update(st, ins->x.str,b, c);a=SymbolTable_get(st, ins->x.str);}
static void setInstrInfo( SymbolTable* st, Instr* ins ) {
	
	switch( ins->op ) {
		// instructions with x only
		case OP_LABEL: break;
		case OP_GOTO: break;

		case OP_IF:
		case OP_IF_FALSE:
		case OP_PARAM:
		case OP_RET_VAL:
			if( isAddrVar( ins->x ) ) {
				cpyNextUseInfo( getAddr( st, ins->x ) , &(ins->xUseInfo) );
			}
			
		break;
		// instructions with x and() y
		

		case OP_CALL: break;

		case OP_SET:
		case OP_SET_BYTE:
		case OP_NEG:
		case OP_NEW:
		case OP_NEW_BYTE:
		 // o que eu faço com você?
		{
			cpyNextUseInfo( getAddr( st, ins->x ), &(ins->xUseInfo) );

			if( isAddrVar( ins->y ) ) {
				cpyNextUseInfo( getAddr( st, ins->y ), &(ins->yUseInfo) );
			}
			break;
		}
		// instruction with x, y and z
		case OP_SET_IDX:
		case OP_SET_IDX_BYTE:
		case OP_IDX_SET:
		case OP_IDX_SET_BYTE:
		case OP_NE:
		case OP_EQ:
		case OP_LT:
		case OP_GT:
		case OP_LE:
		case OP_GE:
		case OP_ADD:
		case OP_SUB:
		case OP_DIV:
		case OP_MUL:
		{
			cpyNextUseInfo( getAddr( st, ins->x), &(ins->xUseInfo) );
			if( isAddrVar( ins->y ) ) {
				cpyNextUseInfo( getAddr( st, ins->y ), &(ins->yUseInfo) );
			}
			if( isAddrVar( ins->z ) ) {
				cpyNextUseInfo( getAddr( st, ins->z ), &(ins->zUseInfo) );
			}
			break;
		}
		// instruction with no args
		case OP_RET:
		break;
	}
}


static void updateTable( SymbolTable* st, Instr* ins ) {
	switch( ins->op ) {
		// instructions with x only
		case OP_LABEL: break;
		case OP_GOTO: break;

		case OP_IF:
		case OP_IF_FALSE:
		case OP_PARAM:
		case OP_RET_VAL:
			if( isAddrVar( ins->x ) ) {
				SymbolTable_update( st, ins->x.str, true, ins);	
			}
		break;
		// instructions with x and() y
		case OP_CALL: break;
		case OP_SET:
		case OP_SET_BYTE:
		case OP_NEG:
		case OP_NEW:
		case OP_NEW_BYTE:
		 // o que eu faço com você?
		{
			SymbolTable_update( st, ins->x.str, false, NULL);	
			if( isAddrVar( ins->y ) ) {
				SymbolTable_update( st, ins->y.str, true, ins );
			}
			break;
		}
		// instruction with x, y and z
		case OP_SET_IDX:
		case OP_SET_IDX_BYTE:
		case OP_IDX_SET:
		case OP_IDX_SET_BYTE:
		case OP_NE:
		case OP_EQ:
		case OP_LT:
		case OP_GT:
		case OP_LE:
		case OP_GE:
		case OP_ADD:
		case OP_SUB:
		case OP_DIV:
		case OP_MUL:
		{
			SymbolTable_update( st, ins->x.str, false, NULL);	
			//}
			if( isAddrVar( ins->y ) ) {
				SymbolTable_update( st, ins->y.str, true, ins);	
			}
			if( isAddrVar( ins->z ) ) {
				SymbolTable_update( st, ins->z.str, true, ins);	
			}
			break;
		}
		// instruction with no args
		case OP_RET:
		break;
		
	}
}


void SymbolTable_handleCode(BasicBlock* blocks) {
	Instr* ins;
	BasicBlock* bl;
	SymbolTable* st;
	bl = blocks;
	//int i = 0;
	for( bl = blocks; bl; bl = bl->next ) { // Itera sobre blocos básicos
		ins = bl->lastInstr;
		st = bl->symTable;
		while( ins ) { // 
			setInstrInfo( st, ins ); // passo 1
			updateTable( st, ins ); // passo 2 e 3
			if( ins == bl->leader ) {
				break;
			}
			ins = ins->prev;
		}
	}
}





#endif