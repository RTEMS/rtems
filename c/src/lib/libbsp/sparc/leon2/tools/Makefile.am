##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

transform =
bsptools_bindir = $(PROJECT_ROOT)/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = runtest

TMPINSTALL_FILES = $(bsptools_bindir) $(bsptools_bindir)/runtest

$(bsptools_bindir):
	$(mkinstalldirs) $@

$(bsptools_bindir)/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../../automake/host.am
