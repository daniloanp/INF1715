#ifndef REC_PARSER_H
#include <stdbool.h>
#include "../trab1/token_list.h"
#include "../trab1/token.h"


/*
The ENUM below represents any possibly derivation node type of the derivation of the parser.
ALL Terminals has the same number and name of the Token.
*/
typedef enum {
	NT_PROGRAM = 1,
	NT_DECL_FUNCTION = 2,
	NT_DECL_VAR = 3,
	NT_TYPE = 4,
	NT_BLOCK = 5,
	NT_DECL_OR_COMMAND = 6,
	NT_COMMAND = 7,
	NT_COMMAND_ATTR_OR_CALL = 8,
	NT_COMMAND_WHILE = 9,
	NT_COMMAND_RETURN = 10,
	NT_COMMAND_IF = 11,
	NT_DECL = 12,
	NT_ATTR = 13,
	NT_ARRAY_ACCESS = 14,
	NT_NEW = 15,
	NT_CALL = 16,
	NT_PARAMS = 17,
	NT_PARAM = 18,
	NT_EXPRESSION = 19,
	NT_EXPRESSION_LIST = 20,
	NT_VAL = 21,
	NT_UNARY_OP = 22,
	NT_MUL_DIV_OP = 23,
	NT_MIN_ADD_OP = 24,
	NT_COMPARISON_OP = 25,
	NT_AND = 26,
	NT_COMMAND_ELSE_IF = 27,
	NT_COMMAND_ELSE = 28,
} NonTerminal;

typedef int (*callbackOnDerivation) ( NonTerminal rule, Token t, int line );
bool parser( TokenList tl, callbackOnDerivation f );


#endif //REC_PARSER_H

