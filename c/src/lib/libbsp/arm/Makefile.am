## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

## Descend into the $(RTEMS_BSP_FAMILY) directory
## Currently, the shared directory is not explicitly
## added but it is present in the source tree.
SUBDIRS = shared $(RTEMS_BSP_FAMILY)

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
