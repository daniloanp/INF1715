#include <malloc.h>
#include "token_list.h"

struct struct_token_list {
	TokenList previous;
	TokenList next;
	Token token;
};

//Unsafe! The below functions assume that Token instances can be "destroyed" when convenient
TokenList TokenList_New( Token t ) {
	if(t == NULL) {
		return NULL;
	}
	TokenList tl = NULL;
	tl = (TokenList)malloc(sizeof(struct struct_token_list));
	if ( !tl ) {
		fprintf(stderr, "Malloc has failed! at TokenList_New function in the token_list.c file" );
		return NULL;
	}
	tl->previous = NULL;
	tl->next = NULL;
	tl->token = t;
	return tl;
}

int TokenList_Destroy(TokenList tl) {
	TokenList next;
	if(tl == NULL) {
		return 1;
	}
	tl = TokenList_GetFirst(tl);

	while(  tl ) {
		next = tl->next;
		Token_Destroy(tl->token);
		free(tl);
		tl=next; 
	}
	
	return 1;
}

int TokenList_Free(TokenList tl) {
	TokenList next;
	if(tl == NULL)
		return 1;
	tl = TokenList_GetFirst(tl);
	next = tl->next;

	while(  next != NULL ) {
		tl=next; 
		next=tl->next;
		Token_Free(tl->token);
		free(tl);
	}
	
	return 1;
}

TokenList TokenList_Insert( TokenList tl, Token t ) {
	TokenList newNode = TokenList_New(t);
	if(tl==NULL) {
		return newNode;
	}
	else {
		while ( tl->next != NULL ) {
			tl = tl->next;
		}
		newNode->previous = tl;
		tl->next = newNode;

		return newNode;
	}
}

int TokenList_Find(TokenList tl, Token t) {
	tl = TokenList_GetFirst(tl);
	for( ; tl != NULL; tl = tl->next ) { // safe
		if( Token_IsEqual( tl->token, t ) ) {
			return 1;
		}
	}
	return 0;
}

void TokenList_Remove(TokenList tl, Token t)
{
	TokenList oldNode;
	tl = TokenList_GetFirst(tl);
	for ( ; tl != NULL; tl = tl->next )	{ //safe
		if ( Token_IsEqual( tl->token, t ) )	{
			oldNode = tl;
			tl->previous->next = tl->next;
			tl->next->previous = tl->previous;
			Token_Free(oldNode->token);
			free(oldNode);
		}
	}
}

TokenList TokenList_Next( TokenList tl ) {
	if( tl == NULL ) {
		return tl;
	}
	else {
		return tl->next;
	}
}

TokenList TokenList_Previous( TokenList tl )
{
	if( tl == NULL) {
		return tl;
	}
	return tl->previous;
}

TokenList TokenList_GetFirst(TokenList tl)
{
	if( tl == NULL) {
		return tl;
	}
	while( tl->previous != NULL ) {
		tl = tl->previous;
	}
	return tl;
}

TokenList TokenList_GetLast( TokenList tl )
{
	if( tl == NULL) {
		return tl;
	}
	while(tl->next != NULL) {
		tl=tl->next;
	}
	return tl;
}

TokenList TokenList_ExecuteForEach(TokenList tl, tokenCallBack func)
{
	tl = TokenList_GetFirst(tl);//Safe
	for( ; tl != NULL; tl = tl->next ) {
		func(tl->token);
	}
	return tl;
}

Token TokenList_GetCurrentToken(TokenList tl) {
	if(!tl)
		return NULL;
	return tl->token;
}

