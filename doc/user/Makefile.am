#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=c_user

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi

FILES= bsp.texi dirstat.texi example.texi glossary.texi preface.texi

GENERATED_FILES=overview.texi concepts.texi init.texi task.texi \
    intr.texi clock.texi timer.texi sem.texi msg.texi \
    event.texi signal.texi part.texi region.texi \
    dpmem.texi io.texi fatal.texi schedule.texi rtmon.texi \
    bsp.texi userext.texi conf.texi mp.texi 

info_TEXINFOS = c_user.texi
c_user_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)   

PNG_FILES = rtemsarc.png rtemspie.png states.png
if USE_HTML
html_project_DATA += $(PNG_FILES)
endif

#preface.texi: preface.t
#	$(BMENU) -p "Top" \
#	    -u "Top" \
#	    -n "Overview" $<

overview.texi: overview.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Key Concepts" $<

concepts.texi: concepts.t
	$(BMENU) -p "Overview Manual Organization" \
	    -u "Top" \
	    -n "Initialization Manager" $<

init.texi: init.t
	$(BMENU) -p "Key Concepts Memory Management" \
	    -u "Top" \
	    -n "Task Manager" $<

task.texi: task.t
	$(BMENU) -p "Initialization Manager SHUTDOWN_EXECUTIVE - Shutdown RTEMS" \
	    -u "Top" \
	    -n "Interrupt Manager" $<

intr.texi: intr.t
	$(BMENU) -p "Task Manager TASK_VARIABLE_DELETE - Remove per task variable" \
	    -u "Top" \
	    -n "Clock Manager" $<

clock.texi: clock.t
	$(BMENU) \
	 -p "Interrupt Manager INTERRUPT_IS_IN_PROGRESS - Is an ISR in Progress" \
	    -u "Top" \
	    -n "Timer Manager" $<

timer.texi: timer.t
	$(BMENU) -p "Clock Manager CLOCK_TICK - Announce a clock tick" \
	    -u "Top" \
	    -n "Semaphore Manager" $<

sem.texi: sem.t
	$(BMENU) -p "Timer Manager TIMER_RESET - Reset an interval timer" \
	    -u "Top" \
	    -n "Message Manager" $<

msg.texi: msg.t
	$(BMENU) -p "Semaphore Manager SEMAPHORE_FLUSH - Unblock all tasks waiting on a semaphore" \
	    -u "Top" \
	    -n "Event Manager" $<

event.texi: event.t
	$(BMENU) \
	-p "Message Manager MESSAGE_QUEUE_FLUSH - Flush all messages on a queue" \
	    -u "Top" \
	    -n "Signal Manager" $<

signal.texi: signal.t
	$(BMENU) -p "Event Manager EVENT_RECEIVE - Receive event condition" \
	    -u "Top" \
	    -n "Partition Manager" $<

part.texi: part.t
	$(BMENU) -p "Signal Manager SIGNAL_SEND - Send signal set to a task" \
	    -u "Top" \
	    -n "Region Manager" $<

region.texi: region.t
	$(BMENU) \
-p "Partition Manager PARTITION_RETURN_BUFFER - Return buffer to a partition" \
	    -u "Top" \
	    -n "Dual-Ported Memory Manager" $<

dpmem.texi: dpmem.t
	$(BMENU) \
	  -p "Region Manager REGION_GET_SEGMENT_SIZE - Obtain size of a segment" \
	    -u "Top" \
	    -n "I/O Manager" $<

io.texi: io.t
	$(BMENU) -p "Dual-Ported Memory Manager PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address" \
	    -u "Top" \
	    -n "Fatal Error Manager" $<

fatal.texi: fatal.t
	$(BMENU) -p "I/O Manager IO_CONTROL - Special device services" \
	    -u "Top" \
	    -n "Scheduling Concepts" $<

schedule.texi: schedule.t
	$(BMENU) \
-p "Fatal Error Manager FATAL_ERROR_OCCURRED - Invoke the fatal error handler" \
	    -u "Top" \
	    -n "Rate Monotonic Manager" $<

rtmon.texi: rtmon.t
	$(BMENU) -p "Scheduling Concepts Task State Transitions" \
	    -u "Top" \
	    -n "Board Support Packages" $<

bsp.texi: bsp.t
	$(BMENU) -p "Rate Monotonic Manager RATE_MONOTONIC_GET_STATUS - Obtain status information on period" \
	    -u "Top" \
	    -n "User Extensions Manager" $<

userext.texi: userext.t
	$(BMENU) -p "Board Support Packages Heterogeneous Systems" \
	    -u "Top" \
	    -n "Configuring a System" $<

conf.texi: conf.t
	$(BMENU) -p "User Extensions Manager EXTENSION_DELETE - Delete a extension set" \
	    -u "Top" \
	    -n "Multiprocessing Manager" $<

mp.texi: mp.t
	$(BMENU) -p "Configuring a System Sizing the RTEMS RAM Workspace" \
	    -u "Top" \
	    -n "Directive Status Codes" $<

noinst_DATA = \
states.gif states.eps \
rtemspie.gif rtemspie.eps

EXTRA_DIST = *.t $(PNG_FILES) $(noinst_DATA)
