#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=c_user

all:  

COMMON_FILES=../common/cpright.texi
FILES= bsp.texi c_user.texi clock.texi concepts.texi conf.texi \
dirstat.texi dpmem.texi event.texi example.texi fatal.texi \
glossary.texi init.texi intr.texi io.texi mp.texi msg.texi overview.texi \
part.texi preface.texi region.texi rtmon.texi schedule.texi sem.texi \
signal.texi task.texi timer.texi userext.texi $(COMMON_FILES)

GENERATED_FILES=overview.texi concepts.texi init.texi task.texi \
    intr.texi clock.texi timer.texi sem.texi msg.texi \
    event.texi signal.texi part.texi region.texi \
    dpmem.texi io.texi fatal.texi schedule.texi rtmon.texi \
    bsp.texi userext.texi conf.texi mp.texi 

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: c_user
	cp $(PROJECT) $(PROJECT)-* $(INFO_INSTALL)

c_user: $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f $(PROJECT)

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

dv: dvi
	$(XDVI) $(PROJECT).dvi

view: ps
	$(GHOSTVIEW) $(PROJECT).ps

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

html:
	-mkdir $(WWW_INSTALL)/c_user
	cp rtemsarc.gif rtemspie.gif states.gif $(WWW_INSTALL)/c_user
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core *.html 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f c_user c_user-* _* $(GENERATED_FILES)

#preface.texi: preface.t
#	$(BMENU) -p "Top" \
#	    -u "Top" \
#	    -n "Overview" ${*}.t

overview.texi: overview.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Key Concepts" ${*}.t

concepts.texi: concepts.t
	$(BMENU) -p "Overview Manual Organization" \
	    -u "Top" \
	    -n "Initialization Manager" ${*}.t

init.texi: init.t
	$(BMENU) -p "Key Concepts Memory Management" \
	    -u "Top" \
	    -n "Task Manager" ${*}.t

task.texi: task.t
	$(BMENU) -p "Initialization Manager SHUTDOWN_EXECUTIVE - Shutdown RTEMS" \
	    -u "Top" \
	    -n "Interrupt Manager" ${*}.t

intr.texi: intr.t
	$(BMENU) -p "Task Manager TASK_WAKE_WHEN - Wake up when specified" \
	    -u "Top" \
	    -n "Clock Manager" ${*}.t

clock.texi: clock.t
	$(BMENU) \
	 -p "Interrupt Manager INTERRUPT_IS_IN_PROGRESS - Is an ISR in Progress" \
	    -u "Top" \
	    -n "Timer Manager" ${*}.t

timer.texi: timer.t
	$(BMENU) -p "Clock Manager CLOCK_TICK - Announce a clock tick" \
	    -u "Top" \
	    -n "Semaphore Manager" ${*}.t

sem.texi: sem.t
	$(BMENU) -p "Timer Manager TIMER_RESET - Reset an interval timer" \
	    -u "Top" \
	    -n "Message Manager" ${*}.t

msg.texi: msg.t
	$(BMENU) -p "Semaphore Manager SEMAPHORE_RELEASE - Release a semaphore" \
	    -u "Top" \
	    -n "Event Manager" ${*}.t

event.texi: event.t
	$(BMENU) \
	-p "Message Manager MESSAGE_QUEUE_FLUSH - Flush all messages on a queue" \
	    -u "Top" \
	    -n "Signal Manager" ${*}.t

signal.texi: signal.t
	$(BMENU) -p "Event Manager EVENT_RECEIVE - Receive event condition" \
	    -u "Top" \
	    -n "Partition Manager" ${*}.t

part.texi: part.t
	$(BMENU) -p "Signal Manager SIGNAL_SEND - Send signal set to a task" \
	    -u "Top" \
	    -n "Region Manager" ${*}.t

region.texi: region.t
	$(BMENU) \
-p "Partition Manager PARTITION_RETURN_BUFFER - Return buffer to a partition" \
	    -u "Top" \
	    -n "Dual-Ported Memory Manager" ${*}.t

dpmem.texi: dpmem.t
	$(BMENU) \
	  -p "Region Manager REGION_GET_SEGMENT_SIZE - Obtain size of a segment" \
	    -u "Top" \
	    -n "I/O Manager" ${*}.t

io.texi: io.t
	$(BMENU) -p "Dual-Ported Memory Manager PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address" \
	    -u "Top" \
	    -n "Fatal Error Manager" ${*}.t

fatal.texi: fatal.t
	$(BMENU) -p "I/O Manager IO_CONTROL - Special device services" \
	    -u "Top" \
	    -n "Scheduling Concepts" ${*}.t

schedule.texi: schedule.t
	$(BMENU) \
-p "Fatal Error Manager FATAL_ERROR_OCCURRED - Invoke the fatal error handler" \
	    -u "Top" \
	    -n "Rate Monotonic Manager" ${*}.t

rtmon.texi: rtmon.t
	$(BMENU) -p "Scheduling Concepts Task State Transitions" \
	    -u "Top" \
	    -n "Board Support Packages" ${*}.t

bsp.texi: bsp.t
	$(BMENU) -p "Rate Monotonic Manager RATE_MONOTONIC_GET_STATUS - Obtain status information on period" \
	    -u "Top" \
	    -n "User Extensions Manager" ${*}.t

userext.texi: userext.t
	$(BMENU) -p "Board Support Packages Heterogeneous Systems" \
	    -u "Top" \
	    -n "Configuring a System" ${*}.t

conf.texi: conf.t
	$(BMENU) -p "User Extensions Manager EXTENSION_DELETE - Delete a extension set" \
	    -u "Top" \
	    -n "Multiprocessing Manager" ${*}.t

mp.texi: mp.t
	$(BMENU) -p "Configuring a System Sizing the RTEMS RAM Workspace" \
	    -u "Top" \
	    -n "Directive Status Codes" ${*}.t

