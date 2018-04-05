include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

_SUBDIRS = cpu

## include

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
endif

if HAS_SMP
endif

## src

noinst_LIBRARIES = libscore.a
libscore_a_SOURCES =
libscore_a_CPPFLAGS = $(AM_CPPFLAGS)

# We only build multiprocessing related files if HAS_MP was defined
if HAS_MP
libscore_a_SOURCES += src/mpci.c src/objectmp.c src/threadmp.c
endif

if HAS_SMP
libscore_a_SOURCES += src/percpustatewait.c
libscore_a_SOURCES += src/profilingsmplock.c
libscore_a_SOURCES += src/scheduleredfsmp.c
libscore_a_SOURCES += src/schedulerpriorityaffinitysmp.c
libscore_a_SOURCES += src/schedulerprioritysmp.c
libscore_a_SOURCES += src/schedulersimplesmp.c
libscore_a_SOURCES += src/schedulerstrongapa.c
libscore_a_SOURCES += src/smp.c
libscore_a_SOURCES += src/smplock.c
libscore_a_SOURCES += src/smpmulticastaction.c
libscore_a_SOURCES += src/schedulerdefaultaskforhelp.c
libscore_a_SOURCES += src/schedulerdefaultsetaffinity.c
libscore_a_SOURCES += src/schedulersmp.c
libscore_a_SOURCES += src/schedulersmpstartidle.c
endif

## CORE_APIMUTEX_C_FILES
libscore_a_SOURCES += src/allocatormutex.c
libscore_a_SOURCES += src/apimutexisowner.c
libscore_a_SOURCES += src/apimutexlock.c
libscore_a_SOURCES += src/apimutexunlock.c

## CORE_BARRIER_C_FILES
libscore_a_SOURCES += src/corebarrier.c src/corebarrierrelease.c \
    src/corebarrierwait.c

## CORE_MESSAGE_QUEUE_C_FILES
libscore_a_SOURCES += src/coremsg.c src/coremsgbroadcast.c \
    src/coremsgclose.c src/coremsgflush.c src/coremsgflushwait.c \
    src/coremsginsert.c src/coremsgseize.c \
    src/coremsgsubmit.c

## CORE_MUTEX_C_FILES
libscore_a_SOURCES += src/coremutexseize.c

## CORE_PERCPU_C_FILES
libscore_a_SOURCES += src/percpu.c
libscore_a_SOURCES += src/percpuasm.c

## CORE_RWLOCK_C_FILES
libscore_a_SOURCES += src/corerwlock.c src/corerwlockobtainread.c \
    src/corerwlockobtainwrite.c src/corerwlockrelease.c

## CORE_SEMAPHORE_C_FILES
libscore_a_SOURCES += src/coresem.c

## HEAP_C_FILES
libscore_a_SOURCES += src/heap.c src/heapallocate.c src/heapextend.c \
    src/heapfree.c src/heapsizeofuserarea.c src/heapwalk.c src/heapgetinfo.c \
    src/heapgetfreeinfo.c src/heapresizeblock.c src/heapiterate.c \
    src/heapgreedy.c src/heapnoextend.c

## OBJECT_C_FILES
libscore_a_SOURCES += src/objectallocate.c src/objectclose.c \
    src/objectextendinformation.c src/objectfree.c \
    src/objectgetnext.c src/objectinitializeinformation.c \
    src/objectnametoid.c src/objectnametoidstring.c \
    src/objectshrinkinformation.c src/objectgetnoprotection.c \
    src/objectidtoname.c src/objectgetnameasstring.c src/objectsetname.c \
    src/objectgetinfo.c src/objectgetinfoid.c src/objectapimaximumclass.c \
    src/objectnamespaceremove.c \
    src/objectactivecount.c
libscore_a_SOURCES += src/objectgetlocal.c

## SCHEDULER_C_FILES
libscore_a_SOURCES += src/log2table.c
libscore_a_SOURCES += src/scheduler.c
libscore_a_SOURCES += src/schedulergetaffinity.c
libscore_a_SOURCES += src/schedulersetaffinity.c
libscore_a_SOURCES += src/schedulerdefaultmappriority.c
libscore_a_SOURCES += src/schedulerdefaultnodedestroy.c
libscore_a_SOURCES += src/schedulerdefaultnodeinit.c
libscore_a_SOURCES += src/schedulerdefaultreleasejob.c
libscore_a_SOURCES += src/schedulerdefaultschedule.c
libscore_a_SOURCES += src/schedulerdefaultstartidle.c
libscore_a_SOURCES += src/schedulerdefaulttick.c

## SCHEDULERPRIORITY_C_FILES
libscore_a_SOURCES += src/schedulerpriority.c \
    src/schedulerpriorityblock.c \
    src/schedulerprioritychangepriority.c \
    src/schedulerpriorityschedule.c \
    src/schedulerpriorityunblock.c \
    src/schedulerpriorityyield.c

