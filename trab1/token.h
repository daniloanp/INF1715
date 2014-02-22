typedef struct struct_token* Token;

typedef union token_val* TokenValue;


typedef enum{
	KEYWORD = 2,
	IDENTIFIER = 4,
	BOOLEAN = 8,
	NUMBER = 16,
	CHAR = 32,
	STRING = 64,
	PLUS = '+',
	MINUS = '-',
	MUL = '*',
	DIV = '/',
	NL = '\n',
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
Token newToken(TokenKind t, long line, TokenValue value );
int tokenGetLine(Token t);
TokenKind tokenKind(Token t);
TokenValue tokenGetVal(Token t);
int deleteToken(Token t);
int cmpToken(void* e0, void* e1);




