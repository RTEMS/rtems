##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

EXTRA_DIST =

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/%.inl: $(INLINEdir)/%.inl
	$(INSTALL_DATA) $< $@

if HAS_POSIX

PREINSTALL_FILES = $(PROJECT_INCLUDE)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix

# include

noinst_HEADERS = include/devctl.h include/intr.h
noinst_HEADERS += src/config.h

include_HEADERS = include/sched.h include/aio.h include/mqueue.h include/semaphore.h

PREINSTALL_FILES += $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

# include/rtems/posix

include_rtems_posixdir = $(includedir)/rtems/posix

include_H_FILES = include/rtems/posix/cancel.h include/rtems/posix/cond.h include/rtems/posix/config.h \
    include/rtems/posix/key.h include/rtems/posix/mqueue.h include/rtems/posix/mutex.h \
    include/rtems/posix/posixapi.h include/rtems/posix/priority.h include/rtems/posix/psignal.h \
    include/rtems/posix/pthread.h include/rtems/posix/ptimer.h include/rtems/posix/semaphore.h \
    include/rtems/posix/threadsup.h include/rtems/posix/time.h include/rtems/posix/timer.h
PREINSTALL_FILES += $(include_H_FILES:include/%=$(PROJECT_INCLUDE)/%)
include_rtems_posix_HEADERS = $(include_H_FILES)

include_MP_H_FILES = include/rtems/posix/condmp.h include/rtems/posix/mqueuemp.h include/rtems/posix/mutexmp.h \
    include/rtems/posix/pthreadmp.h include/rtems/posix/semaphoremp.h
if HAS_MP
PREINSTALL_FILES += $(include_MP_H_FILES:include/%=$(PROJECT_INCLUDE)/%)
include_rtems_posix_HEADERS += $(include_MP_H_FILES)
endif

noinst_HEADERS += include/rtems/posix/intr.h include/rtems/posix/threadsup.h
noinst_HEADERS += $(include_H_FILES) $(include_MP_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## inline

inline_H_FILES = inline/rtems/posix/cond.inl inline/rtems/posix/key.inl \
    inline/rtems/posix/mqueue.inl inline/rtems/posix/mutex.inl \
    inline/rtems/posix/pthread.inl inline/rtems/posix/priority.inl \
    inline/rtems/posix/semaphore.inl inline/rtems/posix/timer.inl
noinst_HEADERS += inline/rtems/posix/intr.inl $(inline_H_FILES)

if INLINE
PREINSTALL_FILES += $(inline_H_FILES:inline/%=$(PROJECT_INCLUDE)/%)
include_rtems_posix_HEADERS += $(inline_H_FILES)
endif

## macros

macros_H_FILES = macros/rtems/posix/cond.inl macros/rtems/posix/key.inl \
    macros/rtems/posix/mqueue.inl macros/rtems/posix/mutex.inl \
    macros/rtems/posix/pthread.inl macros/rtems/posix/priority.inl \
    macros/rtems/posix/semaphore.inl macros/rtems/posix/timer.inl
noinst_HEADERS += macros/rtems/posix/intr.inl $(macros_H_FILES)

if MACROS
PREINSTALL_FILES += $(macros_H_FILES:macros/%=$(PROJECT_INCLUDE)/%)
include_rtems_posix_HEADERS += $(macros_H_FILES)
endif

## src
LIB=$(ARCH)/libposix.a

# following are semi-implemented and untested
UNUSED_C_FILES = src/devctl.c src/intr.c

# These are really in the stand but not really functional
BUILD_FOR_NOW_C_FILES = src/aio.c

ENOSYS_C_FILES = src/execl.c src/execle.c src/execlp.c src/execv.c src/execve.c src/execvp.c src/fork.c \
    src/pthreadatfork.c src/wait.c src/waitpid.c

CANCEL_C_FILES = src/cancel.c src/cancelrun.c src/cleanuppop.c src/cleanuppush.c \
    src/setcancelstate.c src/setcanceltype.c src/testcancel.c

CONDITION_VARIABLE_C_FILES = src/cond.c src/condattrdestroy.c src/condattrgetpshared.c \
    src/condattrinit.c src/condattrsetpshared.c src/condbroadcast.c \
    src/conddefaultattributes.c src/conddestroy.c src/condinit.c src/condmp.c src/condsignal.c \
    src/condsignalsupp.c src/condtimedwait.c src/condwait.c src/condwaitsupp.c

ID_C_FILES = src/getegid.c src/geteuid.c src/getgid.c src/getgroups.c src/getlogin.c src/getpgrp.c \
    src/getpid.c src/getppid.c src/getuid.c src/setpgid.c src/setsid.c

KEY_C_FILES = src/key.c src/keycreate.c src/keydelete.c src/keygetspecific.c \
    src/keyrundestructors.c src/keysetspecific.c

MEMORY_C_FILES = src/getpagesize.c src/mprotect.c

MESSAGE_QUEUE_C_FILES = src/mqueue.c src/mqueueclose.c src/mqueuecreatesupp.c \
    src/mqueuedeletesupp.c src/mqueuegetattr.c src/mqueuenametoid.c src/mqueuenotify.c \
    src/mqueueopen.c src/mqueuereceive.c src/mqueuerecvsupp.c src/mqueuesend.c \
    src/mqueuesendsupp.c src/mqueuesetattr.c src/mqueuetimedreceive.c src/mqueuetimedsend.c \
    src/mqueuetranslatereturncode.c src/mqueueunlink.c

MUTEX_C_FILES = src/mutex.c src/mutexattrdestroy.c src/mutexattrgetprioceiling.c \
    src/mutexattrgetprotocol.c src/mutexattrgetpshared.c src/mutexattrinit.c \
    src/mutexattrsetprioceiling.c src/mutexattrsetprotocol.c src/mutexattrsetpshared.c \
    src/mutexdefaultattributes.c src/mutexdestroy.c src/mutexfromcorestatus.c \
    src/mutexgetprioceiling.c src/mutexinit.c src/mutexlock.c src/mutexlocksupp.c src/mutexmp.c \
    src/mutexsetprioceiling.c src/mutextimedlock.c src/mutextrylock.c src/mutexunlock.c

PTHREAD_C_FILES = src/pthread.c src/pthreadsetcputime.c src/pthreadgetcputime.c \
    src/pthreadgetcpuclockid.c src/pthreadonce.c src/pthreadequal.c src/pthreadself.c \
    src/pthreadexit.c src/pthreaddetach.c src/pthreadjoin.c src/pthreadcreate.c \
    src/pthreadattrsetdetachstate.c src/pthreadattrgetdetachstate.c \
    src/pthreadattrgetstackaddr.c src/pthreadattrsetstackaddr.c \
    src/pthreadattrgetstacksize.c src/pthreadattrsetstacksize.c src/pthreadattrinit.c \
    src/pthreadattrdestroy.c src/pthreadsetschedparam.c src/pthreadgetschedparam.c \
    src/pthreadattrsetschedparam.c src/pthreadattrgetschedparam.c \
    src/pthreadattrgetschedpolicy.c src/pthreadattrsetschedpolicy.c \
    src/pthreadattrgetinheritsched.c src/pthreadattrsetinheritsched.c \
    src/pthreadattrgetscope.c src/pthreadattrsetscope.c

PSIGNAL_C_FILES = src/psignal.c src/alarm.c src/kill.c src/killinfo.c src/pause.c \
    src/psignalclearprocesssignals.c src/psignalsetprocesssignals.c \
    src/psignalchecksignal.c src/psignalclearsignals.c src/psignalunblockthread.c \
    src/pthreadkill.c src/pthreadsigmask.c src/sigaction.c src/sigaddset.c src/sigdelset.c \
    src/sigemptyset.c src/sigfillset.c src/sigismember.c src/sigpending.c src/sigprocmask.c \
    src/sigqueue.c src/sigsuspend.c src/sigtimedwait.c src/sigwait.c src/sigwaitinfo.c \
    src/signal_2.c

SEMAPHORE_C_FILES = src/semaphore.c src/semaphorecreatesupp.c src/semaphoredeletesupp.c \
    src/semaphoremp.c src/semaphorenametoid.c src/semaphorewaitsupp.c src/semclose.c \
    src/semdestroy.c src/semgetvalue.c src/seminit.c src/semopen.c src/sempost.c src/semtimedwait.c \
    src/semtrywait.c src/semunlink.c src/semwait.c

TIME_C_FILES = src/adjtime.c src/time.c src/posixtimespecsubtract.c \
    src/posixtimespectointerval.c src/posixintervaltotimespec.c src/clockgetcpuclockid.c \
    src/clockgetenableattr.c src/clockgetres.c src/clockgettime.c src/clocksetenableattr.c \
    src/clocksettime.c src/nanosleep.c src/sleep.c

# the timer manager needs to be split further but only after its
# dependence on the Classic API Timer Manager is removed.
TIMER_C_FILES = src/ptimer.c src/ptimer1.c

EXTRA_DIST += src/README.mqueue

C_FILES = src/adasupp.c $(CANCEL_C_FILES) $(CONDITION_VARIABLE_C_FILES) \
    $(ID_C_FILES) $(KEY_C_FILES) $(MEMORY_C_FILES) $(MESSAGE_QUEUE_C_FILES) \
    $(MUTEX_C_FILES) $(PTHREAD_C_FILES) $(PSIGNAL_C_FILES) src/sched.c \
    $(SEMAPHORE_C_FILES) src/sysconf.c $(TIME_C_FILES) $(TIMER_C_FILES) src/types.c \
    $(ENOSYS_C_FILES) $(BUILD_FOR_NOW_C_FILES)
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -I$(srcdir)/src -D__RTEMS_INSIDE__

all-local: ${ARCH} $(PREINSTALL_FILES) $(LIB)

$(LIB): ${OBJS}
	$(make-library)
endif

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

# temporary so we can see how many things are left to implement
not:
	grep -i NOT_IMPL $(C_FILES:%=$(srcdir)/%) | grep -v MP_NOT_IMPL
	@echo
	@echo
	@echo
	@echo `grep -i NOT_IMPL $(C_FILES:%=$(srcdir)/%) | grep -v MP_NOT_IMPL | wc -l ` places marked not implemented
	@echo `wc -l $(C_FILES:%=$(srcdir)/%) | grep total` lines of C code to test

EXTRA_DIST += $(C_FILES) $(UNUSED_C_FILES)

include $(top_srcdir)/automake/local.am
