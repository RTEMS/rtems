ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/mvme3100/start/bsp_specs


noinst_PROGRAMS =


project_lib_DATA =

start.$(OBJEXT): ../../../../../../bsps/powerpc/mvme3100/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

preload.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

vectors_entry.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) \
    start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/start/misc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/zerobss.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/rtc/todcfg.c
# pclock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/console.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/openpic_i8259_irq.c

# openpic
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/openpic.c

# pci
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/pci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/pci/detect_host_bridge.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/generic_clear_hberrs.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/pcifinddevice.c

# flash
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/flash/flash.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/flash/spansionFlash.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/flash/flashcfg.c

# i2c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/i2c/mpc8540_i2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/i2c/i2c_init.c

# vpd
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/vpd.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme3100/net/tsec.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/clock/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/e500-mmu.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mvme3100/headers.am
