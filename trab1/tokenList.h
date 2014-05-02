#ifndef TOKEN_LIST_H
#define TOKEN_LIST_H
#include "token.h"

typedef struct struct_token_list* TokenList;

typedef int (*tokenCallBack) (Token t);


//Unsafe! The below functions assume that Token instances can be "destroyed" when convenient
TokenList tokenListCreate(Token t);
int tokenListDestroy(TokenList tl);
int tokenListDestroyKeepingStrings(TokenList tl);

TokenList tokenListInsert(TokenList tl, Token t);
int tokenListFind(TokenList tl, Token t);
void tokenListRemove(TokenList tl, Token t);

Token tokenListGetCurrentToken(TokenList tl);
TokenList tokenListNext( TokenList tl );
TokenList tokenListPrevious( TokenList tl );
TokenList tokenListGetFirst(TokenList tl);
TokenList tokenListGetLast(TokenList tl);
TokenList tokenListExecuteForEach(TokenList tl, tokenCallBack func);

#endif // TOKEN_LIST_H