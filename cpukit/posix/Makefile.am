##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =

if LIBPOSIX

AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libposix.a
libposix_a_CPPFLAGS = $(AM_CPPFLAGS)
libposix_a_SOURCES =

libposix_a_SOURCES += src/sleep.c
libposix_a_SOURCES += src/usleep.c
libposix_a_SOURCES += src/nanosleep.c

if HAS_PTHREADS
# include

include_HEADERS = include/sched.h include/aio.h include/mqueue.h \
    include/semaphore.h include/devctl.h

# include/rtems/posix

include_rtems_posixdir = $(includedir)/rtems/posix

include_rtems_posix_HEADERS = include/rtems/posix/cancel.h \
    include/rtems/posix/cond.h include/rtems/posix/config.h \
    include/rtems/posix/key.h include/rtems/posix/mqueue.h \
    include/rtems/posix/mutex.h include/rtems/posix/posixapi.h \
    include/rtems/posix/priority.h include/rtems/posix/psignal.h \
    include/rtems/posix/pthread.h include/rtems/posix/semaphore.h \
    include/rtems/posix/threadsup.h include/rtems/posix/time.h \
    include/rtems/posix/timer.h include/rtems/posix/barrier.h \
    include/rtems/posix/rwlock.h include/rtems/posix/spinlock.h

if HAS_MP
include_rtems_posix_HEADERS += include/rtems/posix/condmp.h \
    include/rtems/posix/mqueuemp.h include/rtems/posix/mutexmp.h \
    include/rtems/posix/pthreadmp.h include/rtems/posix/semaphoremp.h
endif

include_rtems_posix_HEADERS += inline/rtems/posix/cond.inl \
    inline/rtems/posix/key.inl inline/rtems/posix/mqueue.inl \
    inline/rtems/posix/mutex.inl inline/rtems/posix/pthread.inl \
    inline/rtems/posix/priority.inl inline/rtems/posix/semaphore.inl \
    inline/rtems/posix/timer.inl inline/rtems/posix/barrier.inl \
    inline/rtems/posix/rwlock.inl inline/rtems/posix/spinlock.inl

## src

# These are really in the stand but not really functional
## BUILD_FOR_NOW_C_FILES
libposix_a_SOURCES += src/aio.c src/devctl.c

## ENOSYS_C_FILES
libposix_a_SOURCES += src/execl.c src/execle.c src/execlp.c src/execv.c \
    src/execve.c src/execvp.c src/fork.c src/pthreadatfork.c src/wait.c \
    src/waitpid.c

## BARRIER_C_FILES
libposix_a_SOURCES += src/barrierattrdestroy.c src/barrierattrgetpshared.c \
    src/barrierattrinit.c src/barrierattrsetpshared.c src/pbarrier.c \
    src/pbarrierdestroy.c src/pbarrierinit.c \
    src/pbarriertranslatereturncode.c src/pbarrierwait.c

## CANCEL_C_FILES
libposix_a_SOURCES += src/cancel.c src/cancelrun.c src/cleanuppop.c \
    src/cleanuppush.c src/setcancelstate.c src/setcanceltype.c \
    src/testcancel.c

## CONDITION_VARIABLE_C_FILES
libposix_a_SOURCES += src/cond.c src/condattrdestroy.c \
    src/condattrgetpshared.c src/condattrinit.c src/condattrsetpshared.c \
    src/condbroadcast.c src/conddefaultattributes.c src/conddestroy.c \
    src/condinit.c src/condmp.c src/condsignal.c src/condsignalsupp.c \
    src/condtimedwait.c src/condwait.c src/condwaitsupp.c

## KEY_C_FILES
libposix_a_SOURCES += src/key.c src/keycreate.c src/keydelete.c \
    src/keygetspecific.c \
    src/keyrundestructors.c src/keysetspecific.c

## MEMORY_C_FILES
libposix_a_SOURCES += src/mprotect.c

## MESSAGE_QUEUE_C_FILES
libposix_a_SOURCES += src/mqueue.c src/mqueueclose.c \
    src/mqueuecreatesupp.c src/mqueuedeletesupp.c src/mqueuegetattr.c \
    src/mqueuenametoid.c src/mqueuenotify.c src/mqueueopen.c \
    src/mqueuereceive.c src/mqueuerecvsupp.c src/mqueuesend.c \
    src/mqueuesendsupp.c src/mqueuesetattr.c src/mqueuetimedreceive.c \
    src/mqueuetimedsend.c src/mqueuetranslatereturncode.c \
    src/mqueueunlink.c

