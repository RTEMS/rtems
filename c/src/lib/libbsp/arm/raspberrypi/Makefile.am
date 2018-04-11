##
#
# @file
#
# @brief Makefile of LibBSP for the raspberrypi board.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

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

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
# LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/src/stackalloc.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c
if BSP_START_IN_HYP_SUPPORT
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-in-hyp-support.S
endif

# Startup
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/cmdline.c
librtemsbsp_a_SOURCES += startup/bspgetworkarea.c
if HAS_SMP
librtemsbsp_a_SOURCES += startup/bspsmp.c
librtemsbsp_a_SOURCES += startup/bspsmp_init.c
endif

# Restart
librtemsbsp_a_SOURCES += startup/bspreset.c

# IRQ
librtemsbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += console/console-config.c
librtemsbsp_a_SOURCES += console/console_select.c
librtemsbsp_a_SOURCES += console/usart.c
librtemsbsp_a_SOURCES += console/fb.c
librtemsbsp_a_SOURCES += console/fbcons.c
librtemsbsp_a_SOURCES += console/outch.c

# Mailbox
librtemsbsp_a_SOURCES += misc/mailbox.c

# VideoCore
librtemsbsp_a_SOURCES += misc/vc.c

# clock
librtemsbsp_a_SOURCES += clock/clockdrv.c

# Timer
librtemsbsp_a_SOURCES += misc/timer.c

# GPIO
librtemsbsp_a_SOURCES += ../../shared/gpio.c
librtemsbsp_a_SOURCES += gpio/rpi-gpio.c

# RTC

# SSP

# I2C
librtemsbsp_a_SOURCES += i2c/i2c.c

# SPI
librtemsbsp_a_SOURCES += spi/spi.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

# Start hooks
librtemsbsp_a_SOURCES += startup/bspstarthooks.c

# LIBMM
librtemsbsp_a_SOURCES += startup/mm_config_table.c
librtemsbsp_a_SOURCES += ../shared/mminit.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/raspberrypi/headers.am
