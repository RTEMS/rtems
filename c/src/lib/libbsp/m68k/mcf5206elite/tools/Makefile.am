ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = runtest

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp):
	@$(MKDIR_P) $(PROJECT_ROOT)/@RTEMS_BSP@/tests
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest: runtest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/host.am
