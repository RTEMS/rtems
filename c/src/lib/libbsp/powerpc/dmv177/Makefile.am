##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = include clock console startup start timer scv64 tod sonic \
    @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = QUIRKS README.net STATUS bsp_specs cable.doc times

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
