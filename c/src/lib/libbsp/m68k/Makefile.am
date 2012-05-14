ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST = shared/m68kpretaskinghook.c ../shared/setvec.c shared/start.S

EXTRA_DIST += shared/bspspuriousinit.c shared/m68000spurious.c

EXTRA_DIST += shared/mvme/mvme16x_hw.h

EXTRA_DIST += shared/gdbstub/m68k-stub.c shared/gdbstub/gdb_if.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
