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

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/src/uart-output-char.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c

# Startup
libbsp_a_SOURCES += startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/hsu.c

# Clock
libbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# Misc
libbsp_a_SOURCES += misc/boot.c
libbsp_a_SOURCES += misc/emc.c
libbsp_a_SOURCES += misc/i2c.c
libbsp_a_SOURCES += misc/nand-mlc.c
libbsp_a_SOURCES += misc/nand-mlc-erase-block-safe.c
libbsp_a_SOURCES += misc/nand-mlc-read-blocks.c
libbsp_a_SOURCES += misc/nand-mlc-write-blocks.c
libbsp_a_SOURCES += misc/restart.c
libbsp_a_SOURCES += misc/system-clocks.c
libbsp_a_SOURCES += misc/timer.c
libbsp_a_SOURCES += misc/nand-select.c
libbsp_a_SOURCES += misc/idle-thread.c

# SSP

# I2C

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING
libbsp_a_SOURCES += ../shared/lpc/network/lpc-ethernet.c
endif

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/lpc32xx/headers.am
