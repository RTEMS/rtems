#
#  COPYRIGHT (c) 1988-1999.
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

dvi: dirs $(PROJECT).dvi
ps: dirs $(PROJECT).ps
pdf: dirs $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)

developtools.texi: developtools.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Source Code Organization" ${*}.t

sourcecode.texi: sourcecode.t
	$(BMENU) -p "Development Tools" \
	    -u "Top" \
	    -n "CPU Model Variations" ${*}.t

cpumodels.texi: cpumodels.t
	$(BMENU) -p "Source Code Organization Introduction" \
	    -u "Top" \
	    -n "CPU Initialization" ${*}.t

cpuinit.texi: cpuinit.t
	$(BMENU) -p "CPU Model Variations Coding Issues" \
	    -u "Top" \
	    -n "Interrupts" ${*}.t

interrupts.texi: interrupts.t
	$(BMENU) -p "CPU Initialization Initializing the CPU" \
	    -u "Top" \
	    -n "Task Context Management" ${*}.t

taskcontext.texi: taskcontext.t
	$(BMENU) -p "Interrupts Pointer to _Thread_Dispatch Routine" \
	    -u "Top" \
	    -n "IDLE Thread" ${*}.t

idlethread.texi: idlethread.t
	$(BMENU) -p "Task Context Management Restoring a Floating Point Context" \
	    -u "Top" \
	    -n "Priority Bitmap Manipulation" ${*}.t

prioritybitmap.texi: prioritybitmap.t
	$(BMENU) -p "IDLE Thread Idle Thread Body" \
	    -u "Top" \
	    -n "Code Tuning Parameters" ${*}.t

codetuning.texi: codetuning.t
	$(BMENU) -p "Priority Bitmap Manipulation Bit Scan Support" \
	    -u "Top" \
	    -n "Miscellaneous" ${*}.t

miscellaneous.texi: miscellaneous.t
	$(BMENU) -p "Code Tuning Parameters Partition Element Alignment" \
	    -u "Top" \
	    -n "Command and Variable Index" ${*}.t

