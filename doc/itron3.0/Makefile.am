#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#


PROJECT = itron
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = status.texi task.texi tasksync.texi semaphore.texi \
    eventflags.texi mailbox.texi msgbuffer.texi rendezvous.texi \
    interrupt.texi memorypool.texi fixedblock.texi time.texi config.texi \
    network.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = itron.texi
itron_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

status.texi: status.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

task.texi: task.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

tasksync.texi: tasksync.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

semaphore.texi: semaphore.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

eventflags.texi: eventflags.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

mailbox.texi: mailbox.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

msgbuffer.texi: msgbuffer.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

rendezvous.texi: rendezvous.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

interrupt.texi: interrupt.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

memorypool.texi: memorypool.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

fixedblock.texi: fixedblock.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

time.texi: time.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

config.texi: config.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

network.texi: network.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

noinst_SCRIPTS = gen_all gen_section gen_status_shell

EXTRA_DIST = config.t eventflags.t fixedblock.t interrupt.t mailbox.t \
    memorypool.t msgbuffer.t network.t rendezvous.t semaphore.t status.t \
    task.t tasksync.t time.t $(noinst_SCRIPTS)

CLEANFILES += itron.info itron.info-?
