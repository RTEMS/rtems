##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = include start clock console startup shmsupp timer vectors \
    @exceptions@ wrapup \
    tools

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = bsp_specs

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
