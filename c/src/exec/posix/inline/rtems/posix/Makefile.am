##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_posixdir = $(includedir)/rtems/posix

noinst_HEADERS = intr.inl
I_FILES = cond.inl key.inl mqueue.inl mutex.inl pthread.inl priority.inl \
    semaphore.inl timer.inl

if INLINE
include_rtems_posix_HEADERS = $(I_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(include_rtems_posix_HEADERS:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = intr.inl $(I_FILES)

include $(top_srcdir)/../../../../automake/local.am
