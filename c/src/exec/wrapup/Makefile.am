##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_POSIX
POSIX_DIRS = posix
endif

if HAS_ITRON
ITRON_DIRS = itron
endif

SUBDIRS = rtems $(POSIX_DIRS) $(ITRON_DIRS)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
