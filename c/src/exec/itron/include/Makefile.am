##
## $Id$
##


$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/itronsys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

## include

include_HEADERS = itron.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

## itronsys

include_itronsysdir = $(includedir)/itronsys

include_itronsys_HEADERS = itronsys/eventflags.h itronsys/fmempool.h \
    itronsys/intr.h itronsys/mbox.h itronsys/msgbuffer.h itronsys/network.h \
    itronsys/port.h itronsys/semaphore.h itronsys/status.h itronsys/sysmgmt.h \
    itronsys/task.h itronsys/time.h itronsys/types.h itronsys/vmempool.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys \
    $(include_itronsys_HEADERS:%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST = itronsys/README

## rtems/itron
include_rtems_itrondir = $(includedir)/rtems/itron

include_rtems_itron_HEADERS = rtems/itron/config.h rtems/itron/eventflags.h \
    rtems/itron/fmempool.h rtems/itron/intr.h rtems/itron/itronapi.h \
    rtems/itron/mbox.h rtems/itron/msgbuffer.h rtems/itron/network.h \
    rtems/itron/object.h rtems/itron/port.h rtems/itron/semaphore.h \
    rtems/itron/sysmgmt.h rtems/itron/task.h rtems/itron/time.h \
    rtems/itron/vmempool.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron \
    $(include_rtems_itron_HEADERS:%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST += rtems/itron/README

## General
all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
