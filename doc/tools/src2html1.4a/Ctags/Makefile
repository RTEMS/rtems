#	@(#)Makefile	5.6 (Berkeley) 5/11/90

CC=gcc

PROG=	ctags-new
CFLAGS+=-I. -g

SRCS=	C.c ctags.c fortran.c lisp.c print.c tree.c yacc.c strerror.c

ctags: C.o ctags.o fortran.o lisp.o print.o tree.o yacc.o strerror.o
	$(CC) -o ctags-new C.o ctags.o fortran.o lisp.o print.o tree.o yacc.o \
	strerror.o

clean:
	rm -f *.o ctags-new
