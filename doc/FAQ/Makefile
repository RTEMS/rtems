#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=FAQ

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= \
    basic.texi tools.texi concepts.texi debug.texi freesw.texi hwdebugaids.texi

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
	rm -f *.fixed _* 


#
#  Process Automatically Generated Files
#

basic.texi: basic.t Makefile
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

tools.texi: tools.t Makefile
	$(BMENU) -c -p "Top" \
	    -u "Top" \
	    -n "" ${*}.t

concepts.texi: concepts.t Makefile
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" ${*}.t

debug.texi: debug.t Makefile
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" ${*}.t

freesw.texi: freesw.t Makefile
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" ${*}.t


hwdebugaids.texi: hwdebugaids.t Makefile
	$(BMENU) -c -p "" \
	    -u "Top" \
	    -n "" ${*}.t

