include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

SUBDIRS = cpu

## include

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/debug.h
include_rtems_HEADERS += include/rtems/system.h
include_rtems_HEADERS += include/rtems/seterr.h
include_rtems_HEADERS += include/rtems/bspsmp.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS = include/rtems/score/address.h
include_rtems_score_HEADERS += include/rtems/score/apiext.h
include_rtems_score_HEADERS += include/rtems/score/apimutex.h
include_rtems_score_HEADERS += include/rtems/score/bitfield.h
include_rtems_score_HEADERS += include/rtems/score/chain.h
include_rtems_score_HEADERS += include/rtems/score/context.h
include_rtems_score_HEADERS += include/rtems/score/copyrt.h
include_rtems_score_HEADERS += include/rtems/score/corebarrier.h
include_rtems_score_HEADERS += include/rtems/score/coremsg.h
include_rtems_score_HEADERS += include/rtems/score/coremutex.h
include_rtems_score_HEADERS += include/rtems/score/coresem.h
include_rtems_score_HEADERS += include/rtems/score/heap.h
include_rtems_score_HEADERS += include/rtems/score/protectedheap.h
include_rtems_score_HEADERS += include/rtems/score/interr.h
include_rtems_score_HEADERS += include/rtems/score/isr.h
include_rtems_score_HEADERS += include/rtems/score/isrlevel.h
include_rtems_score_HEADERS += include/rtems/score/object.h
include_rtems_score_HEADERS += include/rtems/score/percpu.h
include_rtems_score_HEADERS += include/rtems/score/priority.h
include_rtems_score_HEADERS += include/rtems/score/prioritybitmap.h
include_rtems_score_HEADERS += include/rtems/score/rbtree.h
include_rtems_score_HEADERS += include/rtems/score/scheduler.h
include_rtems_score_HEADERS += include/rtems/score/schedulercbs.h
include_rtems_score_HEADERS += include/rtems/score/scheduleredf.h
include_rtems_score_HEADERS += include/rtems/score/schedulerpriority.h
include_rtems_score_HEADERS += include/rtems/score/schedulersimple.h
include_rtems_score_HEADERS += include/rtems/score/stack.h
include_rtems_score_HEADERS += include/rtems/score/states.h
include_rtems_score_HEADERS += include/rtems/score/sysstate.h
include_rtems_score_HEADERS += include/rtems/score/thread.h
include_rtems_score_HEADERS += include/rtems/score/threadq.h
include_rtems_score_HEADERS += include/rtems/score/threadsync.h
include_rtems_score_HEADERS += include/rtems/score/timespec.h
include_rtems_score_HEADERS += include/rtems/score/timestamp.h
include_rtems_score_HEADERS += include/rtems/score/timestamp64.h
include_rtems_score_HEADERS += include/rtems/score/tod.h
include_rtems_score_HEADERS += include/rtems/score/tqdata.h
include_rtems_score_HEADERS += include/rtems/score/userext.h
include_rtems_score_HEADERS += include/rtems/score/watchdog.h
include_rtems_score_HEADERS += include/rtems/score/wkspace.h
include_rtems_score_HEADERS += include/rtems/score/cpuopts.h
include_rtems_score_HEADERS += include/rtems/score/basedefs.h

if HAS_SMP
include_rtems_score_HEADERS += include/rtems/score/smplock.h
include_rtems_score_HEADERS += include/rtems/score/smp.h
endif

if HAS_PTHREADS
include_rtems_score_HEADERS += include/rtems/score/corespinlock.h
include_rtems_score_HEADERS += include/rtems/score/corerwlock.h
endif

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += include/rtems/score/mpci.h
include_rtems_score_HEADERS += include/rtems/score/mppkt.h
include_rtems_score_HEADERS += include/rtems/score/objectmp.h
include_rtems_score_HEADERS += include/rtems/score/threadmp.h
endif

if HAS_SMP
include_rtems_score_HEADERS += include/rtems/score/schedulersimplesmp.h
endif

