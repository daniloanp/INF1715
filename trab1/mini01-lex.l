%{
#include "token.h"

int done = 0;

%}

CHAR [^\n"\\]
ESCAPED_CHAR \\[\\"nt]
COMMENT_LINE [/][/][^\n]*
COMMENT_BLOCK [/][*]([^*]*|[*]*[^/])[*][/]	
HEX_NUM 0x[abcdefABCDEF0-9]+
DEC_NUM [0-9]+

%%

[+] 	{ return PLUS;}
[-] 	{ return MINUS;}
[/] 	{ return DIV;}
[*] 	{ return MUL;}
[(] 	{ return OP_PARENTHESIS;}
[)] 	{ return CL_PARENTHESIS;}
[,] 	{ return COMMA;}
[:] 	{ return COLON;}
[>] 	{ return GREATER;}
[<]		{ return LESS;}
[>][=] 	{ return GREATER_EQUAL;}
[<][=] 	{ return LESS_EQUAL;}
[=] 	{ return EQUAL;}
[<][>] 	{ return DIFFERENT;}
[[] 	{ return OP_BRACKET;}
[]] 	{ return CL_BRACKET;}



(if|else|end|while|loop|fun|return|new|string|int|char|bool|and|or|not) {
 return KEYWORD; 
}

(true|false) { return BOOLEAN;}

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