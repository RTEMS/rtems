#
#  COPYRIGHT (c) 1996.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=ada_user

all:  

COMMON_FILES=../common/cpright.texi ../user/bsp.texi ../user/clock.texi \
  ../user/concepts.texi ../user/conf.texi ../user/dirstat.texi \
  ../user/dpmem.texi ../user/event.texi \
  ../user/fatal.texi ../user/glossary.texi ../user/init.texi \
  ../user/intr.texi ../user/io.texi ../user/mp.texi ../user/msg.texi \
  ../user/overview.texi ../user/part.texi ../user/preface.texi \
  ../user/region.texi ../user/rtmon.texi ../user/schedule.texi \
  ../user/sem.texi ../user/signal.texi ../user/task.texi \
  ../user/timer.texi ../user/userext.texi 
FILES= ada_user.texi example.texi $(COMMON_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: ada_user
	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)

ada_user: $(FILES)
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
	-mkdir $(WWW_INSTALL)/ada_user
	cp ../user/rtemsarc.gif ../user/rtemspie.gif ../user/states.gif \
           $(WWW_INSTALL)/ada_user
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f ada_user ada_user-* _*

