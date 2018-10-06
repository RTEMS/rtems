ACLOCAL_AMFLAGS = -I aclocal -I ../aclocal

_SUBDIRS = support samples sptests tmtests
if HAS_MP
_SUBDIRS += mptests
endif

DIST_SUBDIRS = support samples sptests tmtests mptests


include $(top_srcdir)/../automake/subdirs.am
