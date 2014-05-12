%option nounput
%option noinput
%{
#include "token.h"
int done = 0;
int line_num = 1;
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
<comment><<EOF>>	{ printf("Error: Unclosed comment block .\n"); return TK_ERROR;}
<comment>"*/"	   { BEGIN(INITIAL); }
<comment>[^*\n]*
<comment>[\n]	   { ++line_num; }
<comment>"*"

[+] 	{ return TK_PLUS;}
[-] 	{ return TK_MINUS;}
[/] 	{ return TK_DIV;}
[*] 	{ return TK_MUL;}
[(] 	{ return TK_OP_PARENTHESIS;}
[)] 	{ return TK_CL_PARENTHESIS;}
[,] 	{ return TK_COMMA;}
[:] 	{ return TK_COLON;}
[>] 	{ return TK_GREATER;}
[<]		{ return TK_LESS;}
[>][=] 	{ return TK_GREATER_EQUAL;}
[<][=] 	{ return TK_LESS_EQUAL;}
[=] 	{ return TK_EQUAL;}
[<][>] 	{ return TK_DIFFERENT;}
[[] 	{ return TK_OP_BRACKET;}
[]] 	{ return TK_CL_BRACKET;}
if		{ return TK_IF;}
else	{ return TK_ELSE;}
end		{ return TK_END;}
while	{ return TK_WHILE;}
loop	{ return TK_LOOP;}
fun		{ return TK_FUN;}
return	{ return TK_RETURN;}
new		{ return TK_NEW;}
string	{ return TK_STRING;}
int		{ return TK_INT;}
char	{ return TK_CHAR;}
bool	{ return TK_BOOL;}
and		{ return TK_AND;}
or		{ return TK_OR;}
not		{ return TK_NOT;}
(true|false) { return TK_BOOL_VAL;}

({DEC_NUM}|{HEX_NUM}) {
	return TK_INT_VAL; 
}

({ALPHA}|[_])({ALNUM}|[_])* {
	return TK_IDENTIFIER; 
}



["]({STR_CHAR}|{ESCAPED_CHAR})*["] { 
	return TK_STRING_VAL; 
}

{COMMENT_LINE} { }

[[:blank:]]* { }


[\n]([[:blank:]\n]*|{COMMENT_BLOCK}|{COMMENT_LINE})* {
	return TK_NL;
}


. {
 return TK_ERROR; 
}

%%4

int yywrap(void) {
	done = 1;
	return 1;
}

