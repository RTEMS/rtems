##
#
# @file
#
# @brief Makefile of LibBSP for the LPC176x boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/lpc176x/start/bsp_specs


# ----------------------------
# ------  Headers
# ----------------------------


# ----------------------------
# ------  Data
# ----------------------------

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc176x/start/linkcmds.lpc1768_mbed
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc176x/start/linkcmds.lpc1768_mbed_ahb_ram
dist_project_lib_DATA += ../../../../../../bsps/arm/lpc176x/start/linkcmds.lpc1768_mbed_ahb_ram_eth

# ----------------------------
# ------  LibBSP
# ----------------------------

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
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-dispatch-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-nxp-lpc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-armv7m.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/rtc/rtc-config.c

# GPIO
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/gpio/lpc-gpio.c

# CAN
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/can/can.c

# PWMOUT
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/pwm/pwmout.c

# ADC
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/adc.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/timer/timer.c

# Benchmark Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/btimer/btimer.c

# Misc
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/system-clocks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/dma-copy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/restart.c

# Watchdog
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/watchdog.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/bspstarthooks.c


# ----------------------------
# ------  Special Rules
# ----------------------------


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/lpc176x/headers.am
