##
#
# @file
#
# @brief Makefile of LibBSP for the raspberrypi board.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/raspberrypi/start/bsp_specs

###############################################################################
# Header                                                     #
###############################################################################

noinst_HEADERS = ../../../../../../bsps/arm/raspberrypi/console/font_data.h

###############################################################################
# Data                                                       #
###############################################################################

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

###############################################################################
# LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterfrequency.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterread.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/cmdline.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspgetworkarea.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspsmp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspsmp_init.c
endif

# Restart
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspreset.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/console-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/console_select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/usart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/fb.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/fbcons.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/console/outch.c

# Mailbox
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/mailbox.c

# VideoCore
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/vc.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/arm/raspberrypi/clock/clockdrv.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/timer.c

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/gpio/gpio-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/gpio/rpi-gpio.c

# RTC

# SSP

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/i2c/i2c.c

# SPI
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/spi/spi.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7ar-disable-data.S

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/bspstarthooks.c

# LIBMM
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/raspberrypi/start/mm_config_table.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/raspberrypi/headers.am
