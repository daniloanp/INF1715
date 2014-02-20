%{
#define KEYWORD 2
#define IDENTIFIER 4


#define STRING 8
#define COMMENT_BLOCK 16
#define COMMENT_LINE 32
#define ERROR 999



#define PLUS 43
#define MINUS 45
#define ASTERISK 42
#define SLASH 47
#define NL 10

int done = 0;

%}
%%




\"([^\n"\\]|\\["\\])*\" { return STRING; }


(if|else|end|while|loop|fun|return|new|string|int|char|bool|true|false|and|or|not) { return KEYWORD; }

[[:alpha:]_][[:alnum:]_]*  { return IDENTIFIER; }


([\n][ \t\n]*|[ \t\n]*[\n]) { return NL; }

([ \t]*)	{ }
.     { return ERROR; }

%%4

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
