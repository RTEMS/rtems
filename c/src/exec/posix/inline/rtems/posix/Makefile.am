##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

#I_FILES= intr.inl
I_FILES = cond.inl key.inl mqueue.inl mutex.inl pthread.inl priority.inl \
    semaphore.inl

noinst_HEADERS = $(I_FILES)

if INLINE
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/posix \
    $(I_FILES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(PREINSTALL_FILES)

EXTRA_DIST = intr.inl

include $(top_srcdir)/../../../../automake/local.am
