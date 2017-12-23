##
#
# @file
#
# @brief Makefile of LibBSP for the LPC176x boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


# ----------------------------
# ------  Headers
# ----------------------------


# ----------------------------
# ------  Data
# ----------------------------

EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.lpc1768_mbed
dist_project_lib_DATA += startup/linkcmds.lpc1768_mbed_ahb_ram
dist_project_lib_DATA += startup/linkcmds.lpc1768_mbed_ahb_ram_eth

# ----------------------------
# ------  LibBSP
# ----------------------------

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/src/uart-output-char.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
libbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c
libbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
			rtc/rtc-config.c

# GPIO
libbsp_a_SOURCES += gpio/lpc-gpio.c

# CAN
libbsp_a_SOURCES += can/can.c

# PWMOUT
libbsp_a_SOURCES += pwmout/pwmout.c

# ADC
libbsp_a_SOURCES += adc/adc.c

# Timer
libbsp_a_SOURCES += timer/timer.c

# Benchmark Timer
libbsp_a_SOURCES += benchmark_timer/benchmark_timer.c

# Misc
libbsp_a_SOURCES += misc/system-clocks.c
libbsp_a_SOURCES += misc/dma.c
libbsp_a_SOURCES += misc/dma-copy.c
libbsp_a_SOURCES += misc/bspidle.c
libbsp_a_SOURCES += misc/io.c
libbsp_a_SOURCES += misc/restart.c

# Watchdog
libbsp_a_SOURCES += watchdog/watchdog.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../../libcpu/arm/shared/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c


# ----------------------------
# ------  Special Rules
# ----------------------------

DISTCLEANFILES = include/bspopts.h

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/lpc176x/headers.am
