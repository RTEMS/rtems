##
## $Id$
##


rtems_makedir = $(prefix)/make

rtems_make_DATA = main.cfg leaf.cfg

EXTRA_DIST = README directory.cfg host.cfg.in leaf.cfg lib.cfg

rtems_make_Templatesdir = $(rtems_makedir)/Templates

rtems_make_Templates_DATA = \
   Templates/Makefile.dir \
   Templates/Makefile.leaf \
   Templates/Makefile.lib

SUBDIRS = custom compilers

## -------------------------------------------------------
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
