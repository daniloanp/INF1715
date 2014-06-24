
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"
#include "basicblock.h"

extern FILE* yyin;
extern int yyparse();
extern int yydebug;

extern IR* ir;

char * getOutPutFileName( char *inputf ) {
	char *s,*c;
	s = (char*)calloc( sizeof(char), (strlen( inputf )+3));
	strcpy( s, inputf);
	for(c = s;*c; c++);
	*c = '.'; c++;
	*c = 's'; c++;
	*c = '\0'; c++;
	return s;
}


int main(int argc, char** argv) {
	BasicBlock* head = NULL;
	char* asmFileName;
	FILE* output;
	int err;
	//yydebug = 1;
	if (argc < 2) {
		fprintf(stderr, "Uso: %s arquivo.m0.ir\n", argv[0]);
		exit(1);
	}
	yyin = fopen(argv[1], "r");
	err = yyparse();
	fclose(yyin);
	if (err != 0) {
		fprintf(stderr, "Error reading input file.\n");
		exit(1);
	}
	asmFileName = getOutPutFileName( argv[1] );
	output = fopen( asmFileName, "w");
	free( asmFileName );
	//We need do Something here;

	head = IR_BuildBlocks( ir, stdout );

	//SymbolTable* tb = SymbolTable_new( );
	//SymbolTable_update(tb, "a", false, head->leader);
	//sNextUseInfo* nu= SymbolTable_get(tb, "a");
	//printf("%d, %p", nu->isALive?1:0, nu->nextUse);
	SymbolTable_handleCode( head );

	IR_dump(ir, stdout, head);

	fclose( output );
	
	return 0;
}

