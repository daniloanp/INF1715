
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ir.h"


static BasicBlock* head = NULL;
static PrevLabels* prevLabels = NULL; 
static UpcomingLeaders* ucLeaders = NULL; 


// -------------------- List --------------------

/*
Concatenate two lists. Either list may be NULL.
In that case, the other one is returned unmodified.
*/
List* List_link(List* l1, List* l2) {
	if (!l1) {
		return l2;
	}
	List* last = l1;
	while (last->next) {
		last = last->next;
	}
	last->next = l2;
	return l1;
}

// -------------------- String --------------------

/*
Allocate a new string entry.
*/
String* String_new(char* name, char* value) {
	String* str = calloc(1, sizeof(String));
	str->name = name;
	str->value = value;
	return str;
}

// -------------------- Variable --------------------

/*
Allocate a new variable entry.
Lists of variables are used for globals, locals and temps.
*/
Variable* Variable_new(char* name) {
	Variable* var = calloc(1, sizeof(Variable));
	var->name = name;
	return var;
}

/*
Add variable var to the end of the list.
*/
Variable* Variable_append(Variable* list, Variable* var) {
	if (!list) {
		return var;
	}
	Variable* last = list;
	while (last->next) {
		last = last->next;
	}
	last->next = var;
	return list;
}

/*
Given a name, return a variable (plus its position in variable *pos).
This function actually works for String lists as well,
because they are structurally compatible.
*/
static Variable* Variable_find(Variable* list, const char* name, int* pos) {
	int i;
	Variable* v;
	for (v = list, i = 0; v; v = v->next, i++) {
		if (strcmp(name, v->name) == 0) {
			*pos = i;
			return v;
		}
	}
	*pos = i;
	return NULL;
}

// -------------------- Addr --------------------

/*
Create an Addr entry for literal numbers. 
*/
Addr Addr_litNum(int num) {
	Addr addr;
	addr.type = AD_NUMBER;
	addr.num = num;
	char* str = malloc(21);
	snprintf(str, 20, "%d", num);
	addr.str = str;
	return addr;
}

/*
Create an Addr entry for labels. 
*/
Addr Addr_label(char* label) {
	Addr addr;
	addr.type = AD_LABEL;
	addr.num = -1;
	addr.str = label;
	return addr;
}

/*
Create an Addr entry for function names. 
*/
Addr Addr_function(char* name) {
	Addr addr;
	addr.type = AD_FUNCTION;
	addr.num = -1;
	addr.str = name;
	return addr;
}

/*
This function produces an Addr from a given name.

If the name matches an existing global, local or temp,
it returns an Addr containing its name and index.

If the name is not found, it creates and store a
new local or temp entry, and returns an Addr with
its name and index.
*/
Addr Addr_resolve(char* name, IR* ir, Function* fun) {
	Addr addr;
	addr.str = name;
	if (name[0] == '$') {
		int i;
		Variable* v = Variable_find(fun->temps, name, &i);
		if (!v) {
			v = Variable_new(name);
			fun->temps = Variable_append(fun->temps, v);
		}
		addr.type = AD_TEMP;
		addr.num = i;
		return addr;
	}
	int i;
	Variable* v = Variable_find(ir->globals, name, &i);
	if (v) {
		addr.type = AD_GLOBAL;
		addr.num = i;
		return addr;
	}
	String* s = (String*) Variable_find((Variable*) ir->strings, name, &i);
	if (s) {
		addr.type = AD_STRING;
		addr.num = i;
		return addr;
	}
	addr.type = AD_LOCAL;
	v = Variable_find(fun->locals, name, &i);
	if (v) {
		addr.num = i;
		return addr;
	}
	v = Variable_new(name);
	fun->locals = Variable_append(fun->locals, v);
	addr.num = i;
	return addr;
}

/*
Use this function to compare two Addrs.
This way no string comparison is necessary.
*/
bool Addr_eq(Addr a1, Addr a2) {
	return (a1.type == a1.type && a1.num == a1.num);
}

// -------------------- Instr --------------------

