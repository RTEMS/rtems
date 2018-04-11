ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# UNUSED

# startup
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../shared/startup/pgtbl_setup.c
librtemsbsp_a_SOURCES += startup/pgtbl_activate.c
librtemsbsp_a_SOURCES += ../shared/startup/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../shared/startup/zerobss.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../shared/startup/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += startup/bspclean.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += ../shared/startup/probeMemEnd.c

# pclock
librtemsbsp_a_SOURCES += ../shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../shared/console/uart.c
librtemsbsp_a_SOURCES += ../shared/console/console.c

# pci
librtemsbsp_a_SOURCES += pci/pci.c
librtemsbsp_a_SOURCES += pci/pci_interface.c
librtemsbsp_a_SOURCES += pci/detect_host_bridge.c
librtemsbsp_a_SOURCES += ../../shared/pci/pci_find_device.c

# irq
librtemsbsp_a_SOURCES += irq/irq_init.c
librtemsbsp_a_SOURCES += irq/BSP_irq.c

# tod
librtemsbsp_a_SOURCES += ../../shared/tod.c
librtemsbsp_a_SOURCES += tod/todcfg.c

# vectors
librtemsbsp_a_SOURCES += vectors/exceptionhandler.c
librtemsbsp_a_SOURCES += ../shared/start/vectors_entry.S

# GT64260
librtemsbsp_a_SOURCES += GT64260/GT64260TWSI.c
librtemsbsp_a_SOURCES += GT64260/MVME5500I2C.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/if_100MHz/GT64260eth.c
librtemsbsp_a_SOURCES += network/if_1GHz/if_wm.c
librtemsbsp_a_SOURCES += network/if_1GHz/pci_map.c
endif

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

noinst_PROGRAMS += mvme5500start.$(OBJEXT)
mvme5500start___OBJEXT__SOURCES = ../shared/start/preload.S start/start.S
mvme5500start___OBJEXT__CPPFLAGS = $(AM_CPPFLAGS)
mvme5500start___OBJEXT__LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += mvme5500start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/altivec/vec_sup_asm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/clock-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/bat.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/mmuAsm.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/pte121.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/ppc-irq-legacy.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mvme5500/headers.am
