##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

if rpc_not_supported
SUBDIRS =
else
SUBDIRS = include src
endif

EXTRA_DIST = README_RTEMS

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
