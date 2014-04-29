mini0-lex: lex.yy.c
	colorgcc -Wall -o mini0-lex lex.yy.c teste-lex.c lex.h lex.c token.h token.c tokenList.h tokenList.c

lex.yy.c: mini0-lex.l
	flex --header-file=lex.yy.h mini0-lex.l 

clean:
	rm -f mini0-lex *.o lex.yy.c lex.yy.h

test: mini0-lex
	@echo "Teste sem erros:"
	@echo
	@echo
	bash teste.sh	