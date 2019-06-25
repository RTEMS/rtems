##
#
# @file
#
# @brief Makefile of LibBSP for the BeagleBoards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/beagle/start/bsp_specs

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
dist_project_lib_DATA += ../../../../../../bsps/arm/beagle/start/linkcmds.beagle

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_LIBADD =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/gpio/gpio-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterfrequency.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/cpucounter/cpucounterread.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/start/bspdebug.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/start/bspstarthooks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/start/bspstartmmu.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/console/console-config.c

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/i2c/bbb-i2c.c

# SPI
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/spi/spi.c

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/gpio/bbb-gpio.c

#pwm
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/pwm/pwm.c

#RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/rtc/rtc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/beagle/clock/clock.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7ar-disable-data.S

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/beagle/headers.am
