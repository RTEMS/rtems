##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS =

if HAS_POSIX
SUBDIRS += psxtmkey01
SUBDIRS += psxtmkey02
SUBDIRS += psxtmmutex01
SUBDIRS += psxtmmutex02
SUBDIRS += psxtmmutex03
SUBDIRS += psxtmmutex04
SUBDIRS += psxtmmutex05
SUBDIRS += psxtmmutex06
SUBDIRS += psxtmmutex07
SUBDIRS += psxtmnanosleep01
SUBDIRS += psxtmnanosleep02
SUBDIRS += psxtmsem01
SUBDIRS += psxtmsem02
SUBDIRS += psxtmsleep01
SUBDIRS += psxtmsleep02
SUBDIRS += psxtmthread01
SUBDIRS += psxtmthread03
endif

DIST_SUBDIRS = $(SUBDIRS)

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
