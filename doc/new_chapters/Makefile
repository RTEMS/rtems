#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=posix_users
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi

GENERATED_FILES= clock.texi cond.texi key.texi mutex.texi \
  sched.texi signal.texi thread.texi

FILES= clock.texi cond.texi key.texi mutex.texi $(PROJECT).texi preface.texi \
  sched.texi signal.texi thread.texi $(COMMON_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(FILES) $(PROJECT)
#	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

posix_users: $(FILES)
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

thread.texi: thread.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t
signal.texi: signal.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

mutex.texi: mutex.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cond.texi: cond.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

key.texi: key.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

clock.texi: clock.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

sched.texi: sched.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

