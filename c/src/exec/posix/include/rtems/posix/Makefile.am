##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_posixdir = $(includedir)/rtems/posix

MP_H_FILES = condmp.h mqueuemp.h mutexmp.h pthreadmp.h semaphoremp.h

STD_H_FILES = cancel.h cond.h config.h key.h mqueue.h mutex.h posixapi.h \
    priority.h psignal.h pthread.h ptimer.h semaphore.h threadsup.h time.h \
    timer.h $(MP_H_FILES)

noinst_HEADERS = intr.h threadsup.h

if HAS_MP
include_rtems_posix_HEADERS = $(STD_H_FILES) $(MP_H_FILES)
else
include_rtems_posix_HEADERS = $(STD_H_FILES)
endif

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(noinst_HEADERS)

include $(top_srcdir)/../../../../automake/local.am
