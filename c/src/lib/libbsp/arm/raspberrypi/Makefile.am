##
#
# @file
#
# @brief Makefile of LibBSP for the raspberrypi board.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

###############################################################################
# Header                                                     #
###############################################################################

noinst_HEADERS = console/font_data.h

###############################################################################
# Data                                                       #
###############################################################################

EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
# LibBSP                                                     #
###############################################################################

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c
if BSP_START_IN_HYP_SUPPORT
libbsp_a_SOURCES += ../shared/startup/bsp-start-in-hyp-support.S
endif

# Startup
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/cmdline.c
libbsp_a_SOURCES += startup/bspgetworkarea.c
if HAS_SMP
libbsp_a_SOURCES += startup/bspsmp.c
libbsp_a_SOURCES += startup/bspsmp_init.c
endif

# Restart
libbsp_a_SOURCES += startup/bspreset.c

# IRQ
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console.c
libbsp_a_SOURCES += ../../shared/console_control.c
libbsp_a_SOURCES += ../../shared/console_read.c
libbsp_a_SOURCES += ../../shared/console_write.c
libbsp_a_SOURCES += console/console-config.c
libbsp_a_SOURCES += console/console_select.c
libbsp_a_SOURCES += console/usart.c
libbsp_a_SOURCES += console/fb.c
libbsp_a_SOURCES += console/fbcons.c
libbsp_a_SOURCES += console/outch.c

# Mailbox
libbsp_a_SOURCES += misc/mailbox.c

# VideoCore
libbsp_a_SOURCES += misc/vc.c

# clock
libbsp_a_SOURCES += clock/clockdrv.c ../../../shared/clockdrv_shell.h

# Timer
libbsp_a_SOURCES += misc/timer.c

# GPIO
libbsp_a_SOURCES += ../../shared/gpio.c
libbsp_a_SOURCES += gpio/rpi-gpio.c

# RTC

# SSP

# I2C
libbsp_a_SOURCES += i2c/i2c.c

# SPI
libbsp_a_SOURCES += spi/spi.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

# LIBMM
libbsp_a_SOURCES += startup/mm_config_table.c
libbsp_a_SOURCES += ../shared/mminit.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/raspberrypi/headers.am
