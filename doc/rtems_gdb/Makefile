#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=rtems_gdb

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= \
    intro.texi swarch.texi started.texi commands.texi trouble.texi example.texi

FILES=$(PROJECT).texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	# cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: dirs $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _* network.t

#
#  Process Automatically Generated Files
#

intro.texi: intro.t Makefile
	$(BMENU) -p "Top" \
	    -u "Top" \
	    -n "Software Architecture" ${*}.t

swarch.texi: swarch.t Makefile
	$(BMENU) -p "Introduction" \
	    -u "Top" \
	    -n "Getting Started" ${*}.t

started.texi: started.t Makefile
	$(BMENU) -p "Software Architecture" \
	    -u "Top" \
	    -n "Commands" ${*}.t

commands.texi: commands.t Makefile
	$(BMENU) -p "Getting Started Attaching To The System" \
	    -u "Top" \
	    -n "Troubleshooting" ${*}.t

trouble.texi: trouble.t Makefile
	$(BMENU) -p "Commands Thread control" \
	    -u "Top" \
	    -n "Debug Session Example" ${*}.t

example.texi: example.t Makefile
	$(BMENU) -p "Troubleshooting Debug server not present" \
	    -u "Top" \
	    -n "" ${*}.t

