#ifndef AST_H
#include "../trab1/token.h"


typedef struct struct_AST* AST;
typedef union ast_val* ASTNodeValue;

typedef enum {
	AST_IDENTIFIER = 1,
	AST_If = 2,
	AST_ELSE = 3,
	//AST_END = 4,
	AST_While = 5,
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
	
	AST_COMMA = 28 ,//Temp,
	
	AST_GREATER  = 30,
	AST_GREATER_EQUAL = 31,
	AST_LESS = 32,
	AST_LESS_EQUAL = 33,
	AST_EQUAL = 34,
	AST_DIFFERENT = 35,
	
	AST_STRING_VAL = 36,
	AST_DeclGlobalVar = 128,
	AST_CommandWhile = 129,
	AST_CommandIf = 130,
	AST_DeclFunction = 131,
	AST_Type = 132,
	AST_Param = 133,
	AST_ParamList = 134,
	AST_DeclOrCommand = 135,
	AST_New = 136, 
	AST_Expression = 142,
	AST_ExpressionList = 143,
	AST_Call = 144,
	AST_Attr = 145,
	AST_ArrayAccess = 146,
	AST_CommandAttrOrCall = 147,
	AST_CommandReturn = 148,
	AST_Command = 149, 
	AST_Block = 150,
	AST_Decl = 151, 
	AST_Program = 152,

	AST_DeclVars = 153,
	AST_DeclVar = 154,
	AST_Commands = 155,
	AST_Var = 156,
	AST_UNARY_MINUS = 157,
	//Begin TemporaryNodes, a complete AST shouldn't have any of them

	AST_decl = 255,
	AST_END = 256,
	AST_ID = 257,
	

	AST_ERROR = 999
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





#endif //AST_H
