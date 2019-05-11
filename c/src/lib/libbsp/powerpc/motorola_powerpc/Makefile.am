ACLOCAL_AMFLAGS = -I ../../../../aclocal

_SUBDIRS  = .
_SUBDIRS += bootloader
if QEMU
_SUBDIRS += qemu_fakerom
endif

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/motorola_powerpc/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

vectors_entry.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += vectors_entry.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/showbats.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/zerobss.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/pgtbl_setup.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/pgtbl_activate.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/ppc_idle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/start/bspreset.c


# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/rtc/todcfg.c
# pclock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/console/polled_io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/console/console.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/openpic_i8259_irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/i8259.c

# motorola
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/start/motorola.c

# openpic
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/irq/openpic.c

# pci
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/pci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/detect_raven_bridge.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/generic_clear_hberrs.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/pci/pcifinddevice.c

# residual
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/start/residual.c

if QEMU
if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/i386/pc386/net/ne2000.c
endif
endif

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
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/motorola_powerpc/headers.am
