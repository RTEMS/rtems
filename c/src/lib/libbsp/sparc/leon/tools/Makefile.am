##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

noinst_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/leon/tests \
    $(PROJECT_ROOT)/leon/tests/runtest

$(PROJECT_ROOT)/leon/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/leon/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