/*
Allocate a new Instr.
This function may receive up to three extra arguments,
representing x, y and z in the three-address code representation.
*/
Instr* Instr_new(Opcode op, ...) {
	va_list ap;
	va_start(ap, op);
	Instr* ins = calloc(1, sizeof(Instr));
	ins->op = op;
	switch (op) {
		// instructions with x only
		case OP_LABEL:
		case OP_GOTO:
		case OP_PARAM:
		case OP_RET_VAL:
		{
			ins->x = va_arg(ap, Addr);
			break;
		}
		// instructions with x and y
		case OP_IF:
		case OP_IF_FALSE:
		case OP_SET:
		case OP_SET_BYTE:
		case OP_NEG:
		case OP_NEW:
		case OP_NEW_BYTE:
		case OP_CALL:
		{
			ins->x = va_arg(ap, Addr);
			ins->y = va_arg(ap, Addr);
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
			ins->x = va_arg(ap, Addr);
			ins->y = va_arg(ap, Addr);
			ins->z = va_arg(ap, Addr);
			break;
		}
		// instruction with no args
		case OP_RET:
		{
			break;
		}
	}
	va_end(ap);
	return ins;
}

/*
Output an instruction to the given file descriptor.
*/
static BasicBlock* Instr_dump(Instr* ins, FILE* fd, BasicBlock* block) {
	const char* x = ins->x.str;
	const char* y = ins->y.str;
	const char* z = ins->z.str;
	const char* fmt;
	switch (ins->op) {
		case OP_LABEL:		fmt = "%s:";			break;
		case OP_GOTO:		fmt = "\tgoto %s";		break;
		case OP_PARAM:		fmt = "\tparam %s";		break;
		case OP_CALL:		fmt = "\tcall %s %s";		break;
		case OP_IF:		fmt = "\tif %s goto %s";	break;
		case OP_IF_FALSE:	fmt = "\tifFalse %s goto %s";	break;
		case OP_SET:		fmt = "\t%s = %s";		break;
		case OP_SET_BYTE:	fmt = "\t%s = byte %s";	break;
		case OP_SET_IDX:	fmt = "\t%s = %s[%s]";	break;
		case OP_SET_IDX_BYTE:	fmt = "\t%s = byte %s[%s]";	break;
		case OP_IDX_SET:	fmt = "\t%s[%s] = %s";	break;
		case OP_IDX_SET_BYTE:	fmt = "\t%s[%s] = byte %s";	break;
		case OP_RET:		fmt = "\tret";		break;
		case OP_RET_VAL:	fmt = "\tret %s";		break;
		case OP_NE:		fmt = "\t%s = %s != %s";	break;
		case OP_EQ:		fmt = "\t%s = %s == %s";	break;
		case OP_LT:		fmt = "\t%s = %s < %s";	break;
		case OP_GT:		fmt = "\t%s = %s > %s";	break;
		case OP_LE:		fmt = "\t%s = %s <= %s";	break;
		case OP_GE:		fmt = "\t%s = %s >= %s";	break;
		case OP_ADD:		fmt = "\t%s = %s + %s";	break;
		case OP_SUB:		fmt = "\t%s = %s - %s";	break;
		case OP_DIV:		fmt = "\t%s = %s / %s";	break;
		case OP_MUL:		fmt = "\t%s = %s * %s";	break;
		case OP_NEG:		fmt = "\t%s = - %s";		break;
		case OP_NEW:		fmt = "\t%s = new %s";	break;
		case OP_NEW_BYTE:	fmt = "\t%s = new byte %s";	break;
	}
	fprintf(fd, fmt, x, y, z);
	if( block &&  block->leader == ins ) {
		printf("\t LEADER");
		block = block->next;
	}
	fprintf(fd, "\n");
	return block;
}


/*
Output an instruction to the given file descriptor.
*/
static BasicBlock* Instr_build(Instr* ins, FILE* fd, BasicBlock* block) {
	char* target = NULL;
	
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
		break;

		default: break;
	}

	if( target ) {
		
		PrevLabels* label = PrevLabels_Get( prevLabels, target );
		if ( !label ) {
			printf("(1) %s\n\n", target);
			ucLeaders = UpcomingLeaders_Add( ucLeaders, target );
		}
		else if (  label->parent ) {
			printf("(2) %s\n\n", target);
			BasicBlock_AddInOrder( label );
		}
	}
	return block;
}

