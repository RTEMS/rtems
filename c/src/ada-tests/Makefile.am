## $Id$

ACLOCAL_AMFLAGS = -I aclocal -I ../tests/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../tests/automake/compile.am

SUBDIRS = support samples sptests tmtests

if HAS_MP
SUBDIRS += mptests
endif

EXTRA_DIST = ada.am

include $(top_srcdir)/../tests/automake/subdirs.am
include $(top_srcdir)/../tests/automake/local.am
