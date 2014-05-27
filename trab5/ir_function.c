#ifndef IR_FUNCTION_C
#define IR_FUNCTION_C

typedef struct _function* Function;

struct _function {
	char *id;
	List args;//Nomes
	CTE commands;
};


#endif