##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS=runtest

#HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/erc32/tests
	$(INSTALL_SCRIPT) runtest $(PROJECT_ROOT)/erc32/tests/runtest

include $(top_srcdir)/../../../../../../../automake/local.am
