#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_PROGRAMS= \
print_dump

print_dump_SOURCES= \
print_dump.c

# HACK: install to build-tree
install-exec-local: print_dump
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/bin
	$(INSTALL_PROGRAM) print_dump $(PROJECT_ROOT)/${RTEMS_BSP}/bin

include $(top_srcdir)/../../../../../../../automake/local.am
