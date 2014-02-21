typedef struct *struct_token Token




typedef enum{
	KEYWORD = 2,
	IDENTIFIER = 4,
	BOOLEAN = 8,
	NUMBER = 16,
	CHAR = 32,
	STRING = 64,
	COMMENT_BLOCK = 128,
	COMMENT_LINE = 256,
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


Token new_token(TokenKind t, int line, void* value );
int tokenGetLine(Token t);
int tokenKind(Token t);
void* tokenGetVal(Token t);
int deleteToken(Token t);




