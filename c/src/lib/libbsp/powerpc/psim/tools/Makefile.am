##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = psim runtest

## HACK: install to build-tree
all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_ROOT)/@RTEMS_BSP@/tests
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim: psim $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest: runtest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest

include $(top_srcdir)/../../../../../automake/host.am
