##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = config.h eventflags.h fmempool.h intr.h itronapi.h mbox.h \
    msgbuffer.h network.h object.h port.h semaphore.h sysmgmt.h \
    task.h time.h vmempool.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/rtems/itron \
$(H_PIECES:%=$(PROJECT_INCLUDE)/rtems/itron/%)

$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

EXTRA_DIST = README

include $(top_srcdir)/../../../../automake/local.am
