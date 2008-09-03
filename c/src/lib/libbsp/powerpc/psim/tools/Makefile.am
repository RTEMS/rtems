##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = psim psim-gdb runtest

## HACK: install to build-tree
all-local: psim $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp):
	@$(MKDIR_P) $(PROJECT_ROOT)/@RTEMS_BSP@/tests
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)

psim: psim-top psim-shared psim-bottom
	cat $^ >$@
	chmod +x $@

psim-gdb: psim-gdb-top psim-shared psim-gdb-bottom
	cat $^ >$@
	chmod +x $@

runtest: runtest-top psim-shared runtest-bottom
	cat $^ >$@
	chmod +x $@

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim: psim $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim-gdb: psim-gdb $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim-gdb
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/psim-gdb

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest: runtest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest

CLEANFILES = $(TMPINSTALL_FILES) psim psim-gdb

include $(top_srcdir)/../../../../../automake/host.am
