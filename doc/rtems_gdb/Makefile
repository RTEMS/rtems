#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=rtems_gdb
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= \
    intro.t swarch.t started.t commands.t trouble.t example.t


FILES=$(PROJECT).texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	# cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	texi2dvi $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _* network.t

#
#  Process Automatically Generated Files
#

intro.texi: intro.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

swarch.texi: swarch.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

started.texi: started.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

commands.texi: commands.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

trouble.texi: trouble.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

example.texi: example.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