## inline
include_rtems_score_HEADERS += inline/rtems/score/address.inl
include_rtems_score_HEADERS += inline/rtems/score/chain.inl
include_rtems_score_HEADERS += inline/rtems/score/corebarrier.inl
include_rtems_score_HEADERS += inline/rtems/score/coremsg.inl
include_rtems_score_HEADERS += inline/rtems/score/coremutex.inl
include_rtems_score_HEADERS += inline/rtems/score/coresem.inl
include_rtems_score_HEADERS += inline/rtems/score/heap.inl
include_rtems_score_HEADERS += inline/rtems/score/isr.inl
include_rtems_score_HEADERS += inline/rtems/score/object.inl
include_rtems_score_HEADERS += inline/rtems/score/priority.inl
include_rtems_score_HEADERS += inline/rtems/score/prioritybitmap.inl
include_rtems_score_HEADERS += inline/rtems/score/rbtree.inl
include_rtems_score_HEADERS += inline/rtems/score/scheduler.inl
include_rtems_score_HEADERS += inline/rtems/score/schedulerpriority.inl
include_rtems_score_HEADERS += inline/rtems/score/schedulersimple.inl
include_rtems_score_HEADERS += inline/rtems/score/stack.inl
include_rtems_score_HEADERS += inline/rtems/score/states.inl
include_rtems_score_HEADERS += inline/rtems/score/sysstate.inl
include_rtems_score_HEADERS += inline/rtems/score/thread.inl
include_rtems_score_HEADERS += inline/rtems/score/threadq.inl
include_rtems_score_HEADERS += inline/rtems/score/tod.inl
include_rtems_score_HEADERS += inline/rtems/score/tqdata.inl
include_rtems_score_HEADERS += inline/rtems/score/watchdog.inl
include_rtems_score_HEADERS += inline/rtems/score/wkspace.inl

if HAS_PTHREADS
include_rtems_score_HEADERS += inline/rtems/score/corespinlock.inl
include_rtems_score_HEADERS += inline/rtems/score/corerwlock.inl
endif

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += inline/rtems/score/mppkt.inl
include_rtems_score_HEADERS += inline/rtems/score/objectmp.inl
include_rtems_score_HEADERS += inline/rtems/score/threadmp.inl
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

if HAS_SMP
libscore_a_SOURCES += src/isrsmp.c src/smp.c src/smplock.c \
    src/schedulersimplesmpblock.c src/schedulersimplesmpschedule.c \
    src/schedulersimplesmpunblock.c src/schedulersimplesmptick.c 
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

## CORE_PERCPU_C_FILES
libscore_a_SOURCES += src/percpu.c

## CORE_RWLOCK_C_FILES
if HAS_PTHREADS
libscore_a_SOURCES += src/corerwlock.c src/corerwlockobtainread.c \
    src/corerwlockobtainwrite.c src/corerwlockrelease.c src/corerwlocktimeout.c
endif

## CORE_SEMAPHORE_C_FILES
libscore_a_SOURCES += src/coresem.c src/coresemflush.c src/coresemseize.c \
    src/coresemsurrender.c

## CORE_SPINLOCK_C_FILES
if HAS_PTHREADS
libscore_a_SOURCES += src/corespinlock.c src/corespinlockrelease.c \
    src/corespinlockwait.c
endif

## HEAP_C_FILES
libscore_a_SOURCES += src/heap.c src/heapallocate.c src/heapextend.c \
    src/heapfree.c src/heapsizeofuserarea.c src/heapwalk.c src/heapgetinfo.c \
    src/heapgetfreeinfo.c src/heapresizeblock.c src/heapiterate.c \
    src/heapgreedy.c

## OBJECT_C_FILES
libscore_a_SOURCES += src/objectallocate.c src/objectclose.c \
    src/objectextendinformation.c src/objectfree.c src/objectget.c \
    src/objectgetisr.c src/objectgetnext.c src/objectinitializeinformation.c \
    src/objectnametoid.c src/objectnametoidstring.c \
    src/objectshrinkinformation.c src/objectgetnoprotection.c \
    src/objectidtoname.c src/objectgetnameasstring.c src/objectsetname.c \
    src/objectgetinfo.c src/objectgetinfoid.c src/objectapimaximumclass.c \
    src/objectnamespaceremove.c

## SCHEDULER_C_FILES
libscore_a_SOURCES += src/scheduler.c

## SCHEDULERPRIORITY_C_FILES
libscore_a_SOURCES += src/schedulerpriority.c \
    src/schedulerpriorityallocate.c \
    src/schedulerprioritytick.c \
    src/schedulerpriorityblock.c \
    src/schedulerpriorityenqueue.c \
    src/schedulerpriorityenqueuefirst.c \
    src/schedulerpriorityextract.c \
    src/schedulerpriorityfree.c \
    src/schedulerpriorityprioritycompare.c \
    src/schedulerpriorityreleasejob.c \
    src/schedulerpriorityschedule.c \
    src/schedulerpriorityunblock.c \
    src/schedulerpriorityupdate.c \
    src/schedulerpriorityyield.c

## SCHEDULERSIMPLE_C_FILES
libscore_a_SOURCES += src/schedulersimple.c \
    src/schedulersimpleblock.c \
    src/schedulersimpleenqueue.c \
    src/schedulersimpleenqueuefirst.c \
    src/schedulersimpleextract.c \
    src/schedulersimplereadyqueueenqueue.c \
    src/schedulersimplereadyqueueenqueuefirst.c \
    src/schedulersimpleschedule.c \
    src/schedulersimpleunblock.c \
    src/schedulersimpleyield.c 

