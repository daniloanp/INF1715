#ifndef AST_H
#include "../trab1/token.h"


typedef struct struct_AST* AST;
typedef union ast_val* ASTNodeValue;

typedef enum {
	AST_IDENTIFIER = 1,
	AST_IF = 2,
	AST_ELSE = 3,
	//AST_END = 4,
	AST_WHILE = 5,
	AST_LOOP = 6,
	AST_FUN = 7,
	AST_RETURN = 8,
	AST_NEW = 9,
	AST_NL = 10,
	AST_INT = 11,
	AST_CHAR = 12,
	AST_BOOL = 13,
	AST_AND = 14,
	AST_OR = 15,
	AST_NOT = 16,
	AST_BOOL_VAL = 17,
	AST_INT_VAL = 18,
	AST_STRING = 19,
	AST_PLUS = 20,
	AST_MINUS = 21,
	AST_MUL = 22,
	AST_DIV = 23,
	
	AST_OP_BRACKET = 24,
	AST_CL_BRACKET = 25,

	AST_GREATER  = 30,
	AST_GREATER_EQUAL = 31,
	AST_LESS = 32,
	AST_LESS_EQUAL = 33,
	AST_EQUAL = 34,
	AST_DIFFERENT = 35,
	AST_STRING_VAL = 36,




	AST_program = 127,
	AST_attr  = 128,
	AST_expression  = 129,
	AST_call  = 131,
	AST_expressionList = 132,
	AST_declFunction = 133,
	AST_declGlobal = 134,

	//Begin TemporaryNodes, a complete AST shouldn't have any of them

	AST_decl = 255,
	AST_END = 256,

	

	AST_ERROR = 999
}	ASTNodeType;


#endif //AST_H


AST AST_NewNode( ASTNodeType type , int line, ASTNodeValue value );
int AST_NewNodeFromToken( Token t ); 
AST AST_insertChild( AST parent, AST child );
int AST_insertSibling( AST sibling, AST newSibling );
int AST_insertParentLowering( AST curr, AST newParent );





