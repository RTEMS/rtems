##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = runtest

TMPINSTALL_FILES = $(PROJECT_ROOT)/@RTEMS_BSP@/tests \
    $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest

$(PROJECT_ROOT)/@RTEMS_BSP@/tests:
	$(mkinstalldirs) $@

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest: runtest
	$(INSTALL_SCRIPT) $< $@

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/host.am
