##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg

rtems_makedir = $(prefix)/make

GENERIC_FILES = host.cfg

MAKE_FILES = README directory.cfg lib.cfg

rtems_make_DATA = $(MAKE_FILES) $(GENERIC_FILES)

noinst_DATA = leaf.cfg
EXTRA_DIST = leaf.cfg

rtemsdir = $(exec_prefix)
rtems_bspdir = $(rtemsdir)/@RTEMS_BSP@
rtems_bsp_DATA = Makefile.inc

rtems_bsp_makedir = $(rtems_bspdir)/make
rtems_bsp_make_DATA = bsp.cfg

if MAINTAINER_MODE
$(srcdir)/host.cfg.in: $(top_srcdir)/@RTEMS_TOPdir@/make/host.cfg.in
	cp $< $@
endif

@RTEMS_BSP@.cache: Makefile
	@$(RM) $@
	@echo rtems_cv_RTEMS_CPU_MODEL=\$${rtems_cv_RTEMS_CPU_MODEL=$(RTEMS_CPU_MODEL)} >> $@
	@echo rtems_cv_RTEMS_BSP_FAMILY=\$${rtems_cv_RTEMS_BSP_FAMILY=$(RTEMS_BSP_FAMILY)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS=\$${rtems_cv_RTEMS_BSP_CFLAGS=$(CPU_CFLAGS)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_OPTIMIZE_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_OPTIMIZE_V=$(CFLAGS_OPTIMIZE_V)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_DEBUG_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_DEBUG_V=$(CFLAGS_DEBUG_V)} >> $@
	@echo rtems_cv_RTEMS_BSP_CFLAGS_PROFILE_V=\$${rtems_cv_RTEMS_BSP_CFLAGS_PROFILE_V=$(CFLAGS_PROFILE_V)} >> $@
noinst_DATA += @RTEMS_BSP@.cache
DISTCLEANFILES = @RTEMS_BSP@.cache

rtems_make_compilersdir = $(rtems_makedir)/compilers

## At the moment all actively supported configurations
## use gcc-target-default.cfg only.
dist_rtems_make_compilers_DATA = compilers/gcc-target-default.cfg

rtems_make_customdir = $(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@

noinst_DATA += custom/default.cfg
DISTCLEANFILES += custom/default.cfg

include $(top_srcdir)/../../../automake/local.am
