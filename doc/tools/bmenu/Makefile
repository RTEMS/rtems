#
#  COPYRIGHT (c) 1996.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

CC=gcc
#CFLAGS=-O4 -fomit-frame-pointer
CFLAGS=-g 

#TEXINPUTS=/home/gnu/work/binutils-2.6/texinfo:.  
PROG=bmenu

all:  $(PROG)

$(BASE).txt: $(BASE).d ./$(PROG)
	./$(PROG) $(BASE).d

$(PROG): main.o chain.o
	gcc main.o chain.o -o $(PROG)

main.o: main.c base.h

chain.o: chain.c 

info: c_user.texinfo timer.texi
	makeinfo c_user.texinfo

TESTER=init
test: all
	#rm -f timer.txt
	#./bmenu -v timer.texi
	#cp ../user/$(TESTER).texi .
	./bmenu -u "DOCS UP" -p "DOCS PREV" -n "DOCS NEXT" -v testdoc.txt
	#mv $(TESTER).txt $(TESTER).texi
	#makeinfo c_user.texinfo


clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f c_user c_user-* _*

