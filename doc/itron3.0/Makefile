#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=itron

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=

GENERATED_FILES= \
    status.texi \
    task.texi tasksync.texi semaphore.texi eventflags.texi mailbox.texi \
    msgbuffer.texi rendezvous.texi interrupt.texi memorypool.texi \
    fixedblock.texi time.texi config.texi network.texi

FILES= $(PROJECT).texi \
  $(COMMON_FILES) preface.texi $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(FILES) $(PROJECT)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
pdf: $(PROJECT).pdf
	
$(PROJECT).pdf: $(FILES)
	$(TEXI2PDF) $(PROJECT).texi
	cp $(PROJECT).pdf $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(WWW_INSTALL)/$(PROJECT)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: $(FILES)
	-mkdir -p html
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES) $(PROJECT).pdf
	rm -rf html

status.texi: status.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

task.texi: task.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

tasksync.texi: tasksync.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

semaphore.texi: semaphore.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

eventflags.texi: eventflags.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

mailbox.texi: mailbox.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

msgbuffer.texi: msgbuffer.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

rendezvous.texi: rendezvous.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

interrupt.texi: interrupt.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

memorypool.texi: memorypool.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

fixedblock.texi: fixedblock.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

time.texi: time.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

config.texi: config.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

network.texi: network.t Makefile
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

