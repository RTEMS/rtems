#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=posix_users_new
DISTRIBUTION_LEVEL=public

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi

GENERATED_FILES= \
   adminiface.texi cancel.texi clock.texi cond.texi confspace.texi \
   cspecific.texi device.texi dumpcontrol.texi eventlog.texi files.texi \
   io.texi key.texi memorymgmt.texi message.texi mutex.texi procenv.texi \
   process.texi sched.texi semaphores.texi signal.texi systemdb.texi \
   thread.texi

FILES= posix_users.texi preface.texi \
  $(COMMON_FILES) $(GENERATED_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: dirs $(FILES) $(PROJECT)
#	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
	$(MAKEINFO) posix_users.texi

dvi: $(PROJECT).dvi
ps: dirs $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) posix_users.texi
	mv posix_users.dvi $(PROJECT).dvi

html: dirs $(FILES)
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	-cd .. ; gmake headers
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    posix_users.texi


clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* _* $(GENERATED_FILES)

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

eventlog.texi: eventlog.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

dumpcontrol.texi: dumpcontrol.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

confspace.texi: confspace.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

adminiface.texi: adminiface.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

systemdb.texi: systemdb.t Makefile
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" ${*}.t

