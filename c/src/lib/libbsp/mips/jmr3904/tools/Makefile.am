##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

noinst_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/jmr3904/tests \
    $(PROJECT_ROOT)/jmr3904/tests/runtest 

$(PROJECT_ROOT)/jmr3904/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/jmr3904/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

## HACK: install to build-tree
all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
