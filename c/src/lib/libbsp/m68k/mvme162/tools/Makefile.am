##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

sload_SOURCES = sload.c

if SLOAD_BUILDABLE
noinst_PROGRAMS = sload

## HACK: install to build-tree
install-exec-local: $(PROGRAMS)
	$(mkinstalldirs) $(PROJECT_ROOT)/mvme162/bin
	$(INSTALL_PROGRAM) $(PROGRAMS) $(PROJECT_ROOT)/mvme162/bin
endif
include $(top_srcdir)/../../../../../../../automake/host.am
