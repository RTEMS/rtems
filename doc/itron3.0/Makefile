#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=itron
TEXI2DVI=../tools/texi2dvi
MAKEINFO=makeinfo
TEXI2WWW=../tools/texi2www
BMENU=../tools/bmenu
TEXI2WWW_ARGS=-dirfile index.html \
          -header ../tools/rtems_header.html \
          -footer ../tools/rtems_footer.html \
          -icons  .


all: html info ps

COMMON_FILES=

GENERATED_FILES= \
    task.texi tasksync.texi semaphore.texi eventflags.texi mailbox.texi \
    msgbuffer.texi rendezvous.texi interrupt.texi memorypool.texi \
    fixedblock.texi time.texi config.texi network.texi

FILES= $(PROJECT).texi \
  $(COMMON_FILES) $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(FILES) $(PROJECT)

$(PROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html: $(FILES)
	-mkdir -p html
	cp $(wildcard ../tools/*.gif) html
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir html $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)
	rm -rf html

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

