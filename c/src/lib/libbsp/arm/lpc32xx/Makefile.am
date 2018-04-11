##
#
# @file
#
# @brief Makefile of LibBSP for the LPC32XX boards.
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
dist_project_lib_DATA += startup/linkcmds.lpc32xx
dist_project_lib_DATA += startup/linkcmds.lpc32xx_phycore
dist_project_lib_DATA += startup/linkcmds.lpc32xx_mzx_stage_1
dist_project_lib_DATA += startup/linkcmds.lpc32xx_mzx_stage_2
dist_project_lib_DATA += startup/linkcmds.lpc32xx_mzx

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/src/stackalloc.c
librtemsbsp_a_SOURCES += ../../shared/src/uart-output-char.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console-config.c
librtemsbsp_a_SOURCES += console/hsu.c

# Clock
librtemsbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c

# RTC
librtemsbsp_a_SOURCES += ../../shared/tod.c
librtemsbsp_a_SOURCES += rtc/rtc-config.c

# Misc
librtemsbsp_a_SOURCES += misc/boot.c
librtemsbsp_a_SOURCES += misc/emc.c
librtemsbsp_a_SOURCES += misc/i2c.c
librtemsbsp_a_SOURCES += misc/nand-mlc.c
librtemsbsp_a_SOURCES += misc/nand-mlc-erase-block-safe.c
librtemsbsp_a_SOURCES += misc/nand-mlc-read-blocks.c
librtemsbsp_a_SOURCES += misc/nand-mlc-write-blocks.c
librtemsbsp_a_SOURCES += misc/restart.c
librtemsbsp_a_SOURCES += misc/system-clocks.c
librtemsbsp_a_SOURCES += misc/timer.c
librtemsbsp_a_SOURCES += misc/nand-select.c
librtemsbsp_a_SOURCES += misc/idle-thread.c

# SSP

# I2C

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../shared/lpc/network/lpc-ethernet.c
endif

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/lpc32xx/headers.am
