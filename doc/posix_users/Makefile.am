#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = posix_users

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = cancel.texi clock.texi cond.texi cspecific.texi \
    device.texi files.texi io.texi key.texi memorymgmt.texi message.texi \
    mutex.texi procenv.texi process.texi sched.texi semaphores.texi \
    signal.texi status.texi systemdb.texi thread.texi timer.texi libc.texi \
    libm.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = preface.texi
info_TEXINFOS = posix_users.texi
posix_users_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

process.texi: process.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

procenv.texi: procenv.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

files.texi: files.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

thread.texi: thread.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

signal.texi: signal.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

mutex.texi: mutex.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

cond.texi: cond.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

key.texi: key.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

clock.texi: clock.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

timer.texi: timer.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

sched.texi: sched.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

io.texi: io.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

device.texi: device.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

cspecific.texi: cspecific.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

semaphores.texi: semaphores.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

memorymgmt.texi: memorymgmt.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

message.texi: message.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

cancel.texi: cancel.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

systemdb.texi: systemdb.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

status.texi: status.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

libc.texi: libc.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

libm.texi: libm.t
	 $(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

noinst_SCRIPTS = gen_size_report
EXTRA_DIST = cancel.t clock.t cond.t cspecific.t device.t files.t io.t key.t \
    libc.t libm.t memorymgmt.t message.t mutex.t procenv.t process.t sched.t \
    semaphores.t signal.t status.t systemdb.t thread.t timer.t \
    gen_size_report

CLEANFILES += posix_users.info posix_users.info-? posix_users.info-??
