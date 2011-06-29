##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

if HAS_POSIX
SUBDIRS = psxtmmutex02
SUBDIRS += psxtmsleep01
SUBDIRS += psxtmsleep02
SUBDIRS += psxtmthread01
SUBDIRS += psxtmthread03
SUBDIRS += psxtmnanosleep01
SUBDIRS += psxtmnanosleep02
endif

DIST_SUBDIRS = $(SUBDIRS)
# EXTRA_DIST = psxfatal_support/init.c psxfatal_support/system.h

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
