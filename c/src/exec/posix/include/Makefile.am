##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

noinst_HEADERS = devctl.h intr.h

include_HEADERS = sched.h aio.h mqueue.h semaphore.h

PREINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

SUBDIRS = rtems sys

EXTRA_DIST = $(noinst_HEADERS)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
