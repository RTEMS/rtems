#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

EXTRA_DIST = Spec.doc

noinst_PROGRAMS = \
bin2boot \
binpatch

bin2boot_SOURCES = bin2boot.c
binpatch_SOURCES = binpatch.c

# HACK: install into build-tree
preinstall: install-exec-local

install-exec-local: $(PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools
	$(INSTALL_PROGRAM) bin2boot $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools
	$(INSTALL_PROGRAM) binpatch $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools

include $(top_srcdir)/../../../../../../../automake/local.am
