#ifndef REC_PARSER_H
#include "../trab1/tokenList.h"
#include "../trab1/token.h"
//TokenList program( TokenList tl );
typedef int (*callbackOnDerivation) ( 	Token t, int line );
int parser( TokenList tl, callbackOnDerivation f );


#endif //REC_PARSER_H

