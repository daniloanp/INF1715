#include "token.h"
#include <stdlib.h>
#include <string.h>


union token_val
{
unsigned long number;
char* string;
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

//unsafe, you must remember of call free
TokenValue createTokenStringValue(char* str)
{
	TokenValue pt;
	pt = (TokenValue) malloc(sizeof(union token_val));
	if(pt != NULL){
	
		pt->string = malloc(strlen(str)*sizeof(char));
		strcpy(pt->string, str);
	}else 
		return NULL;
}

//unsafe, you must remember of call free
TokenValue createTokenNumberValue(long number)
{
	TokenValue pt;
	pt = (TokenValue) malloc(sizeof(union token_val));
	if(pt != NULL)
		pt->number = number;
	else 
		return NULL;
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

TokenKind tokenKind(Token t)
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
	if(t->kind==IDENTIFIER || t->kind == STRING)
		free(t->value->string);
	free(t);
}		