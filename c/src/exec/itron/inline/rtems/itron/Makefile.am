##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
if INLINE
I_PIECES = eventflags.inl fmempool.inl intr.inl mbox.inl msgbuffer.inl \
    network.inl port.inl semaphore.inl sysmgmt.inl task.inl \
    time.inl vmempool.inl

noinst_HEADERS = $(I_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/itron \
$(I_PIECES:%=$(PROJECT_INCLUDE)/rtems/itron/%)

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
