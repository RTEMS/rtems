##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

MP_H_FILES = condmp.h mqueuemp.h mutexmp.h pthreadmp.h semaphoremp.h

STD_H_FILES = cancel.h cond.h config.h key.h mqueue.h mutex.h posixapi.h \
    priority.h psignal.h pthread.h ptimer.h semaphore.h threadsup.h \
    time.h timer.h $(MP_H_FILES)

UNUSED_H_FILES = intr.h threadsup.h

if HAS_MP
H_FILES = $(STD_H_FILES) $(MP_H_FILES)
else
H_FILES = $(STD_H_FILES)
endif

noinst_HEADERS = $(STD_H_FILES) $(MP_H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(UNUSED_H_FILES)

include $(top_srcdir)/../../../../automake/local.am
