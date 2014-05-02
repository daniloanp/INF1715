#ifndef TOKEN_H
#define TOKEN_H

typedef struct struct_token* Token;

typedef union token_val* TokenValue;

typedef enum {
	IDENTIFIER = 1,
	IF = 2,
	ELSE = 3,
	END = 4,
	WHILE = 5,
	LOOP = 6,
	FUN = 7,
	RETURN = 8,
	NEW = 9,
	NL = 10,
	INT = 11,
	CHAR = 12,
	BOOL = 13,
	AND = 14,
	OR = 15,
	NOT = 16,
	BOOL_VAL = 17,
	INT_VAL = 18,
	STRING = 19,
	PLUS = 20,
	MINUS = 21,
	MUL = 22,
	DIV = 23,
	OP_BRACKET = 24,
	CL_BRACKET = 25,
	OP_PARENTHESIS = 26,
	CL_PARENTHESIS = 27,
	COMMA = 28,
	COLON = 29,
	GREATER  = 30,
	GREATER_EQUAL = 31,
	LESS = 32,
	LESS_EQUAL = 33,
	EQUAL = 34,
	DIFFERENT = 35,
	STRING_VAL = 36,
	ERROR = 999
} TokenKind;

TokenValue createTokenStringValue(char* str);
TokenValue createTokenNumberValue(long number);
TokenValue createTokenCharValue(char c);
Token newToken(TokenKind t, long line, TokenValue value );
int tokenGetLine(Token t);
TokenKind tokenGetKind(Token t);
TokenValue tokenGetVal(Token t);
char* tokenGetStringValue(Token t);
long tokenGetNumberValue(Token t);
int deleteToken(Token t);
int cmpToken(void* e0, void* e1);
char* tokenToString(Token t);
char* tokenKindToString(TokenKind tk);

int deleteKeepingStringToken(Token t);


#endif // TOKEN_H



