##
## $Id$
##


$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/sys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

# include
noinst_HEADERS = devctl.h intr.h

include_HEADERS = sched.h aio.h mqueue.h semaphore.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

# rtems/posix

include_rtems_posixdir = $(includedir)/rtems/posix

MP_H_FILES = rtems/posix/condmp.h rtems/posix/mqueuemp.h rtems/posix/mutexmp.h \
    rtems/posix/pthreadmp.h rtems/posix/semaphoremp.h

STD_H_FILES = rtems/posix/cancel.h rtems/posix/cond.h rtems/posix/config.h \
    rtems/posix/key.h rtems/posix/mqueue.h rtems/posix/mutex.h \
    rtems/posix/posixapi.h rtems/posix/priority.h rtems/posix/psignal.h \
    rtems/posix/pthread.h rtems/posix/ptimer.h rtems/posix/semaphore.h \
    rtems/posix/threadsup.h rtems/posix/time.h rtems/posix/timer.h \
    $(MP_H_FILES)

noinst_HEADERS += rtems/posix/intr.h rtems/posix/threadsup.h

if HAS_MP
include_rtems_posix_HEADERS = $(STD_H_FILES) $(MP_H_FILES)
else
include_rtems_posix_HEADERS = $(STD_H_FILES)
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:%=$(PROJECT_INCLUDE)/%)

## General

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(noinst_HEADERS)

include $(top_srcdir)/../automake/local.am
