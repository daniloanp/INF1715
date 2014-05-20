CC=gcc
CFLAGS=-std=c99 -D_GNU_SOURCE -ggdb
+CFLAGS=-Wall

mini0-lex: lex.yy.c
	$(CC) $(CFLAGS) -Wall -o mini0-lex main.c lex.yy.c mini0-lex.h mini0-lex.c token.h token.c token_list.h token_list.c

lex.yy.c: mini0-lex.l
	flex --header-file=lex.yy.h mini0-lex.l 

clean:
	rm -f mini0-lex *.o lex.yy.h lex.yy.c

test: mini0-lex
	bash -c "source ../iterate_directory.sh; iterate_directory mini0-lex testes"