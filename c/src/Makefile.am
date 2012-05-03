ACLOCAL_AMFLAGS = -I aclocal

SUBDIRS = . @BSP_SUBDIRS@
DIST_SUBDIRS = @BSP_SUBDIRS@

clean-local:
	$(RM) -r $(PROJECT_RELEASE)

make/@RTEMS_BSP@.cache: make/bsp.mak @CUSTOM_CFG_FILES@
	$(RM) make/@RTEMS_BSP@.cache
	$(MAKE) -f ${srcdir}/make/bsp.mak \
	'RTEMS_BSP=@RTEMS_BSP@' 'RTEMS_ROOT=.' \
	make/@RTEMS_BSP@.cache

noinst_DATA = make/@RTEMS_BSP@.cache
DISTCLEANFILES = make/@RTEMS_BSP@.cache

rtems_makedir = $(prefix)/make
rtems_make_customdir = $(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@
DISTCLEANFILES += @CUSTOM_CFG_FILES@

noinst_DATA += make/custom/default.cfg
DISTCLEANFILES += make/custom/default.cfg

noinst_DATA += preinstall-stamp

preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
CLEANFILES = preinstall-stamp

@host_alias@-@RTEMS_BSP@.pc: config.status bsp.pc.in
	./config.status --file=$@:$(srcdir)/bsp.pc.in

pkgconfigdir = $(prefix)/lib/pkgconfig
pkgconfig_DATA = @host_alias@-@RTEMS_BSP@.pc
DISTCLEANFILES += @host_alias@-@RTEMS_BSP@.pc

DISTCLEANFILES += $(DEPDIR)/cfg.P
include $(DEPDIR)/cfg.P

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
