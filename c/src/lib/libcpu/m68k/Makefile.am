##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

if shared
SHARED_LIB = shared
endif

if m68040
CPU_SUBDIR = m68040
endif

if mcf5206
CPU_SUBDIR = mcf5206
endif

SUBDIRS = $(SHARED_LIB) $(CPU_SUBDIR)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
