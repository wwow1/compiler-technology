# part of the makefile

CFLAGS = -O0 -g

all: lrparser.tab.c lex.yy.c ast.c  genllvm.c
	gcc -o calc $(CFLAGS) lrparser.tab.c lex.yy.c ast.c genllvm.c

lex.yy.c: lrlex.l
	flex lrlex.l

lrparser.tab.c: lrparser.y
	bison -d lrparser.y

clean:
	$(RM) *.o *.exe lrparser.tab.c lrparser.tab.h lex.yy.c