##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =
CLEANFILES =

if HAS_POSIX

# include

noinst_HEADERS = include/devctl.h include/intr.h

include_HEADERS = include/sched.h include/aio.h include/mqueue.h \
    include/semaphore.h

# include/rtems/posix

include_rtems_posixdir = $(includedir)/rtems/posix

include_rtems_posix_HEADERS = include/rtems/posix/cancel.h \
    include/rtems/posix/cond.h include/rtems/posix/config.h \
    include/rtems/posix/key.h include/rtems/posix/mqueue.h \
    include/rtems/posix/mutex.h include/rtems/posix/posixapi.h \
    include/rtems/posix/priority.h include/rtems/posix/psignal.h \
    include/rtems/posix/pthread.h include/rtems/posix/ptimer.h \
    include/rtems/posix/semaphore.h include/rtems/posix/threadsup.h \
    include/rtems/posix/time.h include/rtems/posix/timer.h

if HAS_MP
include_rtems_posix_HEADERS += include/rtems/posix/condmp.h \
    include/rtems/posix/mqueuemp.h include/rtems/posix/mutexmp.h \
    include/rtems/posix/pthreadmp.h include/rtems/posix/semaphoremp.h
endif

noinst_HEADERS += include/rtems/posix/intr.h \
    include/rtems/posix/threadsup.h

if INLINE
include_rtems_posix_HEADERS += inline/rtems/posix/cond.inl \
    inline/rtems/posix/key.inl inline/rtems/posix/mqueue.inl \
    inline/rtems/posix/mutex.inl inline/rtems/posix/pthread.inl \
    inline/rtems/posix/priority.inl inline/rtems/posix/semaphore.inl \
    inline/rtems/posix/timer.inl
noinst_HEADERS += inline/rtems/posix/intr.inl
else
include_rtems_posix_HEADERS += macros/rtems/posix/cond.inl \
    macros/rtems/posix/key.inl macros/rtems/posix/mqueue.inl \
    macros/rtems/posix/mutex.inl macros/rtems/posix/pthread.inl \
    macros/rtems/posix/priority.inl macros/rtems/posix/semaphore.inl \
    macros/rtems/posix/timer.inl
noinst_HEADERS += macros/rtems/posix/intr.inl
endif

## src

# following are semi-implemented and untested
UNUSED_C_FILES = src/devctl.c src/intr.c

# These are really in the stand but not really functional
BUILD_FOR_NOW_C_FILES = src/aio.c

ENOSYS_C_FILES = src/execl.c src/execle.c src/execlp.c src/execv.c \
    src/execve.c src/execvp.c src/fork.c src/pthreadatfork.c src/wait.c \
    src/waitpid.c

CANCEL_C_FILES = src/cancel.c src/cancelrun.c src/cleanuppop.c \
    src/cleanuppush.c src/setcancelstate.c src/setcanceltype.c \
    src/testcancel.c

CONDITION_VARIABLE_C_FILES = src/cond.c src/condattrdestroy.c \
    src/condattrgetpshared.c src/condattrinit.c src/condattrsetpshared.c \
    src/condbroadcast.c src/conddefaultattributes.c src/conddestroy.c \
    src/condinit.c src/condmp.c src/condsignal.c src/condsignalsupp.c \
    src/condtimedwait.c src/condwait.c src/condwaitsupp.c

KEY_C_FILES = src/key.c src/keycreate.c src/keydelete.c src/keygetspecific.c \
    src/keyrundestructors.c src/keysetspecific.c

MEMORY_C_FILES = src/getpagesize.c src/mprotect.c

MESSAGE_QUEUE_C_FILES = src/mqueue.c src/mqueueclose.c \
    src/mqueuecreatesupp.c src/mqueuedeletesupp.c src/mqueuegetattr.c \
    src/mqueuenametoid.c src/mqueuenotify.c src/mqueueopen.c \
    src/mqueuereceive.c src/mqueuerecvsupp.c src/mqueuesend.c \
    src/mqueuesendsupp.c src/mqueuesetattr.c src/mqueuetimedreceive.c \
    src/mqueuetimedsend.c src/mqueuetranslatereturncode.c \
    src/mqueueunlink.c

MUTEX_C_FILES = src/mutex.c src/mutexattrdestroy.c \
    src/mutexattrgetprioceiling.c src/mutexattrgetprotocol.c \
    src/mutexattrgetpshared.c src/mutexattrinit.c \
    src/mutexattrsetprioceiling.c src/mutexattrsetprotocol.c \
    src/mutexattrsetpshared.c src/mutexdefaultattributes.c \
    src/mutexdestroy.c src/mutexfromcorestatus.c src/mutexgetprioceiling.c \
    src/mutexinit.c src/mutexlock.c src/mutexlocksupp.c src/mutexmp.c \
    src/mutexsetprioceiling.c src/mutextimedlock.c src/mutextrylock.c \
    src/mutexunlock.c

PTHREAD_C_FILES = src/pthread.c src/pthreadsetcputime.c \
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
    src/pthreadattrgetscope.c src/pthreadattrsetscope.c

PSIGNAL_C_FILES = src/psignal.c src/alarm.c src/kill.c src/killinfo.c \
    src/pause.c src/psignalclearprocesssignals.c \
    src/psignalsetprocesssignals.c src/psignalchecksignal.c \
    src/psignalclearsignals.c src/psignalunblockthread.c src/pthreadkill.c \
    src/pthreadsigmask.c src/sigaction.c src/sigaddset.c src/sigdelset.c \
    src/sigemptyset.c src/sigfillset.c src/sigismember.c src/sigpending.c \
    src/sigprocmask.c src/sigqueue.c src/sigsuspend.c src/sigtimedwait.c \
    src/sigwait.c src/sigwaitinfo.c src/signal_2.c src/ualarm.c

SEMAPHORE_C_FILES = src/semaphore.c src/semaphorecreatesupp.c \
    src/semaphoredeletesupp.c src/semaphoremp.c src/semaphorenametoid.c \
    src/semaphorewaitsupp.c src/semclose.c src/semdestroy.c \
    src/semgetvalue.c src/seminit.c src/semopen.c src/sempost.c \
    src/semtimedwait.c src/semtrywait.c src/semunlink.c src/semwait.c

TIME_C_FILES = src/adjtime.c src/time.c src/posixtimespecsubtract.c \
    src/posixtimespectointerval.c src/posixintervaltotimespec.c \
    src/clockgetcpuclockid.c src/clockgetenableattr.c src/clockgetres.c \
    src/clockgettime.c src/clocksetenableattr.c src/clocksettime.c \
    src/nanosleep.c src/sleep.c src/usleep.c

# the timer manager needs to be split further but only after its
# dependence on the Classic API Timer Manager is removed.
TIMER_C_FILES = src/ptimer.c src/ptimer1.c

EXTRA_DIST += src/README.mqueue

C_FILES = src/adasupp.c $(CANCEL_C_FILES) $(CONDITION_VARIABLE_C_FILES) \
    $(KEY_C_FILES) $(MEMORY_C_FILES) $(MESSAGE_QUEUE_C_FILES) \
    $(MUTEX_C_FILES) $(PTHREAD_C_FILES) $(PSIGNAL_C_FILES) src/sched.c \
    $(SEMAPHORE_C_FILES) src/sysconf.c $(TIME_C_FILES) $(TIMER_C_FILES) \
    src/types.c $(ENOSYS_C_FILES) $(BUILD_FOR_NOW_C_FILES)

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += -D__RTEMS_INSIDE__

EXTRA_LIBRARIES = libposix.a
CLEANFILES += libposix.a
libposix_a_SOURCES = $(C_FILES)
libposix_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libposix_g.a
CLEANFILES += libposix_g.a
libposix_g_a_SOURCES = $(libposix_a_SOURCES)
libposix_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

all-local: $(PREINSTALL_FILES) libposix$(LIB_VARIANT).a
endif

# temporary so we can see how many things are left to implement
not:
	grep -i NOT_IMPL $(C_FILES:%=$(srcdir)/%) | grep -v MP_NOT_IMPL
	@echo
	@echo
	@echo
	@echo `grep -i NOT_IMPL $(C_FILES:%=$(srcdir)/%) | grep -v MP_NOT_IMPL | wc -l ` places marked not implemented
	@echo `wc -l $(C_FILES:%=$(srcdir)/%) | grep total` lines of C code to test

EXTRA_DIST += $(C_FILES) $(UNUSED_C_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_POSIX
$(PROJECT_INCLUDE)/sched.h: include/sched.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sched.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sched.h

$(PROJECT_INCLUDE)/aio.h: include/aio.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/aio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/aio.h

$(PROJECT_INCLUDE)/mqueue.h: include/mqueue.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/mqueue.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mqueue.h

$(PROJECT_INCLUDE)/semaphore.h: include/semaphore.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/semaphore.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/semaphore.h

$(PROJECT_INCLUDE)/rtems/posix/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/posix
	@: > $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/posix/cancel.h: include/rtems/posix/cancel.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/cancel.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/cancel.h

$(PROJECT_INCLUDE)/rtems/posix/cond.h: include/rtems/posix/cond.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/cond.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/cond.h

$(PROJECT_INCLUDE)/rtems/posix/config.h: include/rtems/posix/config.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/config.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/config.h

$(PROJECT_INCLUDE)/rtems/posix/key.h: include/rtems/posix/key.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/key.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/key.h

$(PROJECT_INCLUDE)/rtems/posix/mqueue.h: include/rtems/posix/mqueue.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mqueue.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mqueue.h

$(PROJECT_INCLUDE)/rtems/posix/mutex.h: include/rtems/posix/mutex.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mutex.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mutex.h

$(PROJECT_INCLUDE)/rtems/posix/posixapi.h: include/rtems/posix/posixapi.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/posixapi.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/posixapi.h

$(PROJECT_INCLUDE)/rtems/posix/priority.h: include/rtems/posix/priority.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/priority.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/priority.h

$(PROJECT_INCLUDE)/rtems/posix/psignal.h: include/rtems/posix/psignal.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/psignal.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/psignal.h

$(PROJECT_INCLUDE)/rtems/posix/pthread.h: include/rtems/posix/pthread.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/pthread.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/pthread.h

$(PROJECT_INCLUDE)/rtems/posix/ptimer.h: include/rtems/posix/ptimer.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/ptimer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/ptimer.h

$(PROJECT_INCLUDE)/rtems/posix/semaphore.h: include/rtems/posix/semaphore.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/semaphore.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/semaphore.h

$(PROJECT_INCLUDE)/rtems/posix/threadsup.h: include/rtems/posix/threadsup.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/threadsup.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/threadsup.h

$(PROJECT_INCLUDE)/rtems/posix/time.h: include/rtems/posix/time.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/time.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/time.h

$(PROJECT_INCLUDE)/rtems/posix/timer.h: include/rtems/posix/timer.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/timer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/timer.h

if HAS_MP
$(PROJECT_INCLUDE)/rtems/posix/condmp.h: include/rtems/posix/condmp.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/condmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/condmp.h

$(PROJECT_INCLUDE)/rtems/posix/mqueuemp.h: include/rtems/posix/mqueuemp.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mqueuemp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mqueuemp.h

$(PROJECT_INCLUDE)/rtems/posix/mutexmp.h: include/rtems/posix/mutexmp.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mutexmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mutexmp.h

$(PROJECT_INCLUDE)/rtems/posix/pthreadmp.h: include/rtems/posix/pthreadmp.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/pthreadmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/pthreadmp.h

$(PROJECT_INCLUDE)/rtems/posix/semaphoremp.h: include/rtems/posix/semaphoremp.h $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/semaphoremp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/semaphoremp.h
endif

if INLINE
$(PROJECT_INCLUDE)/rtems/posix/cond.inl: inline/rtems/posix/cond.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/cond.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/cond.inl

$(PROJECT_INCLUDE)/rtems/posix/key.inl: inline/rtems/posix/key.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/key.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/key.inl

$(PROJECT_INCLUDE)/rtems/posix/mqueue.inl: inline/rtems/posix/mqueue.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mqueue.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mqueue.inl

$(PROJECT_INCLUDE)/rtems/posix/mutex.inl: inline/rtems/posix/mutex.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mutex.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mutex.inl

$(PROJECT_INCLUDE)/rtems/posix/pthread.inl: inline/rtems/posix/pthread.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/pthread.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/pthread.inl

$(PROJECT_INCLUDE)/rtems/posix/priority.inl: inline/rtems/posix/priority.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/priority.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/priority.inl

$(PROJECT_INCLUDE)/rtems/posix/semaphore.inl: inline/rtems/posix/semaphore.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/semaphore.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/semaphore.inl

$(PROJECT_INCLUDE)/rtems/posix/timer.inl: inline/rtems/posix/timer.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/timer.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/timer.inl

else
$(PROJECT_INCLUDE)/rtems/posix/cond.inl: macros/rtems/posix/cond.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/cond.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/cond.inl

$(PROJECT_INCLUDE)/rtems/posix/key.inl: macros/rtems/posix/key.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/key.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/key.inl

$(PROJECT_INCLUDE)/rtems/posix/mqueue.inl: macros/rtems/posix/mqueue.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mqueue.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mqueue.inl

$(PROJECT_INCLUDE)/rtems/posix/mutex.inl: macros/rtems/posix/mutex.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/mutex.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/mutex.inl

$(PROJECT_INCLUDE)/rtems/posix/pthread.inl: macros/rtems/posix/pthread.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/pthread.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/pthread.inl

$(PROJECT_INCLUDE)/rtems/posix/priority.inl: macros/rtems/posix/priority.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/priority.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/priority.inl

$(PROJECT_INCLUDE)/rtems/posix/semaphore.inl: macros/rtems/posix/semaphore.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/semaphore.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/semaphore.inl

$(PROJECT_INCLUDE)/rtems/posix/timer.inl: macros/rtems/posix/timer.inl $(PROJECT_INCLUDE)/rtems/posix/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/posix/timer.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix/timer.inl
endif
endif

CLEANFILES += $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
