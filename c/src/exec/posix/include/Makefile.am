#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4

# RTEMS unistd.h needs to be checked against newlib's and removed
#noinst_HEADERS= devctl.h intr.h
H_PIECES = pthread.h sched.h aio.h mqueue.h semaphore.h

noinst_HEADERS = $(H_PIECES)

TMPINSTALL_FILES = \
$(PROJECT_INCLUDE) \
$(noinst_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(TMPINSTALL_FILES)
debug-am: $(TMPINSTALL_FILES)
profile-am: $(TMPINSTALL_FILES)

SUBDIRS = rtems sys

EXTRA_DIST = intr.h devctl.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
