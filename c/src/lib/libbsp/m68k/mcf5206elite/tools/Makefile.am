##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/mcf5206elite/tests \
    $(PROJECT_ROOT)/mcf5206elite/tests/runtest

$(PROJECT_ROOT)/mcf5206elite/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/mcf5206elite/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
