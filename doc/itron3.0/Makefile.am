#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.5

PROJECT = itron
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

COMMON_FILES = $(top_srcdir)/common/setup.texi

GENERATED_FILES = status.texi task.texi tasksync.texi semaphore.texi \
    eventflags.texi mailbox.texi msgbuffer.texi rendezvous.texi \
    interrupt.texi memorypool.texi fixedblock.texi time.texi config.texi \
    network.texi

FILES = preface.texi

info_TEXINFOS = itron.texi
itron_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

$(srcdir)/status.texi: status.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/task.texi: task.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/tasksync.texi: tasksync.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/semaphore.texi: semaphore.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/eventflags.texi: eventflags.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/mailbox.texi: mailbox.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/msgbuffer.texi: msgbuffer.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/rendezvous.texi: rendezvous.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/interrupt.texi: interrupt.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/memorypool.texi: memorypool.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/fixedblock.texi: fixedblock.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/time.texi: time.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/config.texi: config.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

$(srcdir)/network.texi: network.t
	$(BMENU2) -p "" \
	    -u "Top" \
	    -n "" < $< > $@

noinst_SCRIPTS = gen_all gen_section gen_status_shell

EXTRA_DIST = config.t eventflags.t fixedblock.t interrupt.t mailbox.t \
    memorypool.t msgbuffer.t network.t rendezvous.t semaphore.t status.t \
    task.t tasksync.t time.t $(noinst_SCRIPTS)
