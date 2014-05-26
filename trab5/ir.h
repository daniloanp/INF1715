typedef struct _strings Strings;

struct _strings {
	unsigned long id;
	char* str;
	Strings* next;
};

typedef struct _global Global;

struct _global {
	unsigned long id;
	char* name;
	Global* next;
};

typedef struct _function* Function;

struct _function {
	char *id;
	Args args;


};

typedef struct _ir_code* IRCode;

struct _ir_code {
	String strings; //Lista de Declarações, Simples
	Global globals; //Lista de Globais, Complexo.
	Function functions; // Lista de Programs
};

typedef struct _endr* Endr;
typedef enum { LITSTRING, LITNUM, ID, LABEL } EndrType;


struct _endr {
	EndrType type;
	Op op;
	unsigned long val; //if LITSTRING or label, assumes val as the "identifier of String";
	char* id;
};


typedef enum {
 
} Instr;

typedef struct _cte* CTE;

struct _cte {
	Instr cmd;
	Endr args[3];
	CTE next;
};


IRCode code;


void build_ir ( AST tree ) {
	AST c; 
	int i,j;

	if (t == NULL) {
		return;
	}	
	printf( "\n" );
	c = AST_GetFirstChild( tree );

	while( c != NULL ) {
		if( AST_GetType( c ) == AST_DECL_VAR ) {
			add_global( c );
		}
		else {
			add_function( c );
		}
		
		c = AST_GetNextSibling(c);
	}
}

void args( Function, )

Function build_function( AST nodeFunc ) {
	Function function = (Function)malloc( sizeof( struct _function ) );
	AST functionChild = AST_GetFirstChild( nodeFunc ) ;

	function->id = AST_GetStringValue( nodeFunc );
	if ( AST_GetType( functionChild ) == AST_PARAM_LIST ) {
		args( function, functionChild );
		functionChild = AST_GetNextSibling( functionChild );
	}
	
	if ( AST_GetType( functionChild ) != AST_BLOCK ) {
		functionChild = AST_GetNextSibling( functionChild );	
	}

	//BLOCK

	block( function , functionChild );

	return function;

}

void add_function( AST nodeFunc ) {
	Function* pFunction;
	//HEAD
	//function = build_function( func );
	for( pFunction = &(code->function); *pFunction != NULL; pFunction = &(*pFunction)->next );
	*pFunction = build_function( func );

	//END
}









