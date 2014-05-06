+CC=colorgcc

+CFLAGS=-Wall
mini0-ast:
	cd ../trab1/ && rm -f lex.yy.c && $(MAKE) lex.yy.c
	cd ../trab2/
	$(CC) $(CFLAGS) -o mini0-parser main.c build_ast.h build_ast.c  ast.h ast.c ../trab2/recursive_parser.h ../trab2/recursive_parser.c ../trab1/token.h ../trab1/token.c ../trab1/token_list.h ../trab1/token_list.c ../trab1/lex.yy.h ../trab1/lex.yy.c ../trab1/mini0-lex.h ../trab1/mini0-lex.c 
clean:
	rm -f mini0-parser *.o

test: mini0-ast
	bash test.sh
test-oficial: mini0-ast
	bash test-oficial.sh