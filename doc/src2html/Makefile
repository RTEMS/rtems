#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT=browseable_rtems

include ../Make.config

all: html info ps

dirs:
	$(make-dirs)

COMMON_FILES=../common/cpright.texi

FILES= clock.texi cond.texi key.texi mutex.texi $(PROJECT).texi preface.texi \
  sched.texi signal.texi thread.texi $(COMMON_FILES)

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: 

ps:

html: dirs
	-mkdir -p $(WWW_INSTALL)/$(PROJECT)
	$(SRC2HTML) -d3 RTEMS.src

RTEMS.src:
	sed -e 's/WWW_INSTALL/$(WWW_INSTALL)/' \
	    <RTEMS.test >RTEMS.src
	
clean:
	rm -f RTEMS.src
