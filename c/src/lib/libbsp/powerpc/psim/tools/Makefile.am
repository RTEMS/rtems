##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS=psim runtest

## HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/psim/tests
	$(INSTALL_SCRIPT) $(srcdir)/runtest $(PROJECT_ROOT)/psim/tests/runtest
	$(INSTALL_SCRIPT) $(srcdir)/psim $(PROJECT_ROOT)/psim/tests/psim
	$(INSTALL_SCRIPT) $(srcdir)/psim $(PROJECT_ROOT)/psim/tests/psim-gdb

include $(top_srcdir)/../../../../../../../automake/local.am
