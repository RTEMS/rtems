##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_itronsysdir = $(includedir)/itronsys

include_itronsys_HEADERS = eventflags.h fmempool.h intr.h mbox.h msgbuffer.h network.h port.h \
    semaphore.h status.h sysmgmt.h task.h time.h types.h vmempool.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys \
    $(include_itronsys_HEADERS:%=$(PROJECT_INCLUDE)/itronsys/%)

$(PROJECT_INCLUDE)/itronsys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/itronsys/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
