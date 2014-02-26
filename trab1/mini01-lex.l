%{
#include "token.h"

int done = 0;
int line_num = 1;
%}

CHAR [^\n"\\]
ESCAPED_CHAR \\[\\"nt]
COMMENT_LINE [/][/][^\n]*
COMMENT_BLOCK [/][*]([^*]*|[*]*[^/])[*][/]	
HEX_NUM [-+]?[0][x](([abcdefABCDEF]|[0-9])+)
DEC_NUM [-+]?[0-9]+

%x comment
%x nl
%%


"/*"		BEGIN(comment);

<comment>[^*\n]*
<comment>[^*\n]*\n	   ++line_num;
<comment>"*"+[^*/\n]*
<comment>"*"+[^*/\n]*\n ++line_num;
<comment>"*"+"/"	   BEGIN(INITIAL);


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
(true|false) { return BOOLEAN;}

({DEC_NUM}|{HEX_NUM}) { 
	return NUMBER; 
}

[[:alpha:]_][[:alnum:]_]* {
	return IDENTIFIER; 
}


["]({CHAR}|{ESCAPED_CHAR})*["] { 
	return STR; 
}

'({CHAR}|{ESCAPED_CHAR})' { 
	return CHARACTER; 
}
{COMMENT_LINE} { ++line_num;}

[[:blank:]]* { }


\n([[:blank:]\n]*|{COMMENT_BLOCK})* {
	return NL;
}


. {
 return ERROR; 
}

%%4

yywrap() {
	done = 1;
}

