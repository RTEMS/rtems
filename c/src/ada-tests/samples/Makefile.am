## $Id$

SUBDIRS = hello ticker base_sp

if HAS_MP
SUBDIRS += base_mp
endif
DIST_SUBDIRS = hello ticker base_sp base_mp

include $(top_srcdir)/../../../testsuites/automake/subdirs.am
include $(top_srcdir)/../../../testsuites/automake/local.am
