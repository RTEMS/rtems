#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if PERL
perl_scripts = shmdump semdump
endif

noinst_SCRIPTS=runtest looptest $(perl_scripts)

# HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/posix/tests
	for i in $(SCRIPTS); do \
	$(INSTALL_SCRIPT) $$i $(PROJECT_ROOT)/posix/tests ; \
	done

include $(top_srcdir)/../../../../../../../automake/local.am
