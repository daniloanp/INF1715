
CC=colorgcc
CFLAGS=-std=c99 -D_GNU_SOURCE -g -Wall -Werror

PROGRAM=backend
OBJECTS=main.o ir.o basicblock.o symboltable.o irdump.o

all: $(PROGRAM)

$(PROGRAM): grammar.tab.c lexer.lex.c $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM) grammar.tab.c lexer.lex.c  $(OBJECTS)

grammar.tab.c: grammar.y
	bison -t -v grammar.y --defines=grammar.tab.h

lexer.lex.c: lexer.l grammar.tab.c
	flex -o lexer.lex.c lexer.l

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

ir.o: ir.c
	$(CC) $(CFLAGS) -c ir.c

basicblock.o: basicblock.c
	$(CC) $(CFLAGS) -c basicblock.c

symboltable.o: symboltable.c
	$(CC) $(CFLAGS) -c symboltable.c


irdump.o: irdump.c
	$(CC) $(CFLAGS) -c irdump.c

cov:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -fprofile-arcs -ftest-coverage" all

clean:
	rm -f core *.gcov *.gcda *.gcno *.tab.* *.lex.* *.output *.gch *.dot *.o $(PROGRAM)


