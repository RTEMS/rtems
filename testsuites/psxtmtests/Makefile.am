##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

if HAS_POSIX
SUBDIRS = \
    psxtmmutex02 \
    psxtmsleep01 \
    psxtmthread01 psxtmthread03
endif

DIST_SUBDIRS = $(SUBDIRS)
# EXTRA_DIST = psxfatal_support/init.c psxfatal_support/system.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
