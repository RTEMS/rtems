#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

rtems_makedir=$(prefix)/make

GENERIC_FILES = \
host.cfg target.cfg

MAKE_FILES = README \
directory.cfg leaf.cfg \
lib.cfg

rtems_make_DATA = \
$(MAKE_FILES) \
$(GENERIC_FILES)

noinst_DATA = \
main.cfg

rtemsdir = $(prefix)
rtems_bspdir = $(rtemsdir)/@RTEMS_BSP@
rtems_bsp_DATA = \
Makefile.inc

rtems_bsp_makedir = $(rtems_bspdir)/make
rtems_bsp_make_DATA = \
bsp.cfg

if MAINTAINER_MODE
$(srcdir)/main.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/main.cfg
	sed -e 's%\$$(RTEMS_BSP)%\@RTEMS_BSP\@%g' < $< >$@

$(srcdir)/host.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/host.cfg.in
	cp $< $@

$(srcdir)/target.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/target.cfg.in
	cp $< $@
endif

SUBDIRS = compilers custom Templates

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
