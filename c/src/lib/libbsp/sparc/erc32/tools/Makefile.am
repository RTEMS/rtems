#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS=runtest

#HACK: install to build-tree
install-exec-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/tests
	$(INSTALL_SCRIPT) runtest $(PROJECT_ROOT)/${RTEMS_BSP}/tests/runtest

include $(top_srcdir)/../../../../../../../automake/local.am
