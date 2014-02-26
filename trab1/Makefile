mini01-lex: lex.yy.c
	gcc -o mini01-lex lex.yy.c main.c token.h token.c tokenList.h tokenList.c

lex.yy.c: mini01-lex.l
	flex --header-file=lex.yy.h mini01-lex.l 

clean:
	rm -f mini01-lex *.o lex.yy.c lex.yy.h

test: mini01-lex
	@echo "Teste sem erros:"
	@echo
	@echo
	./mini01-lex teste_ok.txt
	@echo
	@echo "==========="
	@echo
	@echo "Teste com erros:"
	@echo
	./mini01-lex teste_erro.txt
	
test-valgrind: mini01-lex
	@echo "Teste sem erros:"
	@echo
	@echo
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes  ./mini01-lex teste_ok.txt
	