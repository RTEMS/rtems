ACLOCAL_AMFLAGS = -I ../../../../../aclocal

EXTRA_DIST = Spec.doc

# HACK: autoconf wants to transform the name, but RTEMS *.cfgs are not able
# to deal with it.
# FIXME: The installation directory is arguable
transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/build-tools
bsptools_bin_PROGRAMS = bin2boot

bin2boot_SOURCES = bin2boot.c

## HACK: install into build-tree
all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp):
	@$(MKDIR_P) $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/bin2boot$(EXEEXT): bin2boot$(EXEEXT) $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/bin2boot$(EXEEXT)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/bin2boot$(EXEEXT)

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../automake/host.am
