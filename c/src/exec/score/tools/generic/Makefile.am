#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

# FIXME: This script doesn't make sense to be installed
noinst_SCRIPTS=size_rtems

# HACK: install into build-tree
preinstall: install-exec-local

install-exec-local: size_rtems
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/bin
	$(INSTALL_SCRIPT) size_rtems $(PROJECT_ROOT)/${RTEMS_BSP}/bin

include $(top_srcdir)/../../../../../../automake/local.am
