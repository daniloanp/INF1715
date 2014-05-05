#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H
#include "token.h"

typedef struct struct_token_list* TokenList;

typedef int (*tokenCallBack) (Token t);


//Unsafe! The below functions assume that Token instances can be "destroyed" when convenient
TokenList TokenList_New(Token t);


int TokenList_Free(TokenList tl);
int TokenList_Destroy(TokenList tl);

TokenList TokenList_Insert(TokenList tl, Token t);
int TokenList_Find(TokenList tl, Token t);
void TokenList_Remove(TokenList tl, Token t);

Token TokenList_SetCurrentToken(TokenList tl);
Token TokenList_GetCurrentToken(TokenList tl);

TokenList TokenList_Next( TokenList tl );
TokenList TokenList_Previous( TokenList tl );
TokenList TokenList_GetFirst(TokenList tl);
TokenList TokenList_GetLast(TokenList tl);
TokenList TokenList_ExecuteForEach(TokenList tl, tokenCallBack func);

#endif // TOKEN_LIST_H