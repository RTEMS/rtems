##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

noinst_SCRIPTS = psim runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/psim/tests \
    $(PROJECT_ROOT)/psim/tests/runtest $(PROJECT_ROOT)/psim/tests/psim \
    $(PROJECT_ROOT)/psim/tests/psim-gdb

$(PROJECT_ROOT)/psim/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/psim/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

$(PROJECT_ROOT)/psim/tests/psim: psim
	$(INSTALL_SCRIPT) $< $@

$(PROJECT_ROOT)/psim/tests/psim-gdb: psim
	$(INSTALL_SCRIPT) $< $@

## HACK: install to build-tree
all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
