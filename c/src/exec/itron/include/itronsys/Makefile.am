##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_PIECES = eventflags.h fmempool.h intr.h mbox.h msgbuffer.h network.h \
    port.h semaphore.h status.h sysmgmt.h task.h time.h types.h vmempool.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE)/itronsys \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/itronsys/%)

$(PROJECT_INCLUDE)/itronsys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/itronsys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
