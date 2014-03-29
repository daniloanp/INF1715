mini01-lex: lex.yy.c
	gcc -o mini01-lex lex.yy.c teste-lex.c lex.h lex.c token.h token.c tokenList.h tokenList.c

lex.yy.c: mini01-lex.l
	flex --header-file=lex.yy.h mini01-lex.l 

clean:
	rm -f mini01-lex *.o lex.yy.c lex.yy.h

test: mini01-lex
	@echo "Teste sem erros:"
	@echo
	@echo
	bash teste.sh	