##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_itrondir = $(includedir)/rtems/itron

I_FILES = eventflags.inl fmempool.inl intr.inl mbox.inl msgbuffer.inl \
    network.inl port.inl semaphore.inl sysmgmt.inl task.inl time.inl \
    vmempool.inl

if INLINE
include_rtems_itron_HEADERS = $(I_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron \
    $(include_rtems_itron_HEADERS:%=$(PROJECT_INCLUDE)/rtems/itron/%)

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = $(I_FILES)

include $(top_srcdir)/../../../../automake/local.am
