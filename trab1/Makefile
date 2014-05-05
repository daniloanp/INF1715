

mini0-lex: lex.yy.c
	colorgcc -Wall -o mini0-lex main.c lex.yy.c mini0-lex.h mini0-lex.c token.h token.c token_list.h token_list.c

lex.yy.c: mini0-lex.l
	flex --header-file=lex.yy.h mini0-lex.l 

clean:
	rm -f mini0-lex *.o lex.yy.h lex.yy.c

test: mini0-lex
	@echo "Teste sem erros:"
	@echo
	@echo
	bash teste.sh	