##
#
# @file
#
# @brief Makefile of LibBSP for the LPC32XX boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/lpc32xx/start/bsp_specs

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
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc32xx/start/linkcmds.lpc32xx
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc32xx/start/linkcmds.lpc32xx_phycore
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc32xx/start/linkcmds.lpc32xx_mzx_stage_1
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc32xx/start/linkcmds.lpc32xx_mzx_stage_2
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc32xx/start/linkcmds.lpc32xx_mzx

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
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/console/hsu.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-nxp-lpc.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/rtc/rtc-config.c

# Misc
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/boot.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/emc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/i2c/i2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/nand/nand-mlc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/nand/nand-mlc-erase-block-safe.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/nand/nand-mlc-read-blocks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/nand/nand-mlc-write-blocks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/restart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/system-clocks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/nand/nand-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/bspidle.c

# SSP

# I2C

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc32xx/start/bspstarthooks.c

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
include $(srcdir)/../../../../../../bsps/arm/lpc32xx/headers.am
