#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=posix_users

all:  

COMMON_FILES=../common/cpright.texi
FILES= clock.texi cond.texi key.texi mutex.texi $(PROJECT).texi preface.texi \
  sched.texi signal.texi thread.texi $(COMMON_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(PROJECT)
#	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

posix_users: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f ./$(PROJECT)

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

dv: dvi
	$(XDVI) $(PROJECT).dvi

view: ps
	$(GHOSTVIEW) $(PROJECT).ps

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html:
	-mkdir $(WWW_INSTALL)/$(PROJECT)
	-cd .. ; gmake headers
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi


clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _*

