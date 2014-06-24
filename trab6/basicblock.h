#ifndef BASICBLOCK_H
#define BASICBLOCK_H
#include <stdlib.h>
#include <stdbool.h>
#include "ir.h"
#define HASH_SIZE 256
/*
Basic Block struct.
A basic block ends when the "this"->next->leader is achivied by a pathway through "this"->leader following isntructions
*/
typedef struct SymbolTable_ SymbolTable;
struct SymbolTable_ {
	NextUseInfo* info[HASH_SIZE];
};

typedef struct _BasicBlock BasicBlock;
struct _BasicBlock {
	BasicBlock* next;
	Instr* leader;
	Instr* lastInstr;
	SymbolTable* symTable;
};

/* A List of visited labels with a flag to status saying if  or not. */
typedef struct _PrevLabels PrevLabels;
struct _PrevLabels {
	PrevLabels* next;
	Instr* label;
	BasicBlock* parent; //if the label is a leader itself, then parent is NULL
};

/* A List of next labels to be added with ( just for labels ) */
typedef struct _UpcomingLeaders UpcomingLeaders;
struct _UpcomingLeaders {
	UpcomingLeaders* next;
	char* name;
};






SymbolTable* SymbolTable_new( );
void SymbolTable_delete(SymbolTable* st);
NextUseInfo* SymbolTable_update( SymbolTable* st, char* name, bool liveness, Instr* nextUse );
NextUseInfo* SymbolTable_get(SymbolTable* st, char* name);

BasicBlock* IR_BuildBlocks(IR* ir, FILE* fd);
void IR_dump(IR* ir, FILE* fd, BasicBlock* block);
void SymbolTable_handleCode(BasicBlock* blocks);

#endif