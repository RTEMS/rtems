ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
_SUBDIRS = @RTEMS_BSP_FAMILY@

dist_project_lib_DATA = shared/startup/linkcmds.base

EXTRA_DIST =
EXTRA_DIST += ../shared/setvec.c
EXTRA_DIST += shared/start.S

EXTRA_DIST += shared/bspspuriousinit.c shared/m68000spurious.c

EXTRA_DIST += shared/gdbstub/m68k-stub.c shared/gdbstub/gdb_if.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
include $(srcdir)/../../../../../bsps/m68k/headers.am
