#
# $Id$
#

AUTOMAKE_OPTIONS = foreign

bin_PROGRAMS=cklength eolstrip packhex unhex

cklength_SOURCES = cklength.c
eolstrip_SOURCES = eolstrip.c
packhex_SOURCES  = packhex.c
unhex_SOURCES    = unhex.c

#
# HACK: install to the build-tree
#
install-exec-local: $(bin_PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools
	$(INSTALL_PROGRAM) $(bin_PROGRAMS) \
	  $(PROJECT_ROOT)/${RTEMS_BSP}/build-tools

preinstall: install-exec-local
