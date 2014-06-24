#include "basicblock.h"
#include <assert.h>
#include <string.h>
/*
Output an instruction to the given file descriptor.
*/

static BasicBlock* head = NULL;
static PrevLabels* prevLabels = NULL; 
static UpcomingLeaders* ucLeaders = NULL; 


PrevLabels* PrevLabels_Add( PrevLabels* list, Instr* label, BasicBlock* parent );
PrevLabels* PrevLabels_Get( PrevLabels* list, char* label );
UpcomingLeaders* UpcomingLeaders_Add( UpcomingLeaders* list, char* leader );
UpcomingLeaders* UpcomingLeaders_Get( UpcomingLeaders* list, char* leader );
UpcomingLeaders* UpcomingLeaders_Remove( UpcomingLeaders* list, char* name );
static void BasicBlock_AddInOrder( PrevLabels* label );

BasicBlock* BasicBlock_Add(BasicBlock* blocks, Instr* i );
#define BasicBlock_link(_l1, _l2) ((BasicBlock*)List_link((BasicBlock*)(_l1), (BasicBlock*)(_l2)))




/*
Output an instruction to the given file descriptor.
*/
static BasicBlock* Instr_build(Instr* ins, FILE* fd, BasicBlock* block) {
	char* target = NULL;
	//SymbolTable* st = block->symTable;
	block->lastInstr = ins;
	switch( ins->op ) {
		case OP_LABEL:
			if( UpcomingLeaders_Get(ucLeaders, ins->x.str ) ) {
				if( ins != block->leader ) {
					block = BasicBlock_Add( block, ins );
				}
				UpcomingLeaders_Remove( ucLeaders, ins->x.str );
				prevLabels = PrevLabels_Add( prevLabels, ins, NULL );
			}
			else {
				prevLabels = PrevLabels_Add( prevLabels, ins, block );
			}
		break;

		case OP_GOTO:
			block = BasicBlock_Add( block, ins->next );
			target = ins->x.str;
		break;

		case OP_IF:
		case OP_IF_FALSE:
			block = BasicBlock_Add( block, ins->next );
			target = ins->y.str;
			/*if( isAddrVar( ins->x ) ) {
				SymbolTable_update( st, ins->x.str, true, NULL);
			}*/
		break;

		case OP_CALL:
			block = BasicBlock_Add( block, ins->next );
		break;

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
			/*SymbolTable_update( st, ins->x.str, true, NULL);	
			if( isAddrVar( ins->y ) ) {
				SymbolTable_update( st, ins->y.str, true, NULL);	
			}
			if( isAddrVar( ins->z ) ) {
				SymbolTable_update( st, ins->z.str, true, NULL);	
			}
			break;*/
		}

		case OP_SET:
		case OP_SET_BYTE:
		case OP_NEG:
		case OP_NEW:
		case OP_NEW_BYTE:
		 // o que eu faço com você?
		{
			/*SymbolTable_update( st, ins->x.str, true, NULL);	

			if( isAddrVar( ins->y ) ) {
				SymbolTable_update( st, ins->y.str, true, NULL );
			}
			break;*/
		}

		case OP_PARAM:
		case OP_RET_VAL:
			/*if( isAddrVar( ins->x ) ) {
				SymbolTable_update( st, ins->x.str, true, ins);	
			}*/
		break;


		default: break;
	}
	if( target ) {
		
		PrevLabels* label = PrevLabels_Get( prevLabels, target );
		if ( !label ) {
			ucLeaders = UpcomingLeaders_Add( ucLeaders, target );
		}
		else if (  label->parent ) {
			BasicBlock_AddInOrder( label );
		}
	}

	return block;
}

