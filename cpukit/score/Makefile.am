##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

SUBDIRS = cpu

## include

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/debug.h include/rtems/system.h \
    include/rtems/seterr.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS = include/rtems/score/address.h \
    include/rtems/score/apiext.h include/rtems/score/apimutex.h \
    include/rtems/score/bitfield.h include/rtems/score/chain.h \
    include/rtems/score/context.h include/rtems/score/copyrt.h \
    include/rtems/score/corebarrier.h include/rtems/score/coremsg.h \
    include/rtems/score/coremutex.h include/rtems/score/corerwlock.h \
    include/rtems/score/coresem.h include/rtems/score/corespinlock.h \
    include/rtems/score/heap.h include/rtems/score/protectedheap.h \
    include/rtems/score/interr.h include/rtems/score/isr.h \
    include/rtems/score/object.h include/rtems/score/priority.h \
    include/rtems/score/stack.h include/rtems/score/states.h \
    include/rtems/score/sysstate.h include/rtems/score/thread.h \
    include/rtems/score/threadq.h include/rtems/score/threadsync.h \
    include/rtems/score/timespec.h include/rtems/score/tod.h \
    include/rtems/score/tqdata.h include/rtems/score/userext.h \
    include/rtems/score/watchdog.h include/rtems/score/wkspace.h \
    include/rtems/score/cpuopts.h

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += include/rtems/score/mpci.h \
    include/rtems/score/mppkt.h include/rtems/score/objectmp.h \
    include/rtems/score/threadmp.h
endif

## inline
include_rtems_score_HEADERS += inline/rtems/score/address.inl \
    inline/rtems/score/chain.inl inline/rtems/score/corebarrier.inl \
    inline/rtems/score/coremsg.inl inline/rtems/score/corerwlock.inl \
    inline/rtems/score/corespinlock.inl \
    inline/rtems/score/coremutex.inl inline/rtems/score/coresem.inl \
    inline/rtems/score/heap.inl inline/rtems/score/isr.inl \
    inline/rtems/score/object.inl inline/rtems/score/priority.inl \
    inline/rtems/score/stack.inl inline/rtems/score/states.inl \
    inline/rtems/score/sysstate.inl inline/rtems/score/thread.inl \
    inline/rtems/score/threadq.inl inline/rtems/score/tod.inl \
    inline/rtems/score/tqdata.inl \
    inline/rtems/score/watchdog.inl inline/rtems/score/wkspace.inl

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += inline/rtems/score/mppkt.inl \
    inline/rtems/score/objectmp.inl inline/rtems/score/threadmp.inl
endif

## src

AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libscore.a
libscore_a_SOURCES =
libscore_a_CPPFLAGS = $(AM_CPPFLAGS)

# We only build multiprocessing related files if HAS_MP was defined
if HAS_MP
libscore_a_SOURCES += src/mpci.c src/objectmp.c src/threadmp.c
endif

## CORE_APIMUTEX_C_FILES
libscore_a_SOURCES += src/apimutex.c src/apimutexallocate.c \
    src/apimutexlock.c src/apimutexunlock.c

## CORE_BARRIER_C_FILES
libscore_a_SOURCES += src/corebarrier.c src/corebarrierrelease.c \
    src/corebarrierwait.c

## CORE_MESSAGE_QUEUE_C_FILES
libscore_a_SOURCES += src/coremsg.c src/coremsgbroadcast.c \
    src/coremsgclose.c src/coremsgflush.c src/coremsgflushwait.c \
    src/coremsginsert.c src/coremsgflushsupp.c src/coremsgseize.c \
    src/coremsgsubmit.c

## CORE_MUTEX_C_FILES
libscore_a_SOURCES += src/coremutex.c src/coremutexflush.c \
    src/coremutexseize.c src/coremutexsurrender.c \
    src/coremutexseizeintr.c

## CORE_RWLOCK_C_FILES
libscore_a_SOURCES += src/corerwlock.c src/corerwlockobtainread.c \
    src/corerwlockobtainwrite.c src/corerwlockrelease.c src/corerwlocktimeout.c

## CORE_SEMAPHORE_C_FILES
libscore_a_SOURCES += src/coresem.c src/coresemflush.c src/coresemseize.c \
    src/coresemsurrender.c

## CORE_SPINLOCK_C_FILES
libscore_a_SOURCES += src/corespinlock.c src/corespinlockrelease.c \
    src/corespinlockwait.c

## HEAP_C_FILES
libscore_a_SOURCES += src/heap.c src/heapallocate.c src/heapextend.c \
    src/heapfree.c src/heapsizeofuserarea.c src/heapwalk.c src/heapgetinfo.c \
    src/heapgetfreeinfo.c src/heapallocatealigned.c \
    src/heapresizeblock.c

