##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../aclocal

SHARED_LIB = shared

if tx39
CPU_SUBDIR = tx39
endif

if r46xx
CPU_SUBDIR = clock timer
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR)


include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
