##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = runtest looptest

if PERL
bsptools_bin_SCRIPTS += shmdump semdump
endif

## HACK: install to build-tree
all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_ROOT)/@RTEMS_BSP@/tests
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest: runtest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/runtest

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/looptest: looptest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/looptest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/looptest

if PERL
$(PROJECT_ROOT)/@RTEMS_BSP@/tests/shmdump: shmdump $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/shmdump
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/shmdump

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/semdump: semdump $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/semdump
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/semdump
endif

include $(top_srcdir)/../../../../../automake/host.am
