##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . start startup clock console timer wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
