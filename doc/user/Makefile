#
#  COPYRIGHT (c) 1996.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=c_user

all:  

COMMON_FILES=../common/cpright.texi
FILES= bsp.texi c_user.texi clock.texi concepts.texi conf.texi \
dirstat.texi dpmem.texi event.texi example.texi fatal.texi \
glossary.texi init.texi intr.texi io.texi mp.texi msg.texi overview.texi \
part.texi preface.texi region.texi rtmon.texi schedule.texi sem.texi \
signal.texi task.texi timer.texi userext.texi $(COMMON_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: c_user
	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)

c_user: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f $(PROJECT)

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
	-mkdir $(WWW_INSTALL)/c_user
	cp rtemsarc.gif rtemspie.gif states.gif $(WWW_INSTALL)/c_user
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f c_user c_user-* _*

