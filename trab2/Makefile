mini0-parser:
	cd ../trab1/ && rm -f lex.yy.c && $(MAKE) lex.yy.c
	cd ../trab2/
	gcc -o mini0-parser test-parser.c recursiveParser.c recursiveParser.h ../trab1/token.h ../trab1/token.c ../trab1/tokenList.h ../trab1/tokenList.c ../trab1/lex.yy.h ../trab1/lex.yy.c ../trab1/lex.h ../trab1/lex.c 
clean:
	rm -f mini0-parser *.o

test: mini0-parser
	@echo "Teste sem erros:"
	@echo
	@echo
	bash test.sh	