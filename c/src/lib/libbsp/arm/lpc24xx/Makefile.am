##
#
# @file
#
# @brief Makefile of LibBSP for the LPC24XX boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  Data                                                       #
###############################################################################

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.lpc17xx_ea_ram
dist_project_lib_DATA += startup/linkcmds.lpc17xx_ea_rom_int
dist_project_lib_DATA += startup/linkcmds.lpc17xx_plx800_ram
dist_project_lib_DATA += startup/linkcmds.lpc17xx_plx800_rom_int
dist_project_lib_DATA += startup/linkcmds.lpc2362
dist_project_lib_DATA += startup/linkcmds.lpc23xx_tli800
dist_project_lib_DATA += startup/linkcmds.lpc24xx_ea
dist_project_lib_DATA += startup/linkcmds.lpc24xx_ncs_ram
dist_project_lib_DATA += startup/linkcmds.lpc24xx_ncs_rom_ext
dist_project_lib_DATA += startup/linkcmds.lpc24xx_ncs_rom_int
dist_project_lib_DATA += startup/linkcmds.lpc24xx_plx800_ram
dist_project_lib_DATA += startup/linkcmds.lpc24xx_plx800_rom_int
dist_project_lib_DATA += startup/linkcmds.lpc40xx_ea_ram
dist_project_lib_DATA += startup/linkcmds.lpc40xx_ea_rom_int

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../../shared/bsppredriverhook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../../shared/src/stackalloc.c \
	../../shared/src/uart-output-char.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c

# Startup
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspreset-armv4.c
librtemsbsp_a_SOURCES += startup/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c
librtemsbsp_a_SOURCES += irq/irq.c
librtemsbsp_a_SOURCES += irq/irq-dispatch.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console-config.c
librtemsbsp_a_SOURCES += console/uart-probe-1.c
librtemsbsp_a_SOURCES += console/uart-probe-2.c
librtemsbsp_a_SOURCES += console/uart-probe-3.c

# Clock
librtemsbsp_a_SOURCES += ../../shared/clockdrv_shell.h
librtemsbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c
librtemsbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c

# RTC
librtemsbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# Misc
librtemsbsp_a_SOURCES += misc/system-clocks.c \
	misc/dma.c \
	misc/dma-copy.c \
	misc/bspidle.c \
	misc/io.c \
	misc/lcd.c \
	misc/restart.c \
	misc/timer.c

# SSP
librtemsbsp_a_SOURCES += ssp/ssp.c

# I2C
librtemsbsp_a_SOURCES += i2c/i2c.c \
	i2c/i2c-config.c

# Framebuffer
librtemsbsp_a_SOURCES += ../shared/arm-pl111-fb.c
librtemsbsp_a_SOURCES += startup/fb-config.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
librtemsbsp_a_SOURCES += startup/start-config-emc-dynamic.c
librtemsbsp_a_SOURCES += startup/start-config-emc-static.c
librtemsbsp_a_SOURCES += startup/start-config-mpu.c
librtemsbsp_a_SOURCES += startup/start-config-pinsel.c

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../shared/lpc/network/lpc-ethernet.c
endif

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/lpc24xx/headers.am
