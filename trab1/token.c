#include "token.h"
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>


union token_val {
	unsigned long number;
	char* string;
	bool boolean;
};

typedef union token_val TokenValue;

struct struct_token {
	TokenKind kind;
	TokenValue value;
	long line;
};

//assuming that t0 and t1 are valid pointers to Token
int Token_IsEqual(void* e0, void* e1) {
	Token t0= (Token)e0;
	Token t1= (Token)e1;
	assert(t0);
	assert(t1);

	if (t0 == t1) { //same pointer
   		return 1;
   	}
	if ( !t0 || !t1 ) {
		return 0;
	}
	if( t0->kind != t1->kind ) {
		return 1;
	}
	if( t1->kind == TK_INT_VAL || t1->kind == TK_BOOL_VAL ) {
		if( t1->value.number > t0->value.number ) {
			return -1;
		}
		else if( t1->value.number < t0->value.number ) {
			return 1;
		}
		else {
			return 0;
		}
	}
	if( t1->kind== TK_IDENTIFIER || t1->kind == TK_STRING_VAL ) {
		return strcmp( t1->value.string, t0->value.string );
	}
	return 0;
}


static char* _fixString(char *s) {
	int i=0;
	char* t = NULL;
	assert(s);
	t = malloc((strlen(s)+1)*sizeof(char));
	assert(t);
	if(*s=='\"') {
		++s;
	}
	for( ; *s; ++s ) {
		if(*s=='\"' && *(s+1)=='\0') {
			break;
		}
		if( *s=='\\' ) {
			switch (*(s+1)) {
				case 'n':
					t[i] = '\n'; break;
				case 't':
					t[i] = '\t'; break;
				case '\\':
					t[i] = '\\'; break;
				case '\"':
					t[i] = '\"'; break;
				default: 
					i--;break;
			}
			++i;
			++s;
		}
		else {
			t[i]=*s;
			++i;
		}
	}
	t[i] = 0;
	return t;
}

//unsafe, you must remember of call free
Token Token_NewAsString ( TokenKind tk, unsigned long line, char* val ) {
	assert(val);
	Token t = Token_New( tk, line );
	t->value.string =  _fixString( val ); //The "string" size(marked by '\0') can be shorter than the allocated size. It is nota problem.	
	return t;
}

Token Token_NewAsInteger ( TokenKind tk, unsigned long line, unsigned long val ) {
	Token t = Token_New( tk, line );
	t->value.number = val;
	return t;
}

Token Token_NewAsBool ( TokenKind tk, unsigned long line, bool val) {
	Token t = Token_New( tk, line );
	t->value.boolean = val;
	return t;
}
Token Token_New( TokenKind kind, long line ) {
	Token pt = 0;
	pt = (Token) malloc(sizeof(struct struct_token) );
	if( pt != NULL) {
		pt->value.string = NULL;
		pt->kind = kind;
		pt->line = line;
		return pt;
	}
	else {
		printf("malloc has failed at Token_New");
		return NULL;
	}
}

int Token_GetLine ( Token t ) {
	if ( !t ) {
		return (TokenKind)NULL;
	}
	else {	
		return t->line;
	}
}

TokenKind Token_GetKind( Token t ) {
	if ( !t ) {
		return (TokenKind)NULL;
	}
	else {
		return t->kind;
	}
}


int Token_Destroy(Token t) {
	if ( t==NULL )
		return 1;
	if( t->kind == TK_IDENTIFIER || t->kind == TK_STRING_VAL ) {	
		if( t->value.string!=NULL ) {
			free(t->value.string);
		}
	}
	free(t);
	return 1;
}

int Token_Free(Token t) {
	if ( t != NULL ) {
		free(t);
	}
	return 1;
}

char* Token_GetStringValue(Token t) {
	if ( t != NULL ) {
		return t->value.string;
	}
	else {
		return NULL;
	}
}

unsigned long Token_GetIntValue(Token t) {
	assert( t );	
	return t->value.number;
}

bool Token_GetBoolValue(Token t) {
	assert( t );	
	return t->value.boolean;
}

static char* token_string[] = {
	"ID",
	"if",
	"else",
	"end",
	"while",
	"loop",
	"fun",
	"return",
	"new",
	"nl",
	"int",
	"char",
	"bool",
	"and",
	"or",
	"not",
	"BOOL_VAL", 
	"INT_VAL",
	"string",
	"+",
	"-",
	"*",
	"/",
	"[",
	"]",
	"(",
	")",
	",",
	":",
	">",
	">=",
	"<",
	"<=",
	"=",
	"<>",
	"STRING_VAL"
};

char* Token_GetStringForKind(Token t) {
	//assert(t);
	if ( !t ) {
		return "<--Invalido-->";
	}
	if(t->kind == TK_ERROR) {
		return "ERROR";
	}
	else return token_string[((int)t->kind)-1];
}

char* TokenKind_ToString(TokenKind tk) {
	if ( tk == TK_ERROR ) {
		return "ERROR";
	}
	return token_string[((int)tk)-1];
}
