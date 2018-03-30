include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am


noinst_LIBRARIES = libposix.a
libposix_a_CPPFLAGS = $(AM_CPPFLAGS)
libposix_a_SOURCES =

# Some POSIX functions that are nice to always have
libposix_a_SOURCES += src/nanosleep.c src/clockgettime.c src/clocksettime.c \
    src/clockgetres.c src/sysconf.c

## Some POSIX threads features are needed all the time
if HAS_PTHREADS
# include
# include/rtems/posix
## src
libposix_a_SOURCES += src/aio_cancel.c src/aio_error.c src/aio_fsync.c \
    src/aio_read.c src/aio_return.c src/aio_suspend.c src/aio_write.c \
    src/lio_listio.c src/aio_misc.c

libposix_a_SOURCES += src/_execve.c
endif

libposix_a_SOURCES += src/fork.c src/vfork.c

libposix_a_SOURCES += src/wait.c src/waitpid.c
libposix_a_SOURCES += src/pthreadgetnamenp.c
libposix_a_SOURCES += src/pthreadsetnamenp.c

## BARRIER_C_FILES
libposix_a_SOURCES += src/barrierattrdestroy.c src/barrierattrgetpshared.c \
    src/barrierattrinit.c src/barrierattrsetpshared.c \
    src/pbarrierdestroy.c src/pbarrierinit.c \
    src/pbarrierwait.c

## CONDITION_VARIABLE_C_FILES
libposix_a_SOURCES += src/condattrdestroy.c \
    src/condattrinit.c  \
    src/condattrgetpshared.c src/condattrsetpshared.c \
    src/condattrgetclock.c src/condattrsetclock.c \
    src/condbroadcast.c src/conddefaultattributes.c src/conddestroy.c \
    src/condinit.c src/condsignal.c src/condsignalsupp.c \
    src/condtimedwait.c src/condwait.c src/condwaitsupp.c

## MUTEX_C_FILES
libposix_a_SOURCES += src/mutexattrdestroy.c src/mutexattrgetprioceiling.c \
    src/mutexattrgetprotocol.c src/mutexattrgetpshared.c \
    src/mutexattrgettype.c src/mutexattrinit.c src/mutexattrsetprioceiling.c \
    src/mutexattrsetprotocol.c src/mutexattrsetpshared.c \
    src/mutexattrsettype.c src/mutexdestroy.c \
    src/mutexgetprioceiling.c src/mutexinit.c src/mutexlock.c \
    src/mutexlocksupp.c src/mutexsetprioceiling.c src/mutextimedlock.c \
    src/mutextrylock.c src/mutexunlock.c

## PTHREAD_C_FILES
libposix_a_SOURCES += src/psxpriorityisvalid.c

## PTHREAD_ATTR_C_FILES
libposix_a_SOURCES += src/pthreadattrdefault.c src/pthreadattrdestroy.c \
   src/pthreadattrgetdetachstate.c src/pthreadattrgetguardsize.c \
   src/pthreadattrgetinheritsched.c src/pthreadattrgetschedparam.c \
   src/pthreadattrgetschedpolicy.c src/pthreadattrgetscope.c \
   src/pthreadattrgetstackaddr.c src/pthreadattrgetstack.c \
   src/pthreadattrgetstacksize.c src/pthreadattrinit.c \
   src/pthreadattrsetdetachstate.c src/pthreadattrsetguardsize.c \
   src/pthreadattrsetinheritsched.c src/pthreadattrsetschedparam.c \
   src/pthreadattrsetschedpolicy.c src/pthreadattrsetscope.c \
   src/pthreadattrsetstackaddr.c src/pthreadattrsetstack.c \
   src/pthreadattrsetstacksize.c

## PTHREAD_AFFINITY_C_FILES
libposix_a_SOURCES += src/pthreadattrsetaffinitynp.c \
    src/pthreadattrgetaffinitynp.c  src/pthreadgetaffinitynp.c   \
    src/pthreadsetaffinitynp.c

if HAS_PTHREADS
## CANCEL_C_FILES
libposix_a_SOURCES += src/cancel.c \
    src/cleanuppush.c src/setcancelstate.c \
    src/setcanceltype.c src/testcancel.c

## MEMORY_C_FILES
libposix_a_SOURCES += src/mlockall.c
libposix_a_SOURCES += src/mlock.c
libposix_a_SOURCES += src/mmap.c
libposix_a_SOURCES += src/mprotect.c
libposix_a_SOURCES += src/msync.c
libposix_a_SOURCES += src/munlockall.c
libposix_a_SOURCES += src/munlock.c
libposix_a_SOURCES += src/munmap.c
libposix_a_SOURCES += src/posix_madvise.c
libposix_a_SOURCES += src/shm.c
libposix_a_SOURCES += src/shmheap.c
libposix_a_SOURCES += src/shmopen.c
libposix_a_SOURCES += src/shmunlink.c
libposix_a_SOURCES += src/shmwkspace.c

