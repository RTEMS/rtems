##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = config.h eventflags.h fmempool.h intr.h itronapi.h mbox.h \
    msgbuffer.h network.h object.h port.h semaphore.h sysmgmt.h task.h \
    time.h vmempool.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron \
    $(H_FILES:%=$(PROJECT_INCLUDE)/rtems/itron/%)

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = README

include $(top_srcdir)/../../../../automake/local.am
