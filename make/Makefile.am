##
## $Id$
##


rtems_makedir = $(prefix)/make

rtems_make_DATA = main.cfg leaf.cfg

EXTRA_DIST = host.cfg.in leaf.cfg

rtems_make_Templatesdir = $(rtems_makedir)/Templates

rtems_make_Templates_DATA = \
   Templates/Makefile.dir \
   Templates/Makefile.leaf \
   Templates/Makefile.lib

EXTRA_DIST += \
   compilers/gcc-no_bsp.cfg \
   compilers/gcc-portsw.cfg \
   compilers/gcc-target-default.cfg

SUBDIRS = custom

## -------------------------------------------------------
include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