## MESSAGE_QUEUE_C_FILES
libposix_a_SOURCES += src/mqueue.c src/mqueueclose.c \
    src/mqueuedeletesupp.c src/mqueuegetattr.c \
    src/mqueuenotify.c src/mqueueopen.c \
    src/mqueuereceive.c src/mqueuerecvsupp.c src/mqueuesend.c \
    src/mqueuesendsupp.c src/mqueuesetattr.c src/mqueuetimedreceive.c \
    src/mqueuetimedsend.c \
    src/mqueueunlink.c

## PTHREAD_C_FILES
libposix_a_SOURCES += src/pthreadatfork.c \
    src/pthreadgetattrnp.c \
    src/pthread.c \
    src/pthreadconcurrency.c \
    src/pthreadcreate.c src/pthreaddetach.c src/pthreadequal.c \
    src/pthreadexit.c src/pthreadgetcpuclockid.c \
    src/pthreadgetschedparam.c \
    src/pthreadinitthreads.c src/pthreadjoin.c src/pthreadkill.c \
    src/pthreadself.c \
    src/pthreadsetschedparam.c src/pthreadsigmask.c \
    src/psxtransschedparam.c
libposix_a_SOURCES += src/pthreadsetschedprio.c

## PSIGNAL_C_FILES
libposix_a_SOURCES += src/psignal.c src/alarm.c src/kill.c src/killinfo.c \
    src/kill_r.c src/pause.c src/psignalclearprocesssignals.c \
    src/psignalsetprocesssignals.c \
    src/psignalclearsignals.c src/psignalunblockthread.c src/pthreadkill.c \
    src/pthreadsigmask.c src/sigaction.c
endif

## KEY_C_FILES
libposix_a_SOURCES += src/key.c src/keycreate.c src/keydelete.c \
    src/keygetspecific.c \
    src/keysetspecific.c

## ONCE_C_FILES
libposix_a_SOURCES += src/pthreadonce.c

libposix_a_SOURCES += src/sigaddset.c src/sigdelset.c src/sigfillset.c \
   src/sigemptyset.c src/sigismember.c

libposix_a_SOURCES += src/sigprocmask.c

## SPINLOCK_C_FILES
libposix_a_SOURCES += src/pspindestroy.c
libposix_a_SOURCES += src/pspininit.c
libposix_a_SOURCES += src/pspinlock.c
libposix_a_SOURCES += src/pspinunlock.c

## SEMAPHORE_C_FILES
libposix_a_SOURCES += src/semdestroy.c
libposix_a_SOURCES += src/semgetvalue.c
libposix_a_SOURCES += src/seminit.c
libposix_a_SOURCES += src/sempost.c
libposix_a_SOURCES += src/semtimedwait.c
libposix_a_SOURCES += src/semtrywait.c
libposix_a_SOURCES += src/semwait.c

## RWLOCK_C_FILES
libposix_a_SOURCES += src/prwlockdestroy.c src/prwlockinit.c \
    src/prwlockrdlock.c src/prwlocktimedrdlock.c src/prwlocktimedwrlock.c \
    src/prwlocktryrdlock.c src/prwlocktrywrlock.c src/prwlockunlock.c \
    src/prwlockwrlock.c src/rwlockattrdestroy.c src/rwlockattrgetpshared.c \
    src/rwlockattrinit.c src/rwlockattrsetpshared.c

if HAS_PTHREADS
libposix_a_SOURCES += src/sigpending.c \
    src/sigqueue.c src/sigsuspend.c src/sigtimedwait.c \
    src/sigwait.c src/sigwaitinfo.c src/signal_2.c src/ualarm.c

## SEMAPHORE_C_FILES
libposix_a_SOURCES += src/semaphore.c
libposix_a_SOURCES += src/semaphoredeletesupp.c
libposix_a_SOURCES += src/semclose.c
libposix_a_SOURCES += src/semopen.c
libposix_a_SOURCES += src/semunlink.c

## TIME_C_FILES
libposix_a_SOURCES += src/adjtime.c src/clockgetcpuclockid.c

## TIMER_C_FILES
libposix_a_SOURCES += src/ptimer.c src/timercreate.c src/timerdelete.c \
    src/timergetoverrun.c src/timergettime.c src/timersettime.c

## ITIMER_C_FILES
libposix_a_SOURCES += src/getitimer.c src/setitimer.c

## SUPPORT_C_FILES
libposix_a_SOURCES += src/psxpriorityisvalid.c
libposix_a_SOURCES += src/psxnametoid.c


libposix_a_SOURCES += src/sched_getparam.c \
    src/sched_getscheduler.c \
    src/sched_rr_get_interval.c src/sched_setparam.c \
    src/sched_setscheduler.c src/sched_yield.c
endif

libposix_a_SOURCES += src/sched_getprioritymin.c
libposix_a_SOURCES += src/sched_getprioritymax.c

include $(top_srcdir)/automake/local.am
