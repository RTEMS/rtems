#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=new_chapters

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi

GENERATED_FILES= \
   adminiface.texi confspace.texi \
   dumpcontrol.texi eventlog.texi \
   stackchk.texi rtmonuse.texi cpuuse.texi error.texi monitor.texi

FILES= $(PROJECT).texi \
  $(COMMON_FILES) $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(FILES) $(PROJECT)
#	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	-cd .. ; gmake headers
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)

eventlog.texi: eventlog.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

dumpcontrol.texi: dumpcontrol.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

confspace.texi: confspace.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

adminiface.texi: adminiface.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

stackchk.texi: stackchk.t Makefile
	$(BMENU) -p "" \
            -u "Top" \
            -n "" ${*}.t

rtmonuse.texi: rtmonuse.t Makefile
	$(BMENU) -p "" \
            -u "Top" \
            -n "" ${*}.t

cpuuse.texi: cpuuse.t Makefile
	$(BMENU) -p "" \
            -u "Top" \
            -n "" ${*}.t

error.texi: error.t Makefile
	$(BMENU) -p "" \
            -u "Top" \
            -n "" ${*}.t

monitor.texi: monitor.t Makefile
	$(BMENU) -p "" \
            -u "Top" \
            -n "" ${*}.t
