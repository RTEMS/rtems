##
##  $Id$
##


INCLUDES = -I.

# following are semi-implemented and untested
UNUSED_C_FILES = devctl.c intr.c

# These are really in the stand but not really functional
BUILD_FOR_NOW_C_FILES = aio.c

ENOSYS_C_FILES = execl.c execle.c execlp.c execv.c execve.c execvp.c fork.c \
    pthreadatfork.c wait.c waitpid.c

CANCEL_C_FILES = cancel.c cancelrun.c cleanuppop.c cleanuppush.c \
    setcancelstate.c setcanceltype.c testcancel.c

CONDITION_VARIABLE_C_FILES = cond.c condattrdestroy.c condattrgetpshared.c \
    condattrinit.c condattrsetpshared.c condbroadcast.c \
    conddefaultattributes.c conddestroy.c condinit.c condmp.c condsignal.c \
    condsignalsupp.c condtimedwait.c condwait.c condwaitsupp.c

ID_C_FILES = getegid.c geteuid.c getgid.c getgroups.c getlogin.c getpgrp.c \
    getpid.c getppid.c getuid.c setpgid.c setsid.c

KEY_C_FILES = key.c keycreate.c keydelete.c keygetspecific.c \
    keyrundestructors.c keysetspecific.c

MEMORY_C_FILES = getpagesize.c mprotect.c

MESSAGE_QUEUE_C_FILES = mqueue.c mqueueclose.c mqueuecreatesupp.c \
    mqueuedeletesupp.c mqueuegetattr.c mqueuenametoid.c mqueuenotify.c \
    mqueueopen.c mqueuereceive.c mqueuerecvsupp.c mqueuesend.c \
    mqueuesendsupp.c mqueuesetattr.c mqueuetimedreceive.c mqueuetimedsend.c \
    mqueuetranslatereturncode.c mqueueunlink.c

MUTEX_C_FILES = mutex.c mutexattrdestroy.c mutexattrgetprioceiling.c \
    mutexattrgetprotocol.c mutexattrgetpshared.c mutexattrinit.c \
    mutexattrsetprioceiling.c mutexattrsetprotocol.c mutexattrsetpshared.c \
    mutexdefaultattributes.c mutexdestroy.c mutexfromcorestatus.c \
    mutexgetprioceiling.c mutexinit.c mutexlock.c mutexlocksupp.c mutexmp.c \
    mutexsetprioceiling.c mutextimedlock.c mutextrylock.c mutexunlock.c

PTHREAD_C_FILES = pthread.c pthreadsetcputime.c pthreadgetcputime.c \
    pthreadgetcpuclockid.c pthreadonce.c pthreadequal.c pthreadself.c \
    pthreadexit.c pthreaddetach.c pthreadjoin.c pthreadcreate.c \
    pthreadattrsetdetachstate.c pthreadattrgetdetachstate.c \
    pthreadattrgetstackaddr.c pthreadattrsetstackaddr.c \
    pthreadattrgetstacksize.c pthreadattrsetstacksize.c pthreadattrinit.c \
    pthreadattrdestroy.c pthreadsetschedparam.c pthreadgetschedparam.c \
    pthreadattrsetschedparam.c pthreadattrgetschedparam.c \
    pthreadattrgetschedpolicy.c pthreadattrsetschedpolicy.c \
    pthreadattrgetinheritsched.c pthreadattrsetinheritsched.c \
    pthreadattrgetscope.c pthreadattrsetscope.c

PSIGNAL_C_FILES = psignal.c alarm.c kill.c killinfo.c pause.c \
    psignalclearprocesssignals.c psignalsetprocesssignals.c \
    psignalchecksignal.c psignalclearsignals.c psignalunblockthread.c \
    pthreadkill.c pthreadsigmask.c sigaction.c sigaddset.c sigdelset.c \
    sigemptyset.c sigfillset.c sigismember.c sigpending.c sigprocmask.c \
    sigqueue.c sigsuspend.c sigtimedwait.c sigwait.c sigwaitinfo.c \
    signal_2.c

SEMAPHORE_C_FILES = semaphore.c semaphorecreatesupp.c semaphoredeletesupp.c \
    semaphoremp.c semaphorenametoid.c semaphorewaitsupp.c semclose.c \
    semdestroy.c semgetvalue.c seminit.c semopen.c sempost.c semtimedwait.c \
    semtrywait.c semunlink.c semwait.c

TIME_C_FILES = time.c posixtimespecsubtract.c posixtimespectointerval.c \
    posixintervaltotimespec.c clockgetcpuclockid.c clockgetenableattr.c \
    clockgetres.c clockgettime.c clocksetenableattr.c clocksettime.c \
    nanosleep.c sleep.c

# the timer manager needs to be split further but only after its
# dependence on the Classic API Timer Manager is removed.
TIMER_C_FILES = ptimer.c ptimer1.c

C_FILES = adasupp.c $(CANCEL_C_FILES) $(CONDITION_VARIABLE_C_FILES) \
    $(ID_C_FILES) $(KEY_C_FILES) $(MEMORY_C_FILES) $(MESSAGE_QUEUE_C_FILES) \
    $(MUTEX_C_FILES) $(PTHREAD_C_FILES) $(PSIGNAL_C_FILES) sched.c \
    $(SEMAPHORE_C_FILES) sysconf.c $(TIME_C_FILES) $(TIMER_C_FILES) types.c \
    $(ENOSYS_C_FILES) $(BUILD_FOR_NOW_C_FILES)
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/multilib.am
include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} ${OBJS}

# temporary so we can see how many things are left to implement
not:
	grep -i NOT_IMPL $(C_FILES) | grep -v MP_NOT_IMPL
	@echo
	@echo
	@echo
	@echo `grep -i NOT_IMPL $(C_FILES) | grep -v MP_NOT_IMPL | wc -l ` places marked not implemented
	@echo `wc -l $(C_FILES) | grep total` lines of C code to test

EXTRA_DIST = $(C_FILES) $(UNUSED_C_FILES)

include $(top_srcdir)/../../../../automake/local.am
