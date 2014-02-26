#include "token.h"
#include <stdlib.h>
#include <string.h>


union token_val
{
unsigned long number;
char* string;
char character;
};

struct struct_token {
	TokenKind kind;
	TokenValue value;
	long line;
};

//assuming that t0 and t1 are valid pointers to Token
int cmpToken(void* e0, void* e1)
{
	Token t0= (Token)e0;
	Token t1= (Token)e1;

	if(t0==NULL||t1==NULL)
		exit(1);
	if(t0->kind != t1->kind )
		return 1;
	if(t1->kind == NUMBER)
		if(t1->value->number > t0->value->number)
			return -1;
		else if(t1->value->number < t0->value->number)
			return 1;
		else
			return 0;
	if(t1->kind==IDENTIFIER || t1->kind == STRING)
		return strcmp(t1->value->string, t0->value->string);
	return 0;
}


static char* fixString(char *s)
{
	int i=0;
	char* t = NULL;
	t = malloc((strlen(s)+1)*sizeof(char));
	//initializeing value cause valgrind is boring;
	memset(t,0,(strlen(s)+1));
	// end_valgrind
	if(t==NULL)
		exit(1);
	if(*s=='\"')
		++s;
	
	for( ; *s; s++ ) {
		if(*s=='\"' && *(s+1)=='\0')
		{
			if(*(s-1)=='\\')
			{
				t[i]=*s;
				i++;
			}
		}else if( !(*s=='\\' && *(s+1)=='\\')) {
			t[i]=*s;
			i++;
		}
	}
	return t;

}

//unsafe, you must remember of call free
TokenValue createTokenStringValue(char* str)
{
	char *temp;
	TokenValue pt;
	pt = (TokenValue) malloc(sizeof(union token_val));
	if(pt != NULL){
		temp = fixString(str);
		pt->string = malloc((strlen(temp)+1)*sizeof(char));
		strcpy(pt->string, temp);
		free(temp);
		return pt;
	}else 
		return NULL;
}

//unsafe, you must remember of call free
TokenValue createTokenNumberValue(long number)
{
	TokenValue pt = NULL;
	pt = (TokenValue) malloc(sizeof(union token_val));
	if(pt != NULL)
		pt->number = number;
	else 
		return NULL;
	return pt;
}

TokenValue createTokenCharValue(char c)
{
	TokenValue pt = NULL;
	pt = (TokenValue) malloc(sizeof(union token_val));
	memset(pt,0,sizeof(union token_val));
	if(pt != NULL)
		pt->character = c;
	else {
		return NULL;
	}
	return pt;
}

Token newToken(TokenKind kind, long line, TokenValue value )
{
	Token pt = 0;
	pt = (Token) malloc(sizeof(struct struct_token));
	if( pt != NULL)
	{
		pt->value = value;
		pt->kind = kind;
		pt->line = line;
	}
	else 
		return NULL;
}

int tokenGetLine(Token t)
{
	if(t!=NULL)
			return t->line;
		else 
			exit(1);
	return -1;
}

TokenKind tokenGetKind(Token t)
{
	if(t!=NULL)
		return t->kind;
	else 
		exit(1);
	return (TokenKind)NULL;
}

TokenValue tokenGetVal(Token t)
{
	if(t!=NULL)
		return t->value;
	else 
		exit(1);
	return (TokenValue)NULL;
	
}

int deleteToken(Token t)
{
	if(t==NULL)
		return -1;
	if(t->value != NULL)
	{
		if( t->value->string!=NULL && (t->kind==IDENTIFIER || t->kind == STRING) )
			free(t->value->string);
		free(t->value);
	}

	free(t);
}

char* tokenGetStringValue(Token t){
	return t->value->string;
}
long tokenGetNumberValue(Token t)
{
	return t->value->number;
}
char tokenGetCharValue(Token t)
{
	return t->value->character;
}			

static char* token_string[] = { "IDENTIFIER","IF","ELSE","END","WHILE","LOOP","FUN","RETURN","NEW","NL","INT","CHAR","BOOL","AND","OR","NOT","BOOLEAN","NUMBER","STRING","CHARACTER","PLUS","MINUS","MUL","DIV","OP_BRACKET","CL_BRACKET","OP_PARENTHESIS","CL_PARENTHESIS","COMMA","COLON","GREATER","GREATER_EQUAL","LESS","LESS_EQUAL","EQUAL","DIFFERENT"};

char* tokenToString(Token t)
{
	if(t->kind == ERROR)
		return "ERROR";
	else return token_string[((int)t->kind)-1];
}
