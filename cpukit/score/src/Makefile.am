##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = mpci.c objectmp.c threadmp.c

CORE_MESSAGE_QUEUE_C_FILES = coremsg.c coremsgbroadcast.c coremsgclose.c \
    coremsgflush.c coremsgflushwait.c coremsginsert.c coremsgflushsupp.c \
    coremsgseize.c coremsgsubmit.c

CORE_MUTEX_C_FILES = coremutex.c coremutexflush.c coremutexseize.c \
    coremutexsurrender.c

CORE_SEMAPHORE_C_FILES = coresem.c coresemflush.c coresemseize.c \
    coresemsurrender.c

HEAP_C_FILES = heap.c heapallocate.c heapextend.c heapfree.c \
    heapsizeofuserarea.c heapwalk.c heapgetinfo.c

OBJECT_C_FILES = object.c objectallocate.c objectallocatebyindex.c \
    objectclearname.c objectcomparenameraw.c objectcomparenamestring.c \
    objectcopynameraw.c objectcopynamestring.c objectextendinformation.c \
    objectfree.c objectget.c objectgetisr.c objectgetbyindex.c \
    objectgetnext.c objectinitializeinformation.c objectnametoid.c \
    objectshrinkinformation.c

THREAD_C_FILES = thread.c threadchangepriority.c threadclearstate.c \
    threadclose.c threadcreateidle.c threaddelayended.c threaddispatch.c \
    threadevaluatemode.c threadget.c threadhandler.c threadidlebody.c \
    threadinitialize.c threadloadenv.c threadready.c threadresettimeslice.c \
    threadreset.c threadrestart.c threadresume.c threadrotatequeue.c \
    threadsetpriority.c threadsetstate.c threadsettransient.c \
    threadstackallocate.c threadstackfree.c threadstart.c \
    threadstartmultitasking.c threadsuspend.c threadtickletimeslice.c \
    threadyieldprocessor.c

THREADQ_C_FILES = threadq.c threadqdequeue.c threadqdequeuefifo.c \
    threadqdequeuepriority.c threadqenqueue.c threadqenqueuefifo.c \
    threadqenqueuepriority.c threadqextract.c threadqextractfifo.c \
    threadqextractpriority.c threadqextractwithproxy.c threadqfirst.c \
    threadqfirstfifo.c threadqfirstpriority.c threadqflush.c \
    threadqtimeout.c

TOD_C_FILES = coretod.c coretodset.c coretodtickle.c coretodtoseconds.c \
    coretodvalidate.c

WATCHDOG_C_FILES = watchdog.c watchdogadjust.c watchdoginsert.c \
    watchdogremove.c watchdogtickle.c

STD_C_FILES = apiext.c chain.c $(CORE_MESSAGE_QUEUE_C_FILES) \
    $(CORE_MUTEX_C_FILES) $(CORE_SEMAPHORE_C_FILES) $(HEAP_C_FILES) interr.c \
    isr.c $(OBJECT_C_FILES) $(THREAD_C_FILES) $(THREADQ_C_FILES) \
    $(TOD_C_FILES) userext.c $(WATCHDOG_C_FILES) wkspace.c

if HAS_MP
C_FILES = $(STD_C_FILES) $(MP_C_FILES)
else
C_FILES = $(STD_C_FILES)
endif
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} ${OBJS}

EXTRA_DIST = $(STD_C_FILES) $(MP_C_FILES) Unlimited.txt

include $(top_srcdir)/../../../automake/local.am
