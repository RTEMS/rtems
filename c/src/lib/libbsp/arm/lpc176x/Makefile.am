##
#
# @file
#
# @brief Makefile of LibBSP for the LPC176x boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs


# ----------------------------
# ------  Headers
# ----------------------------

include_HEADERS = include/bsp.h

nodist_include_HEADERS = ../../shared/include/coverhd.h
nodist_include_HEADERS += include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-timer.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-i2s.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-dma.h
include_bsp_HEADERS += ../shared/armv7m/include/armv7m-irq.h
include_bsp_HEADERS += include/dma.h
include_bsp_HEADERS += include/io-defs.h
include_bsp_HEADERS += include/io.h
include_bsp_HEADERS += include/common-types.h
include_bsp_HEADERS += include/gpio-defs.h
include_bsp_HEADERS += include/gpio.h
include_bsp_HEADERS += include/timer-defs.h
include_bsp_HEADERS += include/timer.h
include_bsp_HEADERS += include/watchdog.h
include_bsp_HEADERS += include/watchdog-defs.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/lpc176x.h
include_bsp_HEADERS += include/lpc-clock-config.h
include_bsp_HEADERS += include/system-clocks.h

include_HEADERS += ../../shared/include/tm27.h


# ----------------------------
# ------  Data
# ----------------------------

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)
project_lib_DATA += startup/linkcmds

EXTRA_DIST =
EXTRA_DIST += startup/linkcmds.lpc1768_mbed
EXTRA_DIST += startup/linkcmds.lpc1768_mbed_ahb_ram
EXTRA_DIST += startup/linkcmds.lpc1768_mbed_ahb_ram_eth


# ----------------------------
# ------  LibBSP
# ----------------------------

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bsplibc.c
libbsp_a_SOURCES += ../../shared/bsppost.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += ../../shared/src/stackalloc.c
libbsp_a_SOURCES += ../../shared/src/uart-output-char.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
libbsp_a_SOURCES += startup/bspreset.c
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
libbsp_a_SOURCES += gpio/gpio.c

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

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
