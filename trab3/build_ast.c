#include <stdlib.h>
#include <stdio.h>
#include "../trab1/lex.h"
#include "../trab1/tokenList.h"
#include "../trab2/recursiveParser.h"
#include "AbstractSyntaxTree.h"



int buildAst(int nodeType, int line, Token t) {
	static AST currParent = NULL;

	if(AST_getType(currParent) == AST_while || AST_getType(currParent) == AST_if ||	AST_getType(currParent) == AST_declFunction) {
		if((ASTNodeType)nodeType == AST_END) {
			currParent=currParent->parent;
		}
	}


	return 1;
}

int main( int argc, char **argv ) {	
	int ret;
	TokenList tl;
	FILE *input;
	++argv, --argc;
	if ( argc > 0 )
		input = fopen( argv[0], "r" );
	else
		input = stdin;
	if(input) {
		tl = generateTokens(input, &ret);
		if(!ret)
			ret = parser(tl, buildAst);

		if( input != stdin )
			fclose(input);
	}
	else {
		"Cannot open file";
		return 1;
	}

	if(ret == 0 )
		printf("Correct Syntax!!!\n");


	return ret;
}