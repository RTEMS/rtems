ACLOCAL_AMFLAGS = -I ../../../../../aclocal

transform =
bsptools_bindir = ${exec_prefix}/@RTEMS_BSP@/build-tools
bsptools_bin_SCRIPTS = runtest
bsptools_bin_PROGRAMS = ndstool

## HACK: install to build-tree
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

ndstool_SOURCES =	ndstool/source/arm7_sha1_homebrew.c	\
			ndstool/source/arm7_sha1_nintendo.c	\
			ndstool/source/banner.cpp		\
			ndstool/source/bigint.cpp		\
			ndstool/source/compile_date.c		\
			ndstool/source/crc.cpp			\
			ndstool/source/default_icon.c		\
			ndstool/source/encryption.cpp		\
			ndstool/source/header.cpp		\
			ndstool/source/hook.cpp			\
			ndstool/source/loadme.c			\
			ndstool/source/logo.cpp			\
			ndstool/source/ndscodes.cpp		\
			ndstool/source/ndscreate.cpp		\
			ndstool/source/ndsextract.cpp		\
			ndstool/source/ndstool.cpp		\
			ndstool/source/ndstree.cpp		\
			ndstool/source/passme.cpp		\
			ndstool/source/passme_sram.c		\
			ndstool/source/raster.cpp		\
			ndstool/source/sha1.cpp
ndstool_CPPFLAGS = -I$(srcdir)/ndstool/include -DVERSION="\"compiled for rtems\""
ndstool_CFLAGS =
ndstool_CXXFLAGS =

$(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp):
	@$(MKDIR_P) $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools
	@: > $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)

$(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/ndstool$(EXEEXT): ndstool$(EXEEXT) $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/ndstool$(EXEEXT)
TMPINSTALL_FILES += $(PROJECT_ROOT)/@RTEMS_BSP@/build-tools/ndstool$(EXEEXT)

include $(top_srcdir)/../../../../../automake/host.am
