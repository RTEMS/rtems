#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_PROGRAMS=gensize

gensize_SOURCES=gensize.c

# We use files that have not been installed yet.
CPU_DIR=$(srcdir)/../../cpu/$(RTEMS_CPU)

INCLUDES = \
-I$(PROJECT_ROOT)/${RTEMS_BSP}/include \
-I$(CPU_DIR)

# HACK: install into the build-tree
preinstall: install-exec-local

install-exec-local: gensize
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/bin
	$(INSTALL_PROGRAM) gensize $(PROJECT_ROOT)/${RTEMS_BSP}/bin

include $(top_srcdir)/../../../../../../automake/local.am
