#
#  COPYRIGHT (c) 1996-1997.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

CC=gcc
#CFLAGS=-O4 -fomit-frame-pointer
CFLAGS=-g 

TEXINPUTS=/home/gnu/work/binutils-2.6/texinfo:.  
PROG=pdl2texi

all:  $(PROG)

$(BASE).texi: $(BASE).d ./$(PROG)
	./$(PROG) $(BASE).d

$(PROG): main.o chain.o
	gcc -static main.o chain.o -o $(PROG)

drive: Drive
Drive: $(BASE).txt
	/usr1/home/joel/tmp/makeinfo $(PROJECT).texi

main.o: main.c base.h

chain.o: chain.c 

dvi: dirs $(PROJECT).dvi
ps: $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
info: Drive
html: $(PROJECT).texi $(BASE).txt
	 ../textools/texi2html $(PROJECT).texi

$(BASE).txt: $(BASE).d
	./$(PROG) -v -p "Turret Subsystem" -u "Turret Subsystem" $(BASE).d

$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

view:
	test -r $(PROJECT).ps && ghostview $(PROJECT).ps

tests: test test1

test: $(PROG)
	rm -f $(BASE).texi
	./$(PROG) -v -p "Turret Subsystem" -u "Turret Subsystem" $(BASE).d

test1:
	rm -f t1.txt
	./$(PROG) -v t1.d

wtest:
	rm -f $(BASE).txt
	./$(PROG) -w -v -p "Turret Subsystem" -u "Turret Subsystem" $(BASE).d

enum:
	./$(PROG) -w -v -p "Magic Subsystem" \
	    -u "Magic Subsystem" test_cases/enum.d
	./$(PROG) -w -v -p "Magic Subsystem" \
	    -u "Magic Subsystem" test_cases/enumbad.d
	
avdas:
	./$(PROG) -v -p "Magic Subsystem" \
	    -u "Magic Subsystem" test_cases/avdas.d
	./$(PROG) -w -v -p "Magic Subsystem" \
	    -u "Magic Subsystem" test_cases/avdas.d
	
clean:
	rm -f *.o $(PROG) *.txt core *.html $(PROJECT) Drive.texi $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f test_cases/*.txt test_cases/*.texi

