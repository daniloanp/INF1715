#include <malloc.h>
#include "tokenList.h"

struct struct_token_list {
	TokenList previous;
	TokenList next;
	Token token;
};

//Unsafe! The below functions assume that Token instances can be "destroyed" when convenient
TokenList tokenListCreate(Token t) {
	if(t == NULL) {
		return NULL;
	}
	TokenList tl=NULL;
	tl= (TokenList)malloc(sizeof(struct struct_token_list));
	tl->previous = NULL;
	tl->next = NULL;
	tl->token = t;
	return tl;
}

int tokenListDestroy(TokenList tl) {
	TokenList next;
	if(tl == NULL)
		return 1;
	tl = tokenListGetFirst(tl);

	while(  tl )
	{
		next = tl->next;
		free(tl);
		tl=next; 
		deleteToken(tl->token);
		
	}
	
	return 1;
}

int tokenListDestroyKeepingStrings(TokenList tl) {
	TokenList next;
	if(tl == NULL)
		return 1;
	tl = tokenListGetFirst(tl);
	next = tl->next;

	while(  next != NULL )
	{
		tl=next; 
		next=tl->next;
		deleteKeepingStringToken(tl->token);
		free(tl);
	}
	
	return 1;
}

TokenList tokenListInsert(TokenList tl, Token t) {
	TokenList newNode = tokenListCreate(t);
	if(tl==NULL) {
		return newNode;
	}
	else {
		while(tl->next != NULL) {
			tl=tl->next;
		}
		newNode->previous = tl;
		tl->next = newNode;

		return newNode;
	}
}

int tokenListFind(TokenList tl, Token t) {
	tl = tokenListGetFirst(tl);
	for( ; tl != NULL; tl = tl->next ) { // safe
		if(!cmpToken(tl->token, t))
			return 1;
	}
	return 0;
}

void tokenListRemove(TokenList tl, Token t)
{
	TokenList oldNode;
	tl = tokenListGetFirst(tl);
	for( ; tl != NULL; tl = tl->next )	{ //safe
		if(!cmpToken(tl->token, t))	{
			oldNode = tl;
			tl->previous->next = tl->next;
			tl->next->previous = tl->previous;
			deleteToken(oldNode->token);
			free(oldNode);
		}
	}
}

TokenList tokenListNext( TokenList tl ) {
	if( tl == NULL) {
		return tl;
	}
	return tl->next;
}

TokenList tokenListPrevious( TokenList tl )
{
	if( tl == NULL) {
		return tl;
	}
	return tl->previous;
}

TokenList tokenListGetFirst(TokenList tl)
{
	if( tl == NULL) {
		return tl;
	}
	while(tl->previous != NULL) {
		tl=tl->previous;
	}
	return tl;
}

TokenList tokenListGetLast(TokenList tl)
{
	if( tl == NULL) {
		return tl;
	}
	while(tl->next != NULL) {
		tl=tl->next;
	}
	return tl;
}

TokenList tokenListExecuteForEach(TokenList tl, tokenCallBack func)
{
	tl = tokenListGetFirst(tl);//Safe
	for( ; tl != NULL; tl = tl->next ) {
		func(tl->token);
	}
	return tl;
}

Token tokenListGetCurrentToken(TokenList tl) {
	if(!tl)
		return NULL;
	return tl->token;
}

