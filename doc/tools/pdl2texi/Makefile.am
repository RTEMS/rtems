#
#  COPYRIGHT (c) 1996-2000.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROG = pdl2texi
noinst_PROGRAMS = $(PROG)

#$(BASE).texi: $(BASE).d ./$(PROG)
#	./$(PROG) $(BASE).d

pdl2texi_SOURCES = address.inl chain.h main.c system.h base.h chain.inl \
    address.h chain.c isr.h

BASE = sample

#drive: Drive
#Drive: $(BASE).txt
#	/usr1/home/joel/tmp/makeinfo $(PROJECT).texi

#dvi: $(PROJECT).dvi
#ps: $(PROJECT).ps
#info: Drive
#html: $(PROJECT).texi $(BASE).txt
#	 ../textools/texi2html $(PROJECT).texi

$(BASE).txt: $(BASE).d
	./$(PROG) -v -p "Turret Subsystem" -u "Turret Subsystem" $(BASE).d

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
	rm -f *.o $(PROG) *.txt core *.html $(PROJECT) Drive.texi
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f test_cases/*.txt test_cases/*.texi

EXTRA_DIST = test_cases/enum.d test_cases/enumbad.d test_cases/table.d s.d \
    sample.d
