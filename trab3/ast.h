#ifndef AST_H
#define AST_H
#include "../trab1/token.h"
#include <stdbool.h>
#include "../trab4/symboltype.h"

typedef struct struct_AST* AST;
typedef union ast_val ASTNodeValue;

typedef enum {
	AST_IF = 2,
	AST_ELSE = 3,
	AST_WHILE = 5,
	AST_RETURN = 8,
	AST_NEW = 9,
	AST_INT = 11,
	AST_CHAR = 12,
	AST_BOOL = 13,
	AST_STRING = 19, // ORDER
	AST_AND = 14,
	AST_OR = 15,
	AST_NOT = 16,
	AST_BOOL_VAL = 17,
	AST_INT_VAL = 18,
	AST_STRING_VAL = 36, // ORDER
	AST_PLUS = 20,
	AST_MINUS = 21,
	AST_MUL = 22,
	AST_DIV = 23,
	AST_GREATER  = 30,
	AST_GREATER_EQUAL = 31,
	AST_LESS = 32,
	AST_LESS_EQUAL = 33,
	AST_EQUAL = 34,
	AST_DIFFERENT = 35,
	AST_DECL_FUNCTION = 131,
	AST_PARAM = 133,
	AST_PARAM_LIST = 134,
	AST_EXPRESSION = 142,
	AST_CALL = 144,
	AST_ATTR = 145,
	AST_BLOCK = 150,
	AST_PROGRAM = 152,
	AST_DECL_VAR = 154,
	AST_VAR = 156,
	AST_UNARYMINUS = 157,
	AST_ELSE_IF = 258

} ASTNodeType;

AST AST_New( ASTNodeType type , int line );	
AST AST_NewAsBool( ASTNodeType type , int line, bool val );	
AST AST_NewAsString( ASTNodeType type , int line, char* val );	
AST AST_NewAsInt( ASTNodeType type , int line, unsigned long val );	

AST AST_InsertChild( AST parent, AST child );
AST AST_InsertSibling( AST sibling, AST newSibling );
AST AST_InsertNewChildParentOfChildren( AST curr, AST newParent );
AST AST_GetParent(AST node);

AST AST_GetFirstChild(AST node);
AST AST_GetLastChild(AST node);
AST AST_GetNextSibling(AST node);
AST AST_GetPrevSibling(AST node);
ASTNodeValue AST_GetNodeValue(AST node);
ASTNodeValue AST_NodeValueFromToken( Token t );
ASTNodeValue AST_NodeValueFromInt( unsigned int v);
AST AST_UpdateNodeValue( AST node, ASTNodeValue val );
AST AST_UpdateNodeType( AST node, ASTNodeType tp );
AST AST_RemoveChild( AST parent, AST child ) ;
bool AST_IsNodeAConstant( AST node );
bool AST_IsNodeABinOp( AST node );
bool AST_IsBaseNodeType( AST node );
ASTNodeType AST_GetType( AST node );


char* AST_GetStringValue(AST node);
unsigned long AST_GetIntValue(AST node);
unsigned int AST_GetLine( AST node );
SymbolType AST_GetSymType( AST node );
void AST_SetSymType( AST node, SymbolType tp );
bool AST_IsOperatorNode( AST node );
void AST_Free(AST t);





void AST_PrettyPrint( AST t, int level );





#endif //AST_H
