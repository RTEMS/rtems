##
## $Id$
##

# FIXME: Herein lies madness ;)

if need_shared
include_bspdir = $(includedir)/bsp

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

include_bsp_HEADERS = console/consoleIo.h console/keyboard.h console/uart.h

## motorola
EXTRA_DIST += motorola.c

include_bsp_HEADERS += motorola.h

## pci
EXTRA_DIST += pci/pci.c pci/detect_raven_bridge.c pci/pcifinddevice.c

include_bsp_HEADERS += pci/pci.h

## residual
EXTRA_DIST += residual/residual.c

include_bsp_HEADERS += residual/pnp.h residual/residual.h

## openpic
EXTRA_DIST += openpic/openpic.c

include_bsp_HEADERS += openpic/openpic.h

## irq
EXTRA_DIST += irq/i8259.c irq/irq.c irq/irq_init.c irq/irq_asm.S

include_bsp_HEADERS += irq/irq.h

## start
EXTRA_DIST += start/start.S start/rtems_crti.S

## startup
EXTRA_DIST += startup/bspstart.c startup/pgtbl_setup.c \
    startup/pgtbl_activate.c
EXTRA_DIST += startup/linkcmds

## vectors
EXTRA_DIST += vectors/vectors_init.c vectors/vectors.S

include_bsp_HEADERS += vectors.h

## vme
# FIXME: This doesn't make much sense here
EXTRA_DIST += vmeconfig.c VMEConfig.h VME.h
endif

PREINSTALL_DIRS =
PREINSTALL_FILES =

if need_shared
$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/consoleIo.h: console/consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/keyboard.h: console/keyboard.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/keyboard.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/keyboard.h

$(PROJECT_INCLUDE)/bsp/uart.h: console/uart.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

$(PROJECT_INCLUDE)/bsp/motorola.h: motorola.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/motorola.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

$(PROJECT_INCLUDE)/bsp/pci.h: pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

$(PROJECT_INCLUDE)/bsp/pnp.h: residual/pnp.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pnp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pnp.h

$(PROJECT_INCLUDE)/bsp/residual.h: residual/residual.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/residual.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/residual.h

$(PROJECT_INCLUDE)/bsp/openpic.h: openpic/openpic.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/openpic.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/openpic.h

$(PROJECT_INCLUDE)/bsp/irq.h: irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/vectors.h: vectors.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vectors.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vectors.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
