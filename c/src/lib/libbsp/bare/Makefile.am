## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(top_srcdir)/../bsp.am

SUBDIRS = . include

EXTRA_DIST = bsp_specs build-tools do-bare-bsp-build

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
