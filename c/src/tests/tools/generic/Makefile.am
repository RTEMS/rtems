#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS=difftest sorttimes

# HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/tests
	$(INSTALL_SCRIPT) difftest $(PROJECT_ROOT)/${RTEMS_BSP}/tests
	$(INSTALL_SCRIPT) sorttimes $(PROJECT_ROOT)/${RTEMS_BSP}/tests

include $(top_srcdir)/../../../../../automake/local.am
