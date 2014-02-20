%{
#define KEYWORD 2
#define IDENTIFIER 4
#define	NUMBER 8
#define	CHAR 16
#define STRING 32

#define COMMENT_BLOCK 64

#define COMMENT_LINE 128
#define ERROR 999

#define PLUS 43
#define MINUS 45
#define MUL 42
#define DIV 47
#define NL 10

int done = 0;



%}

CHAR [^\n"\\]
ESCAPED_CHAR \\[\\"nt]
COMMENT_LINE [/][/][^\n]*
COMMENT_BLOCK [/][*]([^*]*|[*]*[^/])[*][/]
HEX_NUM 0x[abcdefABCDEF0-9]+
DEC_NUM [0-9]+

%%

(if|else|end|while|loop|fun|return|new|string|int|char|bool|true|false|and|or|not) {
 return KEYWORD; 
}

[[:alpha:]_][[:alnum:]_]* {
	return IDENTIFIER; 
}		

([\n][[:blank:]\n]*|[[:blank:]\n]*[\n]) { 
	return NL; 
}

({DEC_NUM}|{HEX_NUM}) { 
	return NUMBER; 
}

["]({CHAR}|{ESCAPED_CHAR})*["] { 
	return STRING; 
}
'({CHAR}|{ESCAPED_CHAR})' { 
	return CHAR; 
}

([[:blank:]]*|{COMMENT_BLOCK}|{COMMENT_LINE}) { 

}

. {
 return ERROR; 
}

%%4

yywrap() {
	done = 1;
}

int main() {
	while (!done) {
		int b =  yylex();
		if(b==999&&b!=0)
			printf("ERROR: ");
		if(b!=0)
		printf("%s::%d\n", yytext, b);
	}
	return 0;
}