##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

## RTEMS unistd.h needs to be checked against newlib's and removed
UNUSED_H_FILES = devctl.h intr.h unistd.h

H_FILES = pthread.h sched.h aio.h mqueue.h semaphore.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)
debug-am: $(PREINSTALL_FILES)
profile-am: $(PREINSTALL_FILES)

SUBDIRS = rtems sys

EXTRA_DIST = $(UNUSED_H_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
