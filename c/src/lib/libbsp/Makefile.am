##
## $Id$
##

# If we are building a "real" BSP, then we need to descend into the
# appropriate CPU specific directory.  The bare BSP is a special
# case which can be built for any CPU and it resides at the same
# level as the CPUs.  If we are building the bare BSP, then descend
# into that directory.

SUBDIRS = shared shmdr @libbsp_cpu_subdir@
DIST_SUBDIRS = shared shmdr @libbsp_cpu_subdir@

EXTRA_DIST = MERGE.PROCEDURE bsp.am

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/local.am
