##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5
ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am

if HAS_POSIX
POSIX_SUBDIRS = posix
endif

if HAS_ITRON
ITRON_SUBDIRS = itron
endif

SUBDIRS = include score rtems $(POSIX_SUBDIRS) $(ITRON_SUBDIRS) sapi wrapup

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
