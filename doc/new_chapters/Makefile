#
#  COPYRIGHT (c) 1996.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#

include ../Make.config

PROJECT=posix
DOCNAME=posix_test_plan

all:  

COMMON_FILES=../common/cpright.texi
FILES= clock.texi cond.texi key.texi mutex.texi $(DOCNAME).texi preface.texi \
  sched.texi signal.texi thread.texi $(COMMON_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(DOCNAME)
#	cp $(DOCNAME) $(DOCNAME)-* $(INFO_INSTALL)
	cp $(DOCNAME) $(INFO_INSTALL)

posix_test_plan: $(FILES)
	$(MAKEINFO) $(DOCNAME).texi

vinfo: info
	$(INFO) -f ./$(DOCNAME)

dvi: $(DOCNAME).dvi
ps: $(DOCNAME).ps
	
$(DOCNAME).ps: $(DOCNAME).dvi
	dvips -o $(DOCNAME).ps $(DOCNAME).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

dv: dvi
	$(XDVI) $(DOCNAME).dvi

view: ps
	$(GHOSTVIEW) $(DOCNAME).ps

$(DOCNAME).dvi: $(FILES)
	$(TEXI2DVI) $(DOCNAME).texi

html:
	-mkdir $(WWW_INSTALL)/$(DOCNAME)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/c_$(PROJECT) \
 	    $(PROJECT).texi
	cp ../rtems.html $(WWW_INSTALL)


clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(DOCNAME) $(DOCNAME)-* _*

