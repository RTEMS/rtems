## $Id$

ACLOCAL_AMFLAGS = -I aclocal -I ../tests/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am

SUBDIRS = support samples sptests tmtests

if HAS_MP
SUBDIRS += mptests
endif

EXTRA_DIST = ada.am

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
