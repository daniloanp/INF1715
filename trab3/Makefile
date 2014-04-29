mini0-ast:
	cd ../trab1/ && rm -f lex.yy.c && $(MAKE) lex.yy.c
	cd ../trab2/
	colorgcc -Wall -o mini0-parser build_ast.c  AbstractSyntaxTree.h AbstractSyntaxTree.c ../trab2/recursiveParser.h ../trab2/recursiveParser.c ../trab1/token.h ../trab1/token.c ../trab1/tokenList.h ../trab1/tokenList.c ../trab1/lex.yy.h ../trab1/lex.yy.c ../trab1/lex.h ../trab1/lex.c 
clean:
	rm -f mini0-parser *.o

test: mini0-ast
	bash test.sh
test-oficial: mini0-ast
	bash test-oficial.sh