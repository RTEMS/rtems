#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=posix_users

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi

GENERATED_FILES= \
   cancel.texi clock.texi cond.texi \
   cspecific.texi device.texi files.texi \
   io.texi key.texi memorymgmt.texi message.texi mutex.texi procenv.texi \
   process.texi sched.texi semaphores.texi signal.texi status.texi \
   systemdb.texi thread.texi timer.texi libc.texi libm.texi

FILES= preface.texi

info_TEXINFOS = posix_users.texi
posix_users_TEXINFOS = $(FILES) $(GENERATED_FILES)

process.texi: process.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

procenv.texi: procenv.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

files.texi: files.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

thread.texi: thread.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

signal.texi: signal.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

mutex.texi: mutex.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

cond.texi: cond.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

key.texi: key.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

clock.texi: clock.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

timer.texi: timer.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

sched.texi: sched.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

io.texi: io.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

device.texi: device.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

cspecific.texi: cspecific.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

semaphores.texi: semaphores.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

memorymgmt.texi: memorymgmt.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

message.texi: message.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

cancel.texi: cancel.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

systemdb.texi: systemdb.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

status.texi: status.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

libc.texi: libc.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<


libm.texi: libm.t
	 $(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

noinst_SCRIPTS = gen_size_report
EXTRA_DIST = *.t gen_size_report

