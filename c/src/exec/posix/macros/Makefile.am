##
## $Id$
##


include_rtems_posixdir = $(includedir)/rtems/posix

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

if MACROS
noinst_HEADERS = rtems/posix/intr.inl
include_rtems_posix_HEADERS = rtems/posix/cond.inl rtems/posix/key.inl \
    rtems/posix/mqueue.inl rtems/posix/mutex.inl \
    rtems/posix/pthread.inl rtems/posix/priority.inl \
    rtems/posix/semaphore.inl rtems/posix/timer.inl

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:%=$(PROJECT_INCLUDE)/%)
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
