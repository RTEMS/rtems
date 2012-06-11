##
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
## Currently, the shared directory is not explicitly
## added but it is present in the source tree.
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST =

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
