##
## $Id$
##

# FIXME: Herein lies madness ;)

## include
EXTRA_DIST = include/nvram.h include/bsp.h

## bootloader
EXTRA_DIST += bootloader/em86.c bootloader/lib.c bootloader/misc.c \
    bootloader/mm.c bootloader/pci.c bootloader/zlib.c
EXTRA_DIST += bootloader/bootldr.h bootloader/zlib.h bootloader/pci.h
EXTRA_DIST += bootloader/em86real.S bootloader/exception.S \
    bootloader/head.S
EXTRA_DIST += bootloader/ppcboot.lds

## clock
EXTRA_DIST += clock/p_clock.c

## console
EXTRA_DIST += console/console.c console/inch.c console/polled_io.c \
    console/uart.c console/reboot.c console/console.inl

## motorola
EXTRA_DIST += motorola/motorola.c

## pci
EXTRA_DIST += pci/pci.c pci/detect_raven_bridge.c pci/pcifinddevice.c

## residual
EXTRA_DIST += residual/residual.c

## openpic
EXTRA_DIST += openpic/openpic.c

## irq
EXTRA_DIST += irq/i8259.c irq/irq.c irq/irq_init.c irq/irq_asm.S

## start
EXTRA_DIST += start/start.S start/rtems_crti.S

## startup
EXTRA_DIST += startup/bspstart.c startup/pgtbl_setup.c \
    startup/pgtbl_activate.c
EXTRA_DIST += startup/linkcmds

## vectors
EXTRA_DIST += vectors/vectors_init.c vectors/vectors.S

## vme
EXTRA_DIST += vme/vmeconfig.c vme/VMEConfig.h vme/VME.h

include $(top_srcdir)/../../../automake/local.am
