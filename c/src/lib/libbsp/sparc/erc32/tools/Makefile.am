##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

noinst_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/erc32/tests \
    $(PROJECT_ROOT)/erc32/tests/runtest

$(PROJECT_ROOT)/erc32/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/erc32/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
