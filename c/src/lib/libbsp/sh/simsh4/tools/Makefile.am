##
## $Id$
##

ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

noinst_SCRIPTS = runtest

TMPINSTALL_FILES += $(PROJECT_ROOT)/simsh4/tests \
    $(PROJECT_ROOT)/simsh4/tests/runtest

$(PROJECT_ROOT)/simsh4/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/simsh4/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/host.am
