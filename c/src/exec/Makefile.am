##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_POSIX
POSIX_SUBDIRS = posix
endif

SUBDIRS = score rtems $(POSIX_SUBDIRS) sapi wrapup

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
