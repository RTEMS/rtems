#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

noinst_PROGRAMS=genoffsets

genoffsets_SOURCES = \
genoffsets.c

# We use files that have not been installed yet.

CPU_DIR=../../cpu/$(RTEMS_CPU)

INCLUDES = \
-I$(PROJECT_INCLUDE) \
-I$(CPU_DIR)

# HACK: install to build-tree
preinstall: install-exec-local

install-exec-local: genoffsets
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/bin
	$(INSTALL_PROGRAM) genoffsets ${PROJECT_ROOT}/${RTEMS_BSP}/bin

include $(top_srcdir)/../../../../../../automake/local.am
