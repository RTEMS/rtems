##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = condmp.h mqueuemp.h mutexmp.h pthreadmp.h semaphoremp.h
endif

H_FILES = cancel.h cond.h config.h key.h mqueue.h mutex.h posixapi.h \
    priority.h psignal.h pthread.h ptimer.h semaphore.h seterr.h threadsup.h \
    time.h timer.h $(MP_H_FILES)
#H_FILES= intr.h threadsup.h time.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/posix \
$(H_FILES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
