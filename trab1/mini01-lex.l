%{
#define KEYWORD 2
#define IDENTIFIER 4
#define STRING 8
#define COMMENT_BLOCK 16
#define COMMENT_LINE 32
#define ERROR 999

int done = 0;

%}

%%




\"([^\n"\\]|\\["\\])*\" { return STRING; }
[a-zA-Z_][a-zA-Z_0-9]*  {return IDENTIFIER;}



([^ \t\n]*)     { return ERROR; }
([ \t\n]*)	{ }

%%

yywrap() {
	done = 1;
}

int main() {
	while (!done) {
		printf("%d\n", yylex());
		printf("%s\n", yytext);
	}
	return 0;
}
