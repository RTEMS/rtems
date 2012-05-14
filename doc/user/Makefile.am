#
#  COPYRIGHT (c) 1988-2011
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = c_user

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = bsp.texi dirstat.texi example.texi glossary.texi preface.texi

GENERATED_FILES = overview.texi concepts.texi datatypes.texi init.texi \
    task.texi intr.texi clock.texi timer.texi sem.texi msg.texi event.texi \
    signal.texi part.texi region.texi dpmem.texi io.texi fatal.texi \
    schedule.texi rtmon.texi barrier.texi bsp.texi userext.texi conf.texi \
    mp.texi stackchk.texi cpuuse.texi object.texi chains.texi cbs.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = c_user.texi
c_user_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

TXT_FILES = rtemsarc.txt rtemspie.txt states.txt

PNG_FILES = rtemsarc.png rtemspie.png states.png ObjectId-32Bits.png \
    ObjectId-16Bits.png semaphore_attributes.png

EPS_IMAGES = rtemspie.eps states.eps ObjectId-32Bits.eps \
    ObjectId-32Bits.eps semaphore_attributes.eps
$(PROJECT).dvi: $(EPS_IMAGES)

PDF_IMAGES = rtemspie.pdf states.pdf ObjectId-16Bits.pdf \
    ObjectId-32Bits.pdf semaphore_attributes.pdf
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
	    -n "Scheduling Concepts" < $< > $@

schedule.texi: schedule.t
	$(BMENU2) -p "RTEMS Data Types List of Data Types" \
	    -u "Top" \
	    -n "Initialization Manager" < $< > $@

init.texi: init.t
	$(BMENU2) -p "Scheduling Concepts Task State Transitions" \
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
	    -n "Rate Monotonic Manager" < $< > $@

rtmon.texi: rtmon.t
	$(BMENU2) -p "Timer Manager TIMER_RESET - Reset an interval timer" \
	    -u "Top" \
	    -n "Semaphore Manager" < $< > $@

sem.texi: sem.t
	$(BMENU2) -p "Rate Monotonic Manager RATE_MONOTONIC_REPORT_STATISTICS - Print period statistics report" \
	    -u "Top" \
	    -n "Barrier Manager" < $< > $@

barrier.texi: barrier.t
	$(BMENU2) -p "Semaphore Manager SEMAPHORE_FLUSH - Unblock all tasks waiting on a semaphore" \
	    -u "Top" \
	    -n "Message Manager" < $< > $@

msg.texi: msg.t
	$(BMENU2) -p "Barrier Manager BARRIER_RELEASE - Release a barrier" \
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
	  -p "Region Manager REGION_RESIZE_SEGMENT - Change size of a segment" \
	    -u "Top" \
	    -n "I/O Manager" < $< > $@

io.texi: io.t
	$(BMENU2) -p "Dual-Ported Memory Manager PORT_INTERNAL_TO_EXTERNAL - Convert internal to external address" \
	    -u "Top" \
	    -n "Fatal Error Manager" < $< > $@

fatal.texi: fatal.t
	$(BMENU2) -p "I/O Manager IO_CONTROL - Special device services" \
	    -u "Top" \
	    -n "Board Support Packages" < $< > $@

bsp.texi: bsp.t
	$(BMENU2) -p "Fatal Error Manager FATAL_ERROR_OCCURRED - Invoke the fatal error handler" \
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
	$(BMENU2) -p "Configuring a System Unlimited Objects" \
	    -u "Top" \
	    -n "Stack Bounds Checker" < $< > $@

stackchk.texi: stackchk.t
	$(BMENU2) -p "Multiprocessing Manager MULTIPROCESSING_ANNOUNCE - Announce the arrival of a packet" \
	    -u "Top" \
	    -n "CPU Usage Statistics" < $< > $@

cpuuse.texi: cpuuse.t
	$(BMENU2) -p "Stack Bounds Checker STACK_CHECKER_REPORT_USAGE - Report Task Stack Usage" \
	    -u "Top" \
	    -n "Object Services" < $< > $@

object.texi: object.t
	$(BMENU2) -p "CPU Usage Statistics cpu_usage_reset - Reset CPU Usage Statistics" \
	    -u "Top" \
	    -n "Chains" < $< > $@

chains.texi: chains.t
	$(BMENU2) -p "Object Services OBJECT_GET_CLASS_INFORMATION - Obtain Class Information" \
	    -u "Top" \
	    -n "Constant Bandwidth Server Scheduler API" < $< > $@

cbs.texi: cbs.t
	$(BMENU2) -p "Chains Prepend a Node" \
	    -u "Top" \
	    -n "Directive Status Codes" < $< > $@

EXTRA_DIST = bsp.t cbs.t clock.t chains.t concepts.t cpuuse.t datatypes.t conf.t \
    dpmem.t event.t fatal.t init.t intr.t io.t mp.t msg.t overview.t \
    part.t region.t rtmon.t sem.t schedule.t signal.t stackchk.t \
    task.t timer.t userext.t $(TXT_FILES) $(PNG_FILES) $(EPS_IMAGES) \
    $(noinst_DATA)

CLEANFILES += c_user.info c_user.info-? c_user.info-??
