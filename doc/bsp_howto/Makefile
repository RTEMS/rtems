#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=bsp_howto
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES= clock.texi console.texi init.texi intro.texi \
    linkcmds.texi makefiles.texi target.texi timer.texi

FILES= $(PROJECT).texi $(GENERATED_FILES)

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
	# cp $(PROJECT).ps $(PS_INSTALL)

# run texi2dvi twice to generate the xref's properly.
$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) -v $(PROJECT).texi
	texi2dvi -v $(PROJECT).texi

intro.texi: intro.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

target.texi: target.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

makefiles.texi: makefiles.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

linkcmds.texi: linkcmds.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

init.texi: init.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

clock.texi: clock.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

console.texi: console.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

timer.texi: timer.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

html: dirs $(FILES)
	@echo NOT BUILDING HTML
	#-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	#$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	#    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*

