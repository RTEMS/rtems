#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_PROGRAMS=sload

sload_SOURCES = sload.c

# HACK: install to build-tree
install-exec-local: $(PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/bin
	$(INSTALL_PROGRAM) $(PROGRAMS) $(PROJECT_ROOT)/${RTEMS_BSP}/bin

include $(top_srcdir)/../../../../../../../automake/local.am