static void BasicBlock_AddInOrder( PrevLabels* plabel ) {
	BasicBlock* parent = plabel->parent;
	BasicBlock* helper = NULL;
	PrevLabels* iterator = NULL;

	BasicBlock* newBlock = NULL;
 	newBlock = (BasicBlock*)malloc(sizeof(BasicBlock));
 	newBlock->leader = plabel->label;
 	newBlock->lastInstr = parent->lastInstr;
 	if( newBlock->lastInstr  == NULL)
		printf("alert 1");
 	newBlock->symTable = SymbolTable_new( );

 	parent->lastInstr = newBlock->leader->prev;
 	if(parent->lastInstr == NULL)
		printf("alert 2");

 	helper = parent->next;
 	newBlock->next = helper;
 	parent->next = newBlock;
 	

	for( iterator = plabel->next; iterator && iterator->parent == parent; iterator = iterator->next) {
		iterator->parent = newBlock;
	}
	plabel->parent = NULL;
}



static BasicBlock* Function_build( Function* fun, FILE* fd, BasicBlock* block ) {
	Instr* ins;
	ins = fun->code;
	block = BasicBlock_Add( block, ins );
	if ( !head ) {
		head = block;
	}

	//another instructions 
	for ( ins = (fun->code)? fun->code->next: NULL; ins; ins = ins->next ) {
		block = Instr_build(ins, fd, block);
	}
	return block;
}


BasicBlock* IR_BuildBlocks(IR* ir, FILE* fd ) {
	BasicBlock* block;
	for (Function* fun = ir->functions; fun; fun = fun->next) {
		block = Function_build(fun, fd, block);
	}
	block = head;
	head = NULL;
	return block;
}


BasicBlock* BasicBlock_Add(BasicBlock* blocks, Instr* ins ) { //don't keep the head
	if( !ins ) {
		return blocks;
	}
	BasicBlock* el;
	BasicBlock* block  = ( BasicBlock*) malloc( sizeof( BasicBlock ));
	block->symTable = SymbolTable_new( );
	block->next = NULL;
	block->leader = ins;
	if( !blocks ) {
		return block;
	}
	else {
		for( el = blocks; el->next; el = el->next );
		//if( ins->prev ) el->lastInstr = ins->prev;
		el->next = block;
		return block;
	}
}




PrevLabels* PrevLabels_Add( PrevLabels* list, Instr* label, BasicBlock *parent ) {
	//printf("add:, %s\n", label->x.str);
	assert(label);
	PrevLabels* el;
	PrevLabels* newLabel = (PrevLabels*)malloc(sizeof(PrevLabels));
	newLabel->label = label;
	newLabel->parent = parent;
	newLabel->next = NULL;

	if( !list ) {
		return newLabel;
	}
	else {
		for( el = list; el->next; el = el->next );
		el->next  = newLabel;
		return list;
	} 
}

UpcomingLeaders* UpcomingLeaders_Add( UpcomingLeaders* list, char* name ) {
	assert(name);
	UpcomingLeaders* el;
	UpcomingLeaders* upcLeader = (UpcomingLeaders*)malloc(sizeof(UpcomingLeaders));
	upcLeader->name = name;
	upcLeader->next = NULL;
	if( !list ) {
		return upcLeader;
	}
	else {
		for( el = list; el->next; el = el->next );
		el->next  = upcLeader;
		return list;
	} 
}

PrevLabels* PrevLabels_Get( PrevLabels* list, char* str ) {
	PrevLabels* el;
	for( el = list; el; el = el->next ) {
		if( !strcmp( el->label->x.str, str ) ) {
			return el;
		}
	}
	return NULL;
}
UpcomingLeaders* UpcomingLeaders_Remove( UpcomingLeaders* list, char* name ) {
	UpcomingLeaders* el;
	UpcomingLeaders* prev = NULL;
	for( el = list; el; prev = el, el = el->next ) {
		if( ! strcmp( el->name,  name ) ) {
			if( prev == NULL ) {
				prev = el;
				el = el->next;
				free( prev );
				return el;
			}
			else {
				prev->next = el->next;
				free(el);
				return prev;
			}

		}
	}
	return NULL;
}

UpcomingLeaders* UpcomingLeaders_Get( UpcomingLeaders* list, char* name ) {
	UpcomingLeaders* el;
	for( el = list; el; el = el->next ) {
		if( ! strcmp( el->name,  name ) ) {
			return el;
		}
	}
	return NULL;
}

