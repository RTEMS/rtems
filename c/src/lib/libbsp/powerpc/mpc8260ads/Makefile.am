##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = include clock console vectors irq network start startup \
    @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = README bsp_specs times

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
