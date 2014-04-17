#ifndef AST_H
#include "../trab1/token.h"


typedef struct struct_AST* AST;
typedef union ast_val* ASTNodeValue;

typedef enum {
	AST_If = 2,
	AST_Else = 3,
	AST_While = 5,
	AST_Return = 8,
	AST_New = 9,
	AST_Int = 11,
	AST_Char = 12,
	AST_Bool = 13,
	AST_String = 19, //Without Order
	AST_And = 14,
	AST_Or = 15,
	AST_Not = 16,
	AST_BoolVal = 17,
	AST_IntVal = 18,
	AST_StringVal = 36, //Without Order
	AST_Plus = 20,
	AST_Minus = 21,
	AST_Mul = 22,
	AST_Div = 23,
	AST_Greater  = 30,
	AST_GreaterEqual = 31,
	AST_Less = 32,
	AST_LessEqual = 33,
	AST_Equal = 34,
	AST_Different = 35,
	AST_DeclFunction = 131,
	AST_Param = 133,
	AST_ParamList = 134,
	AST_Expression = 142,
	AST_Call = 144,
	AST_Attr = 145,
	AST_Block = 150,
	AST_Program = 152,
	AST_DeclVar = 154,
	AST_Var = 156,
	AST_UnaryMinus = 157,
	AST_ElseIf = 258,

}	ASTNodeType;

AST AST_NewNode( ASTNodeType type , int line, ASTNodeValue value );	
AST AST_InsertChild( AST parent, AST child );
AST AST_InsertSibling( AST sibling, AST newSibling );
AST AST_InsertNewChildParentOfChildren( AST curr, AST newParent );
ASTNodeType AST_getType(AST node);
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








void AST_prettyPrint( AST t, int level );





#endif //AST_H
