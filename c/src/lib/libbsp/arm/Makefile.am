ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
## Currently, the shared directory is not explicitly
## added but it is present in the source tree.
SUBDIRS = @RTEMS_BSP_FAMILY@

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = shared/include/linker-symbols.h

dist_project_lib_DATA = shared/startup/linkcmds.base
dist_project_lib_DATA += shared/startup/linkcmds.armv4
dist_project_lib_DATA += shared/startup/linkcmds.armv7m

EXTRA_DIST =

# comm
EXTRA_DIST += shared/comm/uart.h
EXTRA_DIST += shared/comm/uart.c

# abort
EXTRA_DIST += shared/abort/simple_abort.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
