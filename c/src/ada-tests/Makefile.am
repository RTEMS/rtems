## $Id$

ACLOCAL_AMFLAGS = -I aclocal -I ../../../testsuites/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../testsuites/automake/compile.am

SUBDIRS = support samples sptests tmtests

if HAS_MP
SUBDIRS += mptests
endif

EXTRA_DIST = ada.am

include $(top_srcdir)/../../../testsuites/automake/subdirs.am
include $(top_srcdir)/../../../testsuites//automake/local.am
