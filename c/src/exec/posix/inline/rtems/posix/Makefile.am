##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
if INLINE
#I_FILES= intr.inl
I_FILES = cond.inl key.inl mqueue.inl mutex.inl pthread.inl priority.inl \
   semaphore.inl

noinst_HEADERS = $(I_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/posix \
$(I_FILES:%=$(PROJECT_INCLUDE)/rtems/posix/%)

$(PROJECT_INCLUDE)/rtems/posix:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/posix/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

EXTRA_DIST = intr.inl

include $(top_srcdir)/../../../../automake/local.am
