#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

PROJECT = c_user
EDITION = 1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = bsp.texi dirstat.texi example.texi glossary.texi preface.texi

GENERATED_FILES = overview.texi concepts.texi datatypes.texi init.texi \
    task.texi intr.texi clock.texi timer.texi sem.texi msg.texi event.texi \
    signal.texi part.texi region.texi dpmem.texi io.texi fatal.texi \
    schedule.texi rtmon.texi bsp.texi userext.texi conf.texi mp.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = c_user.texi
c_user_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

PNG_FILES = rtemsarc.png rtemspie.png states.png

EPS_IMAGES = rtemspie.eps states.eps
$(PROJECT).dvi: $(EPS_IMAGES)

PDF_IMAGES = rtemspie.pdf states.pdf
CLEANFILES += $(PDF_IMAGES)

if USE_HTML
html_project_DATA += $(PNG_FILES)
endif

overview.texi: overview.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Key Concepts" < $< > $@

concepts.texi: concepts.t
	$(BMENU2) -p "Overview Manual Organization" \
	    -u "Top" \
	    -n "RTEMS Data Types" < $< > $@

datatypes.texi: datatypes.t
	$(BMENU2) -p "Key Concepts Memory Management" \
	    -u "Top" \
	    -n "Initialization Manager" < $< > $@

init.texi: init.t
	$(BMENU2) -p "RTEMS Data Types List of Data Types" \
	    -u "Top" \
	    -n "Task Manager" < $< > $@

task.texi: task.t
	$(BMENU2) -p "Initialization Manager SHUTDOWN_EXECUTIVE - Shutdown RTEMS" \
	    -u "Top" \
	    -n "Interrupt Manager" < $< > $@

intr.texi: intr.t
	$(BMENU2) -p "Task Manager TASK_VARIABLE_DELETE - Remove per task variable" \
	    -u "Top" \
	    -n "Clock Manager" < $< > $@

clock.texi: clock.t
	$(BMENU2) \
	 -p "Interrupt Manager INTERRUPT_IS_IN_PROGRESS - Is an ISR in Progress" \
	    -u "Top" \
	    -n "Timer Manager" < $< > $@

timer.texi: timer.t
	$(BMENU2) -p "Clock Manager CLOCK_TICK - Announce a clock tick" \
	    -u "Top" \
	    -n "Semaphore Manager" < $< > $@

sem.texi: sem.t
	$(BMENU2) -p "Timer Manager TIMER_RESET - Reset an interval timer" \
	    -u "Top" \
	    -n "Message Manager" < $< > $@

msg.texi: msg.t
	$(BMENU2) -p "Semaphore Manager SEMAPHORE_FLUSH - Unblock all tasks waiting on a semaphore" \
	    -u "Top" \
	    -n "Event Manager" < $< > $@

event.texi: event.t
	$(BMENU2) \
	-p "Message Manager MESSAGE_QUEUE_FLUSH - Flush all messages on a queue" \
	    -u "Top" \
	    -n "Signal Manager" < $< > $@

signal.texi: signal.t
	$(BMENU2) -p "Event Manager EVENT_RECEIVE - Receive event condition" \
	    -u "Top" \
	    -n "Partition Manager" < $< > $@

part.texi: part.t
	$(BMENU2) -p "Signal Manager SIGNAL_SEND - Send signal set to a task" \
	    -u "Top" \
	    -n "Region Manager" < $< > $@

region.texi: region.t
	$(BMENU2) \
-p "Partition Manager PARTITION_RETURN_BUFFER - Return buffer to a partition" \
	    -u "Top" \
	    -n "Dual-Ported Memory Manager" < $< > $@

dpmem.texi: dpmem.t
	$(BMENU2) \
	  -p "Region Manager REGION_GET_SEGMENT_SIZE - Obtain size of a segment" \
	    -u "Top" \
	    -n "I/O Manager" < $< > $@

io.texi: io.t
	$(BMENU2) -p "Dual-Ported Memory Manager PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address" \
	    -u "Top" \
	    -n "Fatal Error Manager" < $< > $@

fatal.texi: fatal.t
	$(BMENU2) -p "I/O Manager IO_CONTROL - Special device services" \
	    -u "Top" \
	    -n "Scheduling Concepts" < $< > $@

schedule.texi: schedule.t
	$(BMENU2) \
-p "Fatal Error Manager FATAL_ERROR_OCCURRED - Invoke the fatal error handler" \
	    -u "Top" \
	    -n "Rate Monotonic Manager" < $< > $@

rtmon.texi: rtmon.t
	$(BMENU2) -p "Scheduling Concepts Task State Transitions" \
	    -u "Top" \
	    -n "Board Support Packages" < $< > $@

bsp.texi: bsp.t
	$(BMENU2) -p "Rate Monotonic Manager RATE_MONOTONIC_GET_STATUS - Obtain status information on period" \
	    -u "Top" \
	    -n "User Extensions Manager" < $< > $@

userext.texi: userext.t
	$(BMENU2) -p "Board Support Packages Heterogeneous Systems" \
	    -u "Top" \
	    -n "Configuring a System" < $< > $@

conf.texi: conf.t
	$(BMENU2) -p "User Extensions Manager EXTENSION_DELETE - Delete a extension set" \
	    -u "Top" \
	    -n "Multiprocessing Manager" < $< > $@

mp.texi: mp.t
	$(BMENU2) -p "Configuring a System Sizing the RTEMS RAM Workspace" \
	    -u "Top" \
	    -n "Directive Status Codes" < $< > $@

noinst_DATA = states.gif rtemspie.gif

EXTRA_DIST = bsp.t concepts.t clock.t datatypes.t conf.t dpmem.t event.t \
    fatal.t init.t intr.t io.t mp.t msg.t overview.t part.t region.t rtmon.t \
    sem.t schedule.t signal.t task.t timer.t userext.t $(PNG_FILES) \
    $(EPS_IMAGES) $(noinst_DATA)

CLEANFILES += c_user.info c_user.info-? c_user.info-??

