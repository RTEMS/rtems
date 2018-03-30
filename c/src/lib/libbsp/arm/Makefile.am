ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
## Currently, the shared directory is not explicitly
## added but it is present in the source tree.
_SUBDIRS = @RTEMS_BSP_FAMILY@

dist_project_lib_DATA = shared/startup/linkcmds.base
dist_project_lib_DATA += shared/startup/linkcmds.armv4
dist_project_lib_DATA += shared/startup/linkcmds.armv7m


# comm

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
include $(srcdir)/../../../../../bsps/arm/headers.am
