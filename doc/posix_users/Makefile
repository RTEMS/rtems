#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=posix_users

include ../Make.config

all: html info ps pdf

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi

GENERATED_FILES= \
   cancel.texi clock.texi cond.texi \
   cspecific.texi device.texi files.texi \
   io.texi key.texi memorymgmt.texi message.texi mutex.texi procenv.texi \
   process.texi sched.texi semaphores.texi signal.texi status.texi \
   systemdb.texi thread.texi timer.texi libc.texi libm.texi

FILES= posix_users.texi preface.texi \
  $(COMMON_FILES) $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(FILES) $(PROJECT)
#	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) posix_users.texi

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
	$(TEXI2DVI) posix_users.texi
	cp $(PROJECT).dvi $(WWW_INSTALL)/$(PROJECT)

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	-cd .. ; gmake headers
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    posix_users.texi

clean:
	rm -f *.o $(PROG) *.txt core *.html $(PROJECT).pdf
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)
	rm -f *.cps *.cpss *.fns *.jkys *.pgs *.tps *.vrs

process.texi: process.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

procenv.texi: procenv.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

files.texi: files.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

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

timer.texi: timer.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

sched.texi: sched.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

io.texi: io.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

device.texi: device.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cspecific.texi: cspecific.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

semaphores.texi: semaphores.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

memorymgmt.texi: memorymgmt.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

message.texi: message.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

cancel.texi: cancel.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

systemdb.texi: systemdb.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

status.texi: status.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

libc.texi: libc.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t


libm.texi: libm.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

