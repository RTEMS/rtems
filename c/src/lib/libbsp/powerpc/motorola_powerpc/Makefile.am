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


start.$(OBJEXT): ../../powerpc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

vectors_entry.$(OBJEXT): ../../powerpc/shared/start/vectors_entry.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += vectors_entry.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.share

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../powerpc/shared/startup/bspstart.c \
 	../../powerpc/shared/startup/bspgetworkarea.c \
 	../../powerpc/shared/showbats.c \
    ../../powerpc/shared/startup/bsppredriverhook.c \
 	../../powerpc/shared/startup/zerobss.c \
    ../../powerpc/shared/startup/pgtbl_setup.c \
    ../../powerpc/shared/startup/pgtbl_activate.c \
	../../powerpc/shared/startup/ppc_idle.c \
    ../../powerpc/shared/startup/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bspclean.c \
    ../../shared/gnatinstallhandler.c \
    startup/bspreset.c


# tod
librtemsbsp_a_SOURCES += ../../shared/tod.c ../../powerpc/shared/tod/todcfg.c
# pclock
librtemsbsp_a_SOURCES += ../../powerpc/shared/clock/p_clock.c

# Needs conditional compilation
polledIO_CPPFLAGS = -DSTATIC_LOG_ALLOC
noinst_PROGRAMS += polledIO.rel
polledIO_rel_SOURCES = ../../powerpc/shared/console/polled_io.c
polledIO_rel_CPPFLAGS = $(AM_CPPFLAGS) $(polledIO_CPPFLAGS)
polledIO_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# console
librtemsbsp_a_SOURCES += ../../powerpc/shared/console/uart.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/console/console.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/console/consoleIo.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/console/keyboard.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/console/uart.h

# irq
librtemsbsp_a_SOURCES += ../../powerpc/shared/irq/irq_init.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/irq/openpic_i8259_irq.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/irq/i8259.c

# motorola
librtemsbsp_a_SOURCES += ../../powerpc/shared/motorola/motorola.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/motorola/motorola.c

# openpic
librtemsbsp_a_SOURCES += ../../powerpc/shared/openpic/openpic.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/openpic/openpic.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/openpic/openpic.h

# pci
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/pci.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/detect_raven_bridge.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/generic_clear_hberrs.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/pcifinddevice.c
librtemsbsp_a_SOURCES += ../../powerpc/shared/pci/pci.h

# residual
librtemsbsp_a_SOURCES += ../../powerpc/shared/residual/pnp.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/residual/residual.h
librtemsbsp_a_SOURCES += ../../powerpc/shared/residual/residual.c

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

librtemsbsp_a_LIBADD = polledIO.rel


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(top_srcdir)/../../../../automake/subdirs.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/vme-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/motorola_powerpc/headers.am
