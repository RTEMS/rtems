##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_posixdir = $(includedir)/rtems/posix

I_FILES = cond.inl intr.inl key.inl mqueue.inl mutex.inl priority.inl \
    pthread.inl semaphore.inl timer.inl

if MACROS
include_rtems_posix_HEADERS = $(I_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(I_FILES)

include $(top_srcdir)/../../../../automake/local.am
