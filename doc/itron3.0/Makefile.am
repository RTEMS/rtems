#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=itron

include $(top_srcdir)/project.am

COMMON_FILES=

GENERATED_FILES= \
    status.texi \
    task.texi tasksync.texi semaphore.texi eventflags.texi mailbox.texi \
    msgbuffer.texi rendezvous.texi interrupt.texi memorypool.texi \
    fixedblock.texi time.texi config.texi network.texi

FILES= preface.texi

info_TEXINFOS = itron.texi
itron_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

status.texi: status.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

task.texi: task.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

tasksync.texi: tasksync.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

semaphore.texi: semaphore.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

eventflags.texi: eventflags.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

mailbox.texi: mailbox.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

msgbuffer.texi: msgbuffer.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

rendezvous.texi: rendezvous.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

interrupt.texi: interrupt.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

memorypool.texi: memorypool.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

fixedblock.texi: fixedblock.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

time.texi: time.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

config.texi: config.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

network.texi: network.t
	$(BMENU) -p "" \
	    -u "Top" \
	    -n "" $<

noinst_SCRIPTS = gen_all gen_section gen_status_shell

EXTRA_DIST = *.t $(noinst_SCRIPTS)
