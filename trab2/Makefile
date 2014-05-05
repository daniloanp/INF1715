mini0-parser:
	cd ../trab1/ && rm -f lex.yy.c && $(MAKE) lex.yy.c
	cd ../trab2/
	colorgcc -Wall -o mini0-parser main.c recursive_parser.c recursive_parser.h ../trab1/token.h ../trab1/token.c ../trab1/token_list.h ../trab1/token_list.c ../trab1/lex.yy.h ../trab1/lex.yy.c ../trab1/mini0-lex.h ../trab1/mini0-lex.c 
clean:
	rm -f mini0-parser *.o
test: mini0-parser
	bash test.sh mini0-parser
