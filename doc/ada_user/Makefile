#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=ada_user

include ../Make.config

all: html info ps pdf

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

LINKFILES=../user/rtemspie.eps ../user/rtemspie.pdf \
          ../user/states.eps ../user/states.pdf 

dirs: links
	$(make-dirs)

links:
	cp  $(LINKFILES) .

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	cp $(shell ls -1 $(PROJECT) $(PROJECT)-* 2>/dev/null) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: dirs $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).ps: dirs $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	cp ../user/rtemsarc.gif ../user/rtemspie.gif ../user/states.gif \
           $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

# This is one of the few document directories without its own eps and pdf
# files.  Thus it can safely remove *.pdf and *.eps.
clean:
	rm -f *.o $(PROG) *.txt core *.html *.pdf *.eps
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _*

