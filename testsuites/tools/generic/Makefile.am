##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

transform =

bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/tests
bsptools_bin_SCRIPTS = difftest sorttimes

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_ROOT)/@RTEMS_BSP@/tests
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/difftest: difftest $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/difftest
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/difftest

$(PROJECT_ROOT)/@RTEMS_BSP@/tests/sorttimes: sorttimes $(PROJECT_ROOT)/@RTEMS_BSP@/tests/$(dirstamp)
	$(INSTALL_SCRIPT) $< $(PROJECT_ROOT)/@RTEMS_BSP@/tests/sorttimes
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/tests/sorttimes

include $(top_srcdir)/../../automake/local.am
