##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

rtems_makedir = $(prefix)/make

GENERIC_FILES = host.cfg

MAKE_FILES = README directory.cfg lib.cfg

rtems_make_DATA = $(MAKE_FILES) $(GENERIC_FILES)

noinst_DATA = leaf.cfg

rtemsdir = $(exec_prefix)
rtems_bspdir = $(rtemsdir)/@RTEMS_BSP@
rtems_bsp_DATA = Makefile.inc

rtems_bsp_makedir = $(rtems_bspdir)/make
rtems_bsp_make_DATA = bsp.cfg

if MAINTAINER_MODE
$(srcdir)/host.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/host.cfg.in
	cp $< $@

endif

SUBDIRS = compilers custom

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

@RTEMS_BSP@.cache: Makefile
	$(RM) $@
	@echo rtems_cv_RTEMS_CPU_MODEL=\$${rtems_cv_RTEMS_CPU_MODEL=$(RTEMS_CPU_MODEL)} >> $@
	@echo rtems_cv_RTEMS_BSP_FAMILY=\$${rtems_cv_RTEMS_BSP_FAMILY=$(RTEMS_BSP_FAMILY)} >> $@

DISTCLEANFILES = @RTEMS_BSP@.cache

EXTRA_DIST = leaf.cfg

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
