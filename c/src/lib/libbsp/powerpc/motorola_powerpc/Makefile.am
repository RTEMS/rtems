ACLOCAL_AMFLAGS = -I ../../../../aclocal

_SUBDIRS  = .
_SUBDIRS += bootloader
if QEMU
_SUBDIRS += qemu_fakerom
endif

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

vectors_entry.$(OBJEXT): ../shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += vectors_entry.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/start/bspstart.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../shared/showbats.c
librtemsbsp_a_SOURCES += ../shared/startup/zerobss.c
librtemsbsp_a_SOURCES += ../shared/startup/pgtbl_setup.c
librtemsbsp_a_SOURCES += ../shared/startup/pgtbl_activate.c
librtemsbsp_a_SOURCES += ../shared/startup/ppc_idle.c
librtemsbsp_a_SOURCES += ../shared/startup/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += startup/bspreset.c


# tod
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../shared/tod/todcfg.c
# pclock
librtemsbsp_a_SOURCES += ../shared/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/motorola_powerpc/dev/polled_io.c
librtemsbsp_a_SOURCES += ../shared/console/uart.c
librtemsbsp_a_SOURCES += ../shared/console/console.c

# irq
librtemsbsp_a_SOURCES += ../shared/irq/irq_init.c
librtemsbsp_a_SOURCES += ../shared/irq/openpic_i8259_irq.c
librtemsbsp_a_SOURCES += ../shared/irq/i8259.c

# motorola
librtemsbsp_a_SOURCES += ../shared/motorola/motorola.c

# openpic
librtemsbsp_a_SOURCES += ../shared/openpic/openpic.c

# pci
librtemsbsp_a_SOURCES += ../shared/pci/pci.c
librtemsbsp_a_SOURCES += ../shared/pci/detect_raven_bridge.c
librtemsbsp_a_SOURCES += ../shared/pci/generic_clear_hberrs.c
librtemsbsp_a_SOURCES += ../shared/pci/pcifinddevice.c

# residual
librtemsbsp_a_SOURCES += ../shared/residual/residual.c

if QEMU
if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../i386/pc386/ne2000/ne2000.c
endif
endif

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
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/motorola_powerpc/headers.am
