#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS=psim runtest

# HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/tests
	$(INSTALL_SCRIPT) $(srcdir)/runtest $(PROJECT_ROOT)/${RTEMS_BSP}/tests/runtest
	$(INSTALL_SCRIPT) $(srcdir)/psim $(PROJECT_ROOT)/${RTEMS_BSP}/tests/psim
	$(INSTALL_SCRIPT) $(srcdir)/psim $(PROJECT_ROOT)/${RTEMS_BSP}/tests/psim-gdb

include $(top_srcdir)/../../../../../../../automake/local.am
