##
## $Id$
##


include_rtems_itrondir = $(includedir)/rtems/itron

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

if INLINE
include_rtems_itron_HEADERS = rtems/itron/eventflags.inl \
    rtems/itron/fmempool.inl rtems/itron/intr.inl rtems/itron/mbox.inl \
    rtems/itron/msgbuffer.inl rtems/itron/network.inl rtems/itron/port.inl \
    rtems/itron/semaphore.inl rtems/itron/sysmgmt.inl rtems/itron/task.inl \
    rtems/itron/time.inl rtems/itron/vmempool.inl

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/itron \
    $(include_rtems_itron_HEADERS:%=$(PROJECT_INCLUDE)/%)
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../automake/local.am
