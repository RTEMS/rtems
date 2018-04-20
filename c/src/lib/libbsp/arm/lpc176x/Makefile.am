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
librtemsbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/bspstart.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-nxp-lpc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-armv7m.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += rtc/rtc-config.c

# GPIO
librtemsbsp_a_SOURCES += gpio/lpc-gpio.c

# CAN
librtemsbsp_a_SOURCES += can/can.c

# PWMOUT
librtemsbsp_a_SOURCES += pwmout/pwmout.c

# ADC
librtemsbsp_a_SOURCES += adc/adc.c

# Timer
librtemsbsp_a_SOURCES += timer/timer.c

# Benchmark Timer
librtemsbsp_a_SOURCES += benchmark_timer/benchmark_timer.c

# Misc
librtemsbsp_a_SOURCES += misc/system-clocks.c
librtemsbsp_a_SOURCES += misc/dma.c
librtemsbsp_a_SOURCES += misc/dma-copy.c
librtemsbsp_a_SOURCES += misc/bspidle.c
librtemsbsp_a_SOURCES += misc/io.c
librtemsbsp_a_SOURCES += misc/restart.c

# Watchdog
librtemsbsp_a_SOURCES += watchdog/watchdog.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Start hooks
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/lpc176x/start/bspstarthooks.c


# ----------------------------
# ------  Special Rules
# ----------------------------


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/lpc176x/headers.am
