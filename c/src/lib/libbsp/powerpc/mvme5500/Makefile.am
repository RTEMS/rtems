ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/mvme5500/start/bsp_specs


noinst_PROGRAMS =

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# UNUSED

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/pgtbl_setup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/start/pgtbl_activate.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/zerobss.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/probeMemEnd.c

# pclock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/console.c

# pci
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/pci/pci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/pci/pci_interface.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/pci/detect_host_bridge.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/pci/pci_find_device.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/irq/BSP_irq.c

# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/rtc/todcfg.c

# vectors
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/start/exceptionhandler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/vectors_entry.S

# GT64260
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/GT64260/GT64260TWSI.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/GT64260/MVME5500I2C.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/net/if_100MHz/GT64260eth.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/net/if_1GHz/if_wm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mvme5500/net/if_1GHz/pci_map.c
endif

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

noinst_PROGRAMS += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = ../../../../../../bsps/powerpc/shared/start/preload.S ../../../../../../bsps/powerpc/mvme5500/start/start.S
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.share

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/clock/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mvme5500/headers.am
