##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

rtems_makedir=$(prefix)/make

GENERIC_FILES = \
host.cfg

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

endif

SUBDIRS = compilers custom

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

@RTEMS_BSP@.cache: Makefile
	$(RM) $@
	@echo rtems_cv_RTEMS_CPU_MODEL=\$${rtems_cv_RTEMS_CPU_MODEL=$(RTEMS_CPU_MODEL)} >> $@
	@echo rtems_cv_RTEMS_BSP_FAMILY=\$${rtems_cv_RTEMS_BSP_FAMILY=$(RTEMS_BSP_FAMILY)} >> $@

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
