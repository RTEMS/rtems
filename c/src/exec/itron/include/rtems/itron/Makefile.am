##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_itrondir = $(includedir)/rtems/itron

include_rtems_itron_HEADERS = config.h eventflags.h fmempool.h intr.h itronapi.h mbox.h \
    msgbuffer.h network.h object.h port.h semaphore.h sysmgmt.h task.h \
    time.h vmempool.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/itron \
    $(include_rtems_itron_HEADERS:%=$(PROJECT_INCLUDE)/rtems/itron/%)

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = README

include $(top_srcdir)/../../../../automake/local.am
