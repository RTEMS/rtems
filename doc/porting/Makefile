#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=porting

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi
FILES=preface.texi developtools.texi sourcecode.texi cpumodels.texi \
  cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
  prioritybitmap.texi codetuning.texi miscellaneous.texi $(COMMON_FILES)

GENERATED_FILES=developtools.texi sourcecode.texi cpumodels.texi \
  cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
  prioritybitmap.texi codetuning.texi miscellaneous.texi


INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(PROJECT)
	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(PDF_INSTALL)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	# $(TEXI2DVI) $(PROJECT).texi
	texi2dvi -V $(PROJECT).texi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html *.pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)

developtools.texi: developtools.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "" ${*}.t

sourcecode.texi: sourcecode.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cpumodels.texi: cpumodels.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cpuinit.texi: cpuinit.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

interrupts.texi: interrupts.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

taskcontext.texi: taskcontext.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

idlethread.texi: idlethread.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

prioritybitmap.texi: prioritybitmap.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

codetuning.texi: codetuning.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

miscellaneous.texi: miscellaneous.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

