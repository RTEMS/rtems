##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

rtems_makedir = $(prefix)/make

rtems_make_DATA = main.cfg

EXTRA_DIST = README directory.cfg host.cfg.in leaf.cfg lib.cfg

SUBDIRS = Templates custom compilers

## -------------------------------------------------------
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
