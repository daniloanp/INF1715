%{
#include "token.h"

int done = 0;
int line_num = 1;

//Auxiliar Function that count how many times a char exists into a string
int countCharOccurences(char *s, char c) {
	int i=0;
	for( ; *s; s++ )
		if( *s == c )
			i++;
	return i;
}

%}

STR_CHAR [^\n"\\]
ESCAPED_CHAR \\[\\"nt]
COMMENT_LINE [/][/][^\n]*
COMMENT_BLOCK [/][*]([^*]*|[*]*[^/])[*][/]	
HEX_NUM [0][x](([abcdefABCDEF]|[0-9])+)
DEC_NUM [0-9]+
ALPHA [a-zA-Z]
ALNUM [a-zA-Z0-9]

%x comment

%%

"/*"		{ BEGIN(comment); }
<comment><<EOF>>	{ printf("Unclosed comment block .\n"); return ERROR;}
<comment>"*/"	   { BEGIN(INITIAL); }
<comment>[^*\n]*
<comment>[\n]	   { ++line_num; }
<comment>"*"

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
if		{ return IF;}
else	{ return ELSE;}
end		{ return END;}
while	{ return WHILE;}
loop	{ return LOOP;}
fun		{ return FUN;}
return	{ return RETURN;}
new		{ return NEW;}
string	{ return STRING;}
int		{ return INT;}
char	{ return CHAR;}
bool	{ return BOOL;}
and		{ return AND;}
or		{ return OR;}
not		{ return NOT;}
(true|false) { return BOOL_VAL;}

({DEC_NUM}|{HEX_NUM}) { 
	return INT_VAL; 
}

({ALPHA}|[_])({ALNUM}|[_])* {
	return IDENTIFIER; 
}




["]({STR_CHAR}|{ESCAPED_CHAR})*["] { 
	return STRING_VAL; 
}

{COMMENT_LINE} { }

[[:blank:]]* { }


\n([[:blank:]\n]*|{COMMENT_BLOCK}|{COMMENT_LINE})* {
	return NL;
}


. {
 return ERROR; 
}

%%4

yywrap() {
	done = 1;
}

