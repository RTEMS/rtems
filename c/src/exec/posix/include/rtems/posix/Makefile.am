##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

## We only build multiprocessing related files if HAS_MP was defined
MP_H_PIECES_yes_V = condmp.h mqueuemp.h mutexmp.h pthreadmp.h semaphoremp.h
MP_H_PIECES = $(MP_H_PIECES_$(HAS_MP)_V)

H_PIECES = cancel.h cond.h config.h key.h mqueue.h mutex.h posixapi.h \
    priority.h psignal.h pthread.h ptimer.h semaphore.h seterr.h threadsup.h \
    time.h $(MP_H_PIECES)
#H_PIECES= intr.h threadsup.h time.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/posix \
$(H_PIECES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
