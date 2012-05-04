ACLOCAL_AMFLAGS = -I aclocal -I ../../../testsuites/aclocal

SUBDIRS = support samples sptests tmtests
if HAS_MP
SUBDIRS += mptests
endif

DIST_SUBDIRS = support samples sptests tmtests mptests

EXTRA_DIST = ada.am

include $(top_srcdir)/../../../testsuites/automake/subdirs.am
include $(top_srcdir)/../../../testsuites//automake/local.am