## SCHEDULEREDF_C_FILES
libscore_a_SOURCES += src/scheduleredf.c \
    src/scheduleredfallocate.c \
    src/scheduleredfblock.c \
    src/scheduleredfenqueue.c \
    src/scheduleredfenqueuefirst.c \
    src/scheduleredfextract.c \
    src/scheduleredffree.c \
    src/scheduleredfprioritycompare.c \
    src/scheduleredfreleasejob.c \
    src/scheduleredfschedule.c \
    src/scheduleredfunblock.c \
    src/scheduleredfupdate.c \
    src/scheduleredfyield.c

## SCHEDULERCBS_C_FILES
libscore_a_SOURCES += src/schedulercbs.c \
    src/schedulercbsallocate.c \
    src/schedulercbsattachthread.c \
    src/schedulercbscleanup.c \
    src/schedulercbscreateserver.c \
    src/schedulercbsdestroyserver.c \
    src/schedulercbsdetachthread.c \
    src/schedulercbsgetapprovedbudget.c \
    src/schedulercbsgetexecutiontime.c \
    src/schedulercbsgetparameters.c \
    src/schedulercbsgetremainingbudget.c \
    src/schedulercbsgetserverid.c \
    src/schedulercbssetparameters.c \
    src/schedulercbsreleasejob.c \
    src/schedulercbsunblock.c

## PROTECTED_HEAP_C_FILES
libscore_a_SOURCES += src/pheapallocate.c \
    src/pheapextend.c src/pheapfree.c src/pheapgetsize.c \
    src/pheapgetblocksize.c src/pheapgetfreeinfo.c src/pheapgetinfo.c \
    src/pheapinit.c src/pheapresizeblock.c src/pheapwalk.c src/pheapiterate.c

## RBTREE_C_FILES
libscore_a_SOURCES += src/rbtree.c \
    src/rbtreeextract.c src/rbtreefind.c src/rbtreefindheader.c \
    src/rbtreeget.c src/rbtreeinsert.c src/rbtreeiterate.c src/rbtreenext.c

## THREAD_C_FILES
libscore_a_SOURCES += src/thread.c src/threadchangepriority.c \
    src/threadclearstate.c src/threadclose.c src/threadcreateidle.c \
    src/threaddelayended.c src/threaddispatch.c \
    src/threadenabledispatch.c src/threaddisabledispatch.c \
    src/threadget.c src/threadhandler.c src/threadinitialize.c \
    src/threadloadenv.c src/threadready.c src/threadreset.c \
    src/threadrestart.c src/threadsetpriority.c \
    src/threadsetstate.c src/threadsettransient.c \
    src/threadstackallocate.c src/threadstackfree.c src/threadstart.c \
    src/threadstartmultitasking.c src/iterateoverthreads.c \
    src/threadblockingoperationcancel.c
    
if HAS_SMP
libscore_a_SOURCES += src/threaddispatchdisablelevel.c
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
    src/timespecisvalid.c src/timespeclessthan.c \
    src/timespecsubtract.c src/timespectoticks.c src/timespecdivide.c \
    src/timespecdividebyinteger.c

## TIMESTAMP_INT64_C_FILES
libscore_a_SOURCES += src/ts64addto.c src/ts64dividebyinteger.c \
    src/ts64divide.c src/ts64equalto.c \
    src/ts64getnanoseconds.c src/ts64getseconds.c \
    src/ts64lessthan.c \
    src/ts64set.c src/ts64settozero.c src/ts64subtract.c \
    src/ts64toticks.c src/ts64totimespec.c

## TOD_C_FILES
libscore_a_SOURCES += src/coretod.c src/coretodset.c src/coretodget.c \
    src/coretodgetuptime.c src/coretodgetuptimetimespec.c src/coretodtickle.c \
    src/coretodmsecstoticks.c src/coretodtickspersec.c src/coretodusectoticks.c

## WATCHDOG_C_FILES
libscore_a_SOURCES += src/watchdog.c src/watchdogadjust.c \
    src/watchdogadjusttochain.c src/watchdoginsert.c src/watchdogremove.c \
    src/watchdogtickle.c src/watchdogreport.c src/watchdogreportchain.c \
    src/watchdognanoseconds.c

## USEREXT_C_FILES
libscore_a_SOURCES += src/userextaddset.c \
    src/userext.c src/userextremoveset.c src/userextthreadbegin.c \
    src/userextthreadcreate.c src/userextthreaddelete.c \
    src/userextthreadrestart.c src/userextthreadstart.c \
    src/userextthreadswitch.c

## STD_C_FILES
libscore_a_SOURCES += src/apiext.c src/chain.c src/chainappend.c \
    src/chainextract.c src/chainget.c src/chaininsert.c \
    src/chainappendempty.c src/chainprependempty.c src/chaingetempty.c \
    src/interr.c src/isr.c src/wkspace.c src/wkstringduplicate.c

EXTRA_DIST = src/Unlimited.txt

DISTCLEANFILES = include/rtems/score/cpuopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
