##
## Makefile.am,v 1.8.4.1 2003/02/20 21:55:34 joel Exp
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = include clock console pci irq tod vectors start \
    startup GT64260 network @exceptions@ vme wrapup

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = README bsp_specs ChangeLog.1

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
