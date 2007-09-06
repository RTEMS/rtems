##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

# Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST =

EXTRA_DIST += shared/bspclean.c
EXTRA_DIST += shared/bspstart.c
EXTRA_DIST += shared/gnatcommon.c
EXTRA_DIST += shared/start.S

# AMBA Plug&Play bus
EXTRA_DIST += shared/include/ambapp.h
EXTRA_DIST += shared/amba/ambapp.c

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
