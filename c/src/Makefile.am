ACLOCAL_AMFLAGS = -I aclocal

_SUBDIRS = . @BSP_SUBDIRS@
DIST_SUBDIRS = @BSP_SUBDIRS@

$(PROJECT_LIB)/$(dirstamp):
	@echo "Making project library directory: $(PROJECT_LIB)"
	@$(MKDIR_P) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)

all-local: $(PROJECT_LIB)/$(dirstamp)

clean-local:
	$(RM) -r $(PROJECT_RELEASE)

make/@RTEMS_BSP@.cache: make/bsp.mak @CUSTOM_CFG_FILES@
	$(RM) make/@RTEMS_BSP@.cache
	$(MAKE) -f ${srcdir}/make/bsp.mak \
	'RTEMS_BSP=@RTEMS_BSP@' 'RTEMS_ROOT=.' \
	make/@RTEMS_BSP@.cache

noinst_DATA = make/@RTEMS_BSP@.cache

rtems_makedir = $(prefix)/make
rtems_make_customdir = $(rtems_makedir)/custom
rtems_make_custom_DATA = @CUSTOM_CFG_FILES@

noinst_DATA += make/custom/default.cfg

@host_alias@-@RTEMS_BSP@.pc: config.status bsp.pc.in
	./config.status --file=$@:$(srcdir)/bsp.pc.in

pkgconfigdir = $(prefix)/lib/pkgconfig
pkgconfig_DATA = @host_alias@-@RTEMS_BSP@.pc

include $(DEPDIR)/cfg.P

include $(top_srcdir)/automake/subdirs.am
include $(srcdir)/../../bsps/headers.am
