#ifndef REC_PARSER_H
#include "../trab1/tokenList.h"
#include "../trab1/token.h"
//TokenList program( TokenList tl );
typedef int (*callbackOnDerivation) (int nodeType, int line, Token t);
int parser( TokenList tl, callbackOnDerivation f );


#endif //REC_PARSER_H

