ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = shared/include/linker-symbols.h

dist_project_lib_DATA = shared/startup/linkcmds.base

EXTRA_DIST =
EXTRA_DIST += ../shared/setvec.c
EXTRA_DIST += shared/start.S

EXTRA_DIST += shared/bspspuriousinit.c shared/m68000spurious.c

EXTRA_DIST += shared/mvme/mvme16x_hw.h

EXTRA_DIST += shared/gdbstub/m68k-stub.c shared/gdbstub/gdb_if.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
