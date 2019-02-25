##
#
# @file
#
# @brief Makefile of LibBSP for the LPC24XX boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/lpc24xx/start/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc17xx_ea_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc17xx_ea_rom_int
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc17xx_plx800_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc17xx_plx800_rom_int
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc2362
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc23xx_tli800
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_ea
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_ncs_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_ncs_rom_ext
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_ncs_rom_int
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_plx800_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc24xx_plx800_rom_int
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc40xx_ea_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc24xx/start/linkcmds.lpc40xx_ea_rom_int

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/uart-output-char.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bspreset-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/bspreset-armv4.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-dispatch-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/irq/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/irq/irq-dispatch.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/console/uart-probe-1.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/console/uart-probe-2.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/console/uart-probe-3.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-nxp-lpc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-armv7m.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/rtc/rtc-config.c

# Misc
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/system-clocks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/dma-copy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/fb/lcd.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/restart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/timer.c

# SSP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/spi/ssp.c

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/i2c/i2c.c

# Framebuffer
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/fb/arm-pl111.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/fb-config.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/bspstarthooks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/start-config-emc-dynamic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/start-config-emc-static.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/start-config-mpu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc24xx/start/start-config-pinsel.c

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/net/
endif

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/lpc24xx/headers.am
