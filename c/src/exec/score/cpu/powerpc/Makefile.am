##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if PPC_NEW_EXCEPTION_PROCESSING
CPUDIR = new_exception_processing
else
CPUDIR = old_exception_processing
endif

SUBDIRS = shared $(CPUDIR)

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
