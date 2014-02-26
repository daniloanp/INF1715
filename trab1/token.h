#ifndef TOKEN_H
#define TOKEN_H

typedef struct struct_token* Token;

typedef union token_val* TokenValue;

typedef enum{
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
	BOOLEAN = 17,
	NUMBER = 18,
	STRING = 19,
	CHARACTER = 20,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '/',
	OP_BRACKET = '[',
	CL_BRACKET = ']',
	OP_PARENTHESIS = '(',
	CL_PARENTHESIS = ')',
	COMMA = ',',
	COLON = ':',
	GREATER  = '>',
	GREATER_EQUAL = '>'*10,
	LESS = ('<'),
	LESS_EQUAL = ('<'*10),
	EQUAL = '=',
	DIFFERENT = '^',
	ERROR = 999
} TokenKind;

TokenValue createTokenStringValue(char* str);
TokenValue createTokenNumberValue(long number);
TokenValue createTokenCharValue(char c);
Token newToken(TokenKind t, long line, TokenValue value );
int tokenGetLine(Token t);
TokenKind tokenGetKind(Token t);
TokenValue tokenGetVal(Token t);
char tokenGetCharValue(Token t);
char* tokenGetStringValue(Token t);
long tokenGetNumberValue(Token t);
int deleteToken(Token t);
int cmpToken(void* e0, void* e1);

#endif // TOKEN_H



