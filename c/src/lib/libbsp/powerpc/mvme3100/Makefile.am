ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


project_lib_DATA =

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

preload.$(OBJEXT): ../shared/start/preload.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<

vectors_entry.$(OBJEXT): ../shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<

motld_start.$(OBJEXT):	preload.$(OBJEXT) vectors_entry.$(OBJEXT) \
    start.$(OBJEXT)
	$(LD) -o $@ -r $^

project_lib_DATA += motld_start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/misc.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../shared/startup/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../shared/startup/zerobss.c
librtemsbsp_a_SOURCES += ../shared/startup/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += tod/todcfg.c
# pclock
librtemsbsp_a_SOURCES += ../shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../shared/console/uart.c
librtemsbsp_a_SOURCES += ../shared/console/console.c

# irq
librtemsbsp_a_SOURCES += irq/irq_init.c
librtemsbsp_a_SOURCES += ../shared/irq/openpic_i8259_irq.c

# openpic
librtemsbsp_a_SOURCES += ../shared/openpic/openpic.c

# pci
librtemsbsp_a_SOURCES += ../shared/pci/pci.c
librtemsbsp_a_SOURCES += pci/detect_host_bridge.c
librtemsbsp_a_SOURCES += ../shared/pci/generic_clear_hberrs.c
librtemsbsp_a_SOURCES += ../shared/pci/pcifinddevice.c

# flash
librtemsbsp_a_SOURCES += ../shared/flash/flash.c
librtemsbsp_a_SOURCES += ../shared/flash/spansionFlash.c
librtemsbsp_a_SOURCES += flash/flashcfg.c

# i2c
librtemsbsp_a_SOURCES += i2c/mpc8540_i2c.c
librtemsbsp_a_SOURCES += i2c/i2c_init.c

# vpd
librtemsbsp_a_SOURCES += ../shared/motorola/vpd.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/tsec.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/e500-mmu.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mvme3100/headers.am
