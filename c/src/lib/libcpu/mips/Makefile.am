##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/macros

SHARED_LIB = 

if r46xx
CPU_SUBDIR = clock timer
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR)


include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