## OBJECT_C_FILES
libscore_a_SOURCES += src/object.c src/objectallocate.c src/objectclose.c \
    src/objectextendinformation.c src/objectfree.c src/objectget.c \
    src/objectgetisr.c src/objectgetnext.c src/objectinitializeinformation.c \
    src/objectnametoid.c src/objectnametoidstring.c \
    src/objectshrinkinformation.c src/objectgetnoprotection.c \
    src/objectidtoname.c src/objectgetnameasstring.c src/objectsetname.c \
    src/objectgetinfo.c src/objectgetinfoid.c src/objectapimaximumclass.c \
    src/objectnamespaceremove.c

## OBJECT_C_FILES only used by ITRON API
if LIBITRON
libscore_a_SOURCES += src/objectallocatebyindex.c src/objectgetbyindex.c
endif

## PROTECTED_HEAP_C_FILES
libscore_a_SOURCES += src/pheapallocatealigned.c src/pheapallocate.c \
    src/pheapextend.c src/pheapfree.c src/pheapgetblocksize.c \
    src/pheapgetfreeinfo.c src/pheapgetinfo.c src/pheapinit.c \
    src/pheapresizeblock.c src/pheapwalk.c

## THREAD_C_FILES
libscore_a_SOURCES += src/thread.c src/threadchangepriority.c \
    src/threadclearstate.c src/threadclose.c src/threadcreateidle.c \
    src/threaddelayended.c src/threaddispatch.c src/threadevaluatemode.c \
    src/threadget.c src/threadhandler.c src/threadidlebody.c \
    src/threadinitialize.c src/threadloadenv.c src/threadready.c \
    src/threadresettimeslice.c src/threadreset.c src/threadrestart.c \
    src/threadresume.c src/threadsetpriority.c src/threadsetstate.c \
    src/threadsettransient.c src/threadstackallocate.c \
    src/threadstackfree.c src/threadstart.c src/threadstartmultitasking.c \
    src/threadsuspend.c src/threadtickletimeslice.c \
    src/threadyieldprocessor.c src/iterateoverthreads.c \
    src/threadblockingoperationcancel.c

## THREAD_C_FILES only used by ITRON API
if LIBITRON
libscore_a_SOURCES += src/threadrotatequeue.c 
endif

## THREADQ_C_FILES
libscore_a_SOURCES += src/threadq.c src/threadqdequeue.c \
    src/threadqdequeuefifo.c src/threadqdequeuepriority.c \
    src/threadqenqueue.c src/threadqenqueuefifo.c \
    src/threadqenqueuepriority.c src/threadqextract.c \
    src/threadqextractfifo.c src/threadqextractpriority.c \
    src/threadqextractwithproxy.c src/threadqfirst.c src/threadqfirstfifo.c \
    src/threadqfirstpriority.c src/threadqflush.c src/threadqrequeue.c \
    src/threadqprocesstimeout.c src/threadqtimeout.c

## TIMESPEC_C_FILES
libscore_a_SOURCES += src/timespecaddto.c src/timespecfromticks.c \
    src/timespecisvalid.c src/timespeclessthan.c src/timespecgreaterthan.c \
    src/timespecsubtract.c src/timespectoticks.c src/timespecdivide.c \
    src/timespecdividebyinteger.c

## TOD_C_FILES
libscore_a_SOURCES += src/coretod.c src/coretodset.c src/coretodget.c \
    src/coretodgetuptime.c src/coretodtickle.c

## WATCHDOG_C_FILES
libscore_a_SOURCES += src/watchdog.c src/watchdogadjust.c \
    src/watchdoginsert.c src/watchdogremove.c src/watchdogtickle.c \
    src/watchdogreport.c src/watchdogreportchain.c

## USEREXT_C_FILES
libscore_a_SOURCES += src/userextaddapiset.c src/userextaddset.c \
    src/userext.c src/userextremoveset.c src/userextthreadbegin.c \
    src/userextthreadcreate.c src/userextthreaddelete.c \
    src/userextthreadrestart.c src/userextthreadstart.c \
    src/userextthreadswitch.c

## STD_C_FILES
libscore_a_SOURCES += src/apiext.c src/chain.c src/chainappend.c \
    src/chainextract.c src/chainget.c src/chaininsert.c \
    src/interr.c src/isr.c src/wkspace.c

EXTRA_DIST = src/Unlimited.txt

DISTCLEANFILES = include/rtems/score/cpuopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
