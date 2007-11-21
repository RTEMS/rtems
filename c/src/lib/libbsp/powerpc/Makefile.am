##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

## Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

# FIXME: This does not work
# DIST_SUBDIRS = @all_subdirs@ support/old_exception_processing \
#    support/new_exception_processing

# FIXME: Herein lies madness ;)

## shared/include
EXTRA_DIST = shared/include/nvram.h

## shared/bootloader
EXTRA_DIST += shared/bootloader/em86.c shared/bootloader/lib.c shared/bootloader/misc.c \
    shared/bootloader/mm.c shared/bootloader/pci.c shared/bootloader/zlib.c
EXTRA_DIST += shared/bootloader/bootldr.h shared/bootloader/zlib.h shared/bootloader/pci.h
EXTRA_DIST += shared/bootloader/em86real.S shared/bootloader/exception.S \
    shared/bootloader/head.S
EXTRA_DIST += shared/bootloader/ppcboot.lds

## shared/clock
EXTRA_DIST += shared/clock/p_clock.c

## shared/console
EXTRA_DIST += shared/console/console.c shared/console/inch.c shared/console/polled_io.c \
    shared/console/uart.c shared/console/reboot.c shared/console/console.inl

## shared/motorola
EXTRA_DIST += shared/motorola/motorola.c

## shared/pci
EXTRA_DIST += shared/pci/pci.c shared/pci/detect_raven_bridge.c shared/pci/pcifinddevice.c \
	shared/pci/generic_clear_hberrs.c

## shared/residual
EXTRA_DIST += shared/residual/residual.c

## shared/openpic
EXTRA_DIST += shared/openpic/openpic.c

## shared/irq
EXTRA_DIST += shared/irq/i8259.c shared/irq/irq.c shared/irq/irq_init.c \
	shared/irq/irq_asm.S shared/irq/openpic_i8259_irq.c

## shared/start
EXTRA_DIST += shared/start/start.S shared/start/rtems_crti.S

## shared/startup
EXTRA_DIST += shared/startup/bspstart.c shared/startup/pgtbl_setup.c \
    shared/startup/pgtbl_activate.c shared/startup/linkcmds \
	shared/startup/zerobss.c shared/startup/pretaskinghook.c

## shared/tod
EXTRA_DIST += shared/tod/todcfg.c

## shared/vectors
EXTRA_DIST += shared/vectors/vectors_init.c shared/vectors/vectors.S

## shared/vme
EXTRA_DIST += shared/vme/vmeconfig.c shared/vme/VMEConfig.h
	shared/vme/README shared/vme/vme_universe.c shared/vme/vme_universe_dma.c

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