## MUTEX_C_FILES
libposix_a_SOURCES += src/mutex.c src/mutexattrdestroy.c \
    src/mutexattrgetprioceiling.c src/mutexattrgetprotocol.c \
    src/mutexattrgetpshared.c src/mutexattrinit.c \
    src/mutexattrsetprioceiling.c src/mutexattrsetprotocol.c \
    src/mutexattrsetpshared.c src/mutexdefaultattributes.c \
    src/mutexdestroy.c src/mutexfromcorestatus.c src/mutexgetprioceiling.c \
    src/mutexinit.c src/mutexlock.c src/mutexlocksupp.c src/mutexmp.c \
    src/mutexsetprioceiling.c src/mutextimedlock.c src/mutextrylock.c \
    src/mutexunlock.c

## PTHREAD_C_FILES
libposix_a_SOURCES += src/pthread.c src/pthreadsetcputime.c \
    src/pthreadgetcputime.c src/pthreadgetcpuclockid.c src/pthreadonce.c \
    src/pthreadequal.c src/pthreadself.c src/pthreadexit.c \
    src/pthreaddetach.c src/pthreadjoin.c src/pthreadcreate.c \
    src/pthreadattrsetdetachstate.c src/pthreadattrgetdetachstate.c \
    src/pthreadattrgetstackaddr.c src/pthreadattrsetstackaddr.c \
    src/pthreadattrgetstacksize.c src/pthreadattrsetstacksize.c \
    src/pthreadattrinit.c src/pthreadattrdestroy.c \
    src/pthreadsetschedparam.c src/pthreadgetschedparam.c \
    src/pthreadattrsetschedparam.c src/pthreadattrgetschedparam.c \
    src/pthreadattrgetschedpolicy.c src/pthreadattrsetschedpolicy.c \
    src/pthreadattrgetinheritsched.c src/pthreadattrsetinheritsched.c \
    src/pthreadattrgetscope.c src/pthreadattrsetscope.c \
    src/pthreadinitthreads.c

## PSIGNAL_C_FILES
libposix_a_SOURCES += src/psignal.c src/alarm.c src/kill.c src/killinfo.c \
    src/pause.c src/psignalclearprocesssignals.c \
    src/psignalsetprocesssignals.c src/psignalchecksignal.c \
    src/psignalclearsignals.c src/psignalunblockthread.c src/pthreadkill.c \
    src/pthreadsigmask.c src/sigaction.c src/sigaddset.c src/sigdelset.c \
    src/sigemptyset.c src/sigfillset.c src/sigismember.c src/sigpending.c \
    src/sigprocmask.c src/sigqueue.c src/sigsuspend.c src/sigtimedwait.c \
    src/sigwait.c src/sigwaitinfo.c src/signal_2.c src/ualarm.c

## RWLOCK_C_FILES
libposix_a_SOURCES += src/prwlock.c src/prwlockdestroy.c src/prwlockinit.c \
    src/prwlockrdlock.c src/prwlocktimedrdlock.c src/prwlocktimedwrlock.c \
    src/prwlocktryrdlock.c src/prwlocktrywrlock.c src/prwlockunlock.c \
    src/prwlockwrlock.c src/rwlockattrdestroy.c src/rwlockattrgetpshared.c \
    src/rwlockattrinit.c src/rwlockattrsetpshared.c \
    src/prwlocktranslatereturncode.c

## SEMAPHORE_C_FILES
libposix_a_SOURCES += src/semaphore.c src/semaphorecreatesupp.c \
    src/semaphoredeletesupp.c src/semaphoremp.c src/semaphorenametoid.c \
    src/semaphorewaitsupp.c src/semclose.c src/semdestroy.c \
    src/semgetvalue.c src/seminit.c src/semopen.c src/sempost.c \
    src/semtimedwait.c src/semtrywait.c src/semunlink.c src/semwait.c

## SPINLOCK_C_FILES
libposix_a_SOURCES += src/pspin.c src/pspindestroy.c src/pspininit.c \
    src/pspinlock.c src/pspinlocktranslatereturncode.c src/pspintrylock.c \
    src/pspinunlock.c

## TIME_C_FILES
libposix_a_SOURCES += src/adjtime.c src/posixtimespecabsolutetimeout.c \
    src/clockgetcpuclockid.c src/clockgetenableattr.c src/clockgetres.c \
    src/clockgettime.c src/clocksetenableattr.c src/clocksettime.c

# the timer manager needs to be split further but only after its
# dependence on the Classic API Timer Manager is removed.
## TIMER_C_FILES
libposix_a_SOURCES += src/ptimer.c src/ptimer1.c

EXTRA_DIST += src/README.mqueue

libposix_a_SOURCES += src/adasupp.c src/sched.c \
    src/sysconf.c src/types.c
endif
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
