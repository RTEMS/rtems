##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../aclocal

SHARED_LIB = shared

if mongoosev
CPU_SUBDIR = mongoosev
endif

if tx39
CPU_SUBDIR = tx39
endif

if r46xx
CPU_SUBDIR = clock timer r46xx
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR)


include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
