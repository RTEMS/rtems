#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

bin_PROGRAMS=cklength eolstrip packhex unhex

cklength_SOURCES = cklength.c
eolstrip_SOURCES = eolstrip.c
packhex_SOURCES  = packhex.c
unhex_SOURCES    = unhex.c

bin_SCRIPTS=\
install-if-change \
lock-directory \
unlock-directory \
rcs-clean 

noinst_SCRIPTS = search-id.sh

#
# HACK: Install to build directory
#
install-exec-local: $(bin_PROGRAMS) $(bin_SCRIPTS)
	if test -n "${RTEMS_BSP}"; then \
	  $(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools; \
	  $(INSTALL_SCRIPT) $(bin_SCRIPTS) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools; \
	  $(INSTALL_PROGRAM) $(bin_PROGRAMS) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools; \
	fi

preinstall: install-exec-local

include $(top_srcdir)/../../automake/local.am
