##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

if PERL
perl_scripts = shmdump semdump
PERL_TMPINSTALL_FILES = $(PROJECT_ROOT)/posix/tests/shmdump \
    $(PROJECT_ROOT)/posix/tests/semdump
endif

noinst_SCRIPTS = runtest looptest $(perl_scripts)

$(PROJECT_ROOT)/posix/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/posix/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

$(PROJECT_ROOT)/posix/tests/looptest: looptest
	$(INSTALL_SCRIPT) $< $@

$(PROJECT_ROOT)/posix/tests/shmdump: shmdump
	$(INSTALL_SCRIPT) $< $@

$(PROJECT_ROOT)/posix/tests/semdump: semdump
	$(INSTALL_SCRIPT) $< $@

TMPINSTALL_FILES = $(PROJECT_ROOT)/posix/tests \
    $(PROJECT_ROOT)/posix/tests/runtest $(PROJECT_ROOT)/posix/tests/looptest

## HACK: install to build-tree
all-local: $(TMPINSTALL_FILES) $(PERL_TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/host.am
