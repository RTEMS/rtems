##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/macros

if shared
SHARED_LIB = shared
endif

if m68040
CPU_SUBDIR = m68040
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR)

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
