##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/erc32/tests \
    $(PROJECT_ROOT)/erc32/tests/runtest

$(PROJECT_ROOT)/erc32/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/erc32/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
