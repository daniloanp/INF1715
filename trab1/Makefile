mini01-lex: lex.yy.c
	gcc -o mini01-lex lex.yy.c

lex.yy.c: mini01-lex.l
	flex mini01-lex.l

clean:
	rm -f mini01-lex *.o lex.yy.c

test: mini01-lex
	@echo "Teste sem erros:"
	./mini01-lex < teste_ok.txt
	@echo "Teste com erros:"
	./mini01-lex < teste_erro.txt
