##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_PROGRAMS = sload

sload_SOURCES = sload.c

## HACK: install to build-tree
install-exec-local: $(PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/mvme162/bin
	$(INSTALL_PROGRAM) $(PROGRAMS) $(PROJECT_ROOT)/mvme162/bin

include $(top_srcdir)/../../../../../../../automake/host.am
