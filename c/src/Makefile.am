##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

SUBDIRS = @BSP_SUBDIRS@

clean-local:
	$(RM) -r $(PROJECT_RELEASE)

make/@RTEMS_BSP@.cache: make/bsp.mak @CUSTOM_CFG_FILES@
	$(RM) make/@RTEMS_BSP@.cache
	$(MAKE) -f ${srcdir}/make/bsp.mak \
	RTEMS_BSP=@RTEMS_BSP@ RTEMS_ROOT=. BARE_CPU_MODEL=${BARE_CPU_MODEL} \
	make/@RTEMS_BSP@.cache

noinst_DATA = make/@RTEMS_BSP@.cache
DISTCLEANFILES = make/@RTEMS_BSP@.cache

rtems_makedir = $(prefix)/make
rtems_make_customdir = $(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@

make/custom/default.cfg: $(srcdir)/make/custom/default.cfg.in
	cp $< $@

make/custom/%.cfg: $(srcdir)/$(cfg_dir)/custom/%.cfg
	cp $< $@

noinst_DATA += make/custom/default.cfg
DISTCLEANFILES += make/custom/default.cfg

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/host.am