## SCHEDULERSIMPLE_C_FILES
libscore_a_SOURCES += src/schedulersimple.c \
    src/schedulersimpleblock.c \
    src/schedulersimplechangepriority.c \
    src/schedulersimpleschedule.c \
    src/schedulersimpleunblock.c \
    src/schedulersimpleyield.c

## SCHEDULEREDF_C_FILES
libscore_a_SOURCES += src/scheduleredf.c \
    src/scheduleredfnodeinit.c \
    src/scheduleredfblock.c \
    src/scheduleredfchangepriority.c \
    src/scheduleredfreleasejob.c \
    src/scheduleredfschedule.c \
    src/scheduleredfunblock.c \
    src/scheduleredfyield.c

## SCHEDULERCBS_C_FILES
libscore_a_SOURCES += src/schedulercbs.c \
    src/schedulercbsnodeinit.c \
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

## FREECHAIN_C_FILES
libscore_a_SOURCES += src/freechain.c

## RBTREE_C_FILES
libscore_a_SOURCES += \
    src/rbtreeextract.c \
    src/rbtreeinsert.c src/rbtreeiterate.c src/rbtreenext.c
libscore_a_SOURCES += src/rbtreereplace.c

## THREAD_C_FILES
libscore_a_SOURCES += src/thread.c src/threadchangepriority.c \
    src/threadclearstate.c src/threadcreateidle.c \
    src/threaddispatch.c \
    src/threadget.c src/threadhandler.c src/threadinitialize.c \
    src/threadloadenv.c \
    src/threadrestart.c \
    src/threadsetstate.c \
    src/threadstackallocate.c src/threadstackfree.c src/threadstart.c \
    src/threadstartmultitasking.c src/iterateoverthreads.c
libscore_a_SOURCES += src/threadentryadaptoridle.c
libscore_a_SOURCES += src/threadentryadaptornumeric.c
libscore_a_SOURCES += src/threadentryadaptorpointer.c
libscore_a_SOURCES += src/threadgetcputimeused.c
libscore_a_SOURCES += src/threaditerate.c
libscore_a_SOURCES += src/threadname.c
libscore_a_SOURCES += src/threadscheduler.c
libscore_a_SOURCES += src/threadtimeout.c
libscore_a_SOURCES += src/threadwaitgetid.c
libscore_a_SOURCES += src/threadyield.c

## THREADQ_C_FILES
libscore_a_SOURCES += src/threadq.c \
    src/threadqenqueue.c \
    src/threadqextractwithproxy.c src/threadqfirst.c \
    src/threadqflush.c
libscore_a_SOURCES += src/threadqops.c
libscore_a_SOURCES += src/threadqtimeout.c

## TIMESPEC_C_FILES
libscore_a_SOURCES += src/timespecaddto.c src/timespecfromticks.c \
    src/timespecisvalid.c src/timespeclessthan.c \
    src/timespecsubtract.c src/timespectoticks.c src/timespecdivide.c \
    src/timespecdividebyinteger.c src/timespecgetasnanoseconds.c

## TOD_C_FILES
libscore_a_SOURCES += src/coretod.c src/coretodset.c \
    src/coretodtickspersec.c \
    src/coretodadjust.c

## WATCHDOG_C_FILES
libscore_a_SOURCES += src/watchdoginsert.c
libscore_a_SOURCES += src/watchdogremove.c
libscore_a_SOURCES += src/watchdogtick.c
libscore_a_SOURCES += src/watchdogtickssinceboot.c

## USEREXT_C_FILES
libscore_a_SOURCES += src/userextaddset.c \
    src/userext.c src/userextremoveset.c src/userextiterate.c

## STD_C_FILES
libscore_a_SOURCES += src/chain.c \
    src/chainnodecount.c \
    src/debugisthreaddispatchingallowed.c \
    src/interr.c src/isr.c src/wkspace.c src/wkstringduplicate.c
libscore_a_SOURCES += src/ioprintf.c
libscore_a_SOURCES += src/iovprintf.c
libscore_a_SOURCES += src/isrisinprogress.c
libscore_a_SOURCES += src/condition.c
libscore_a_SOURCES += src/futex.c
libscore_a_SOURCES += src/profilingisrentryexit.c
libscore_a_SOURCES += src/mutex.c
libscore_a_SOURCES += src/once.c
libscore_a_SOURCES += src/sched.c
libscore_a_SOURCES += src/semaphore.c
libscore_a_SOURCES += src/smpbarrierwait.c
libscore_a_SOURCES += src/kern_tc.c
libscore_a_SOURCES += src/libatomic.c
libscore_a_SOURCES += src/processormaskcopy.c


# This file is in the build tree
include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = ../../include/rtems/score/cpuopts.h

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
