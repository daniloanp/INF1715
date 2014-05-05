#ifndef TOKEN_H
#define TOKEN_H
#include <stdbool.h>

typedef struct struct_token* Token;

typedef enum {
	TK_IDENTIFIER = 1,
	TK_IF = 2,
	TK_ELSE = 3,
	TK_END = 4,
	TK_WHILE = 5,
	TK_LOOP = 6,
	TK_FUN = 7,
	TK_RETURN = 8,
	TK_NEW = 9,
	TK_NL = 10,
	TK_INT = 11,
	TK_CHAR = 12,
	TK_BOOL = 13,
	TK_AND = 14,
	TK_OR = 15,
	TK_NOT = 16,
	TK_BOOL_VAL = 17,
	TK_INT_VAL = 18,
	TK_STRING = 19,
	TK_PLUS = 20,
	TK_MINUS = 21,
	TK_MUL = 22,
	TK_DIV = 23,
	TK_OP_BRACKET = 24,
	TK_CL_BRACKET = 25,
	TK_OP_PARENTHESIS = 26,
	TK_CL_PARENTHESIS = 27,
	TK_COMMA = 28,
	TK_COLON = 29,
	TK_GREATER  = 30,
	TK_GREATER_EQUAL = 31,
	TK_LESS = 32,
	TK_LESS_EQUAL = 33,
	TK_EQUAL = 34,
	TK_DIFFERENT = 35,
	TK_STRING_VAL = 36,
	TK_ERROR = 999
} TokenKind;

/*TokenValue createTokenStringValue( char* str );
TokenValue createTokenNumberValue( long number );
*/

Token Token_New ( TokenKind t, long line );
Token Token_NewAsString (  TokenKind t, unsigned long line, char* val );
Token Token_NewAsInteger ( TokenKind t, unsigned long line, unsigned long val );
Token Token_NewAsBool ( TokenKind t, unsigned long line, bool val );

int Token_GetLine( Token t );
char* Token_GetStringValue( Token t ); //Only aplicable to INDETIFIER and STRING_VAL, otherwise it will through an exception
long Token_GetNumberValue( Token t ); //Only aplicable to INT_VAL, otherwise it will through an exception
long Token_GetBoolValue( Token t ); //Only aplicable to INT_VAL, otherwise it will through an exception


int Token_IsEqual( void* e0, void* e1 );
int Token_Free( Token t ); // Keep value Untouched(if it is an String);
int Token_Destroy( Token t ); // Free token as well string, if it's kind is either STRING_VAL or IDENTIFIER. 
char* Token_GetStringForKind( Token t ); //

char* TokenKind_ToString( TokenKind tk );
TokenKind Token_GetKind( Token t );



#endif // TOKEN_H