void BasicBlock_AddInOrder( PrevLabels* plabel ) {
	BasicBlock* parent = plabel->parent;
	BasicBlock* helper = NULL;
	PrevLabels* iterator = NULL;

	BasicBlock* newBlock = NULL;
 	newBlock = (BasicBlock*)malloc(sizeof(BasicBlock));
 	newBlock->leader = plabel->label;

 	helper = parent->next;
 	newBlock->next = helper;
 	parent->next = newBlock;
 	

	for( iterator = plabel->next; iterator && iterator->parent == parent; iterator = iterator->next) {
		iterator->parent = newBlock;
	}
	plabel->parent = NULL;
}

// -------------------- Function --------------------

/*
Allocate a new function, with a given name and a given set of arguments.
*/
Function* Function_new(char* name, Variable* args) {
	Function* fun = calloc(1, sizeof(Function));
	fun->name = name;
	fun->locals = args;
	int nArgs = 0;
	for (Variable* a = args; a; a = a->next) {
		nArgs++;
	}
	fun->nArgs = nArgs;
	return fun;
}

/*
Output a function to the given file descriptor.
*/
static BasicBlock* Function_dump(Function* fun, FILE* fd, BasicBlock* block) {
	Variable* arg;
	fprintf(fd, "fun %s (", fun->name);
	arg = fun->locals;
	for (int i = 0; i < fun->nArgs; i++) {
		if (i > 0) {
			fprintf(fd, ", ");
		}
		fprintf(fd, "%s", arg->name);
		arg = arg->next;
	}
	fprintf(fd, ")\n");
	
	for (Instr* ins = fun->code; ins; ins = ins->next ) {
		block = Instr_dump( ins, fd, block );
	}
	return block;
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

// -------------------- IR --------------------

/*
Allocate a new IR data structure.
*/
IR* IR_new() {
	IR* ir = calloc(1, sizeof(IR));
	return ir;
}

/*
Set the list of literal strings.
*/
void IR_setStrings(IR* ir, String* strings) {
	ir->strings = strings;
}

/*
Set the list of globals.
*/
void IR_setGlobals(IR* ir, Variable* globals) {
	ir->globals = globals;
}

/*
Add a function to the IR data structure.
*/
void IR_addFunction(IR* ir, Function* fun) {
	if (!ir->functions) {
		ir->functions = fun;
		return;
	}
	Function* lastFn = ir->functions;
	while (lastFn->next) {
		lastFn = lastFn->next;
	}
	lastFn->next = fun;
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
/*
Output the entire IR data structure to the given file descriptor.
This outputs a file equivalent to the one given as input to the program. With annotation about basic blocks
*/
void IR_dump(IR* ir, FILE* fd, BasicBlock* block) {
	for (String* s = ir->strings; s; s = s->next) {
		fprintf(fd, "string %s = %s\n", s->name, s->value);
	}
	fprintf(fd, "\n");
	for (Variable* v = ir->globals; v; v = v->next) {
		fprintf(fd, "global %s\n", v->name);
	}
	fprintf(fd, "\n");
	for (Function* fun = ir->functions; fun; fun = fun->next) {
		block = Function_dump(fun, fd, block);
		fprintf(fd, "\n");
	}
}

BasicBlock* BasicBlock_Add(BasicBlock* blocks, Instr* ins ) { //don't keep the head
	if( !ins ) {
		return blocks;
	}
	BasicBlock* el;
	BasicBlock* block  = ( BasicBlock*) malloc( sizeof( BasicBlock ));
	block->next = NULL;
	block->leader = ins;
	if( !blocks ) {
		return block;
	}
	else {
		for( el = blocks; el->next; el = el->next );
		el->next = block;
		return block;
	}
}

PrevLabels* PrevLabels_Add( PrevLabels* list, Instr* label, BasicBlock *parent ) {
	printf("add:, %s\n", label->x.str);
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





