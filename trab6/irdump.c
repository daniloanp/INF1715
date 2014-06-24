#include "basicblock.h"
static int i = 1;
static BasicBlock* Instr_dump(Instr* ins, FILE* fd, BasicBlock* block) {
	const char* x = ins->x.str;
	const char* y = ins->y.str;
	const char* z = ins->z.str;
	const char* fmt;

	if( block &&  block->leader == ins ) {
		printf("\n\n");
	}

	switch (ins->op) {
		case OP_LABEL:		fmt = "\t%d| %s:";			break;
		case OP_GOTO:		fmt = "\t%d| goto %s";		break;
		case OP_PARAM:		fmt = "\t%d| param %s";		break;
		case OP_CALL:		fmt = "\t%d| call %s %s";		break;
		case OP_IF:		fmt = "\t%d| if %s goto %s";	break;
		case OP_IF_FALSE:	fmt = "\t%d| ifFalse %s goto %s";	break;
		case OP_SET:		fmt = "\t%d| %s = %s";		break;
		case OP_SET_BYTE:	fmt = "\t%d| %s = byte %s";	break;
		case OP_SET_IDX:	fmt = "\t%d| %s = %s[%s]";	break;
		case OP_SET_IDX_BYTE:	fmt = "\t%d| %s = byte %s[%s]";	break;
		case OP_IDX_SET:	fmt = "\t%d| %s[%s] = %s";	break;
		case OP_IDX_SET_BYTE:	fmt = "\t%d| %s[%s] = byte %s";	break;
		case OP_RET:		fmt = "\t%d| ret";		break;
		case OP_RET_VAL:	fmt = "\t%d| ret %s";		break;
		case OP_NE:		fmt = "\t%d| %s = %s != %s";	break;
		case OP_EQ:		fmt = "\t%d| %s = %s == %s";	break;
		case OP_LT:		fmt = "\t%d| %s = %s < %s";	break;
		case OP_GT:		fmt = "\t%d| %s = %s > %s";	break;
		case OP_LE:		fmt = "\t%d| %s = %s <= %s";	break;
		case OP_GE:		fmt = "\t%d| %s = %s >= %s";	break;
		case OP_ADD:		fmt = "\t%d| %s = %s + %s";	break;
		case OP_SUB:		fmt = "\t%d| %s = %s - %s";	break;
		case OP_DIV:		fmt = "\t%d| %s = %s / %s";	break;
		case OP_MUL:		fmt = "\t%d| %s = %s * %s";	break;
		case OP_NEG:		fmt = "\t%d| %s = - %s";		break;
		case OP_NEW:		fmt = "\t%d| %s = new %s";	break;
		case OP_NEW_BYTE:	fmt = "\t%d| %s = new byte %s";	break;
	}
	if( block &&  block->leader == ins ) {
		i = 1;
	}
	fprintf(fd, fmt, i, x, y, z);
	if( block &&  block->leader == ins ) {
		printf("\t LEADER");
		block = block->next;
	}
	i++;
	if( ins->xUseInfo != NULL ) {
		printf( " --  var %s live(%d), %p ", ins->x.str, ins->xUseInfo->isALive?1:0, ins->xUseInfo->nextUse);
	}
	if( ins->yUseInfo != NULL ) {
		printf( " --  var %s live(%d), %p ", ins->y.str, ins->yUseInfo->isALive?1:0, ins->yUseInfo->nextUse);	
	}
	if( ins->zUseInfo != NULL ) {
		printf( " --  var %s live(%d), %p ", ins->z.str, ins->zUseInfo->isALive?1:0, ins->zUseInfo->nextUse);
	}
	fprintf(fd, "\n");


	return block;
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