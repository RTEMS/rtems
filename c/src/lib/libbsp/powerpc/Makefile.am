##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = shared @RTEMS_BSP_FAMILY@

# FIXME: This does not work
# DIST_SUBDIRS = @all_subdirs@ support/old_exception_processing \
#    support/new_exception_processing

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
