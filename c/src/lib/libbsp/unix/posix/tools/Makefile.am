#
# $Id$
#

AUTOMAKE_OPTIONS = foreign
ACLOCAL = @ACLOCAL@ -I $(RTEMS_TOPdir)/aclocal

if HAVE_PERL
perl_scripts = shmdump semdump
endif

noinst_SCRIPTS=runtest looptest $(perl_scripts)

# HACK: install to build-tree
all-local: $(SCRIPTS)
	$(mkinstalldirs) $(PROJECT_ROOT)/${RTEMS_BSP}/tests
	for i in $(SCRIPTS); do \
	$(INSTALL_SCRIPT) $$i $(PROJECT_ROOT)/${RTEMS_BSP}/tests ; \
	done

include $(top_srcdir)/../../../../../../../automake/local.am
