##
#
# @file
#
# @brief Makefile of LibBSP for the raspberrypi board.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
# Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
    include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

noinst_HEADERS = console/font_data.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../../libbsp/shared/include/mm.h
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../../shared/include/gpio.h
include_bsp_HEADERS += ../shared/include/linker-symbols.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/include/arm-cp15-start.h
include_bsp_HEADERS += ../shared/include/arm-errata.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-timer.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-dma.h
include_bsp_HEADERS += ../shared/include/arm-release-id.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/mmu.h
include_bsp_HEADERS += include/usart.h
include_bsp_HEADERS += include/raspberrypi.h
include_bsp_HEADERS += include/rpi-gpio.h
include_bsp_HEADERS += include/i2c.h
include_bsp_HEADERS += include/spi.h
include_bsp_HEADERS += include/mailbox.h
include_bsp_HEADERS += include/vc.h
include_bsp_HEADERS += include/rpi-fb.h
include_bsp_HEADERS += console/fbcons.h

include_libcpu_HEADERS =  ../../../libcpu/arm/shared/include/arm-cp15.h

###############################################################################
# Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += ../shared/startup/linkcmds.base

EXTRA_DIST = startup/linkcmds

###############################################################################
# LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c
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
libbsp_a_SOURCES += ../../shared/bspreset_loop.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += startup/cmdline.c
libbsp_a_SOURCES += startup/bspgetworkarea.c

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
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

# LIBMM
libbsp_a_SOURCES += startup/mm_config_table.c
libbsp_a_SOURCES += ../shared/mminit.c

###############################################################################
# Network                                                    #
###############################################################################

#if HAS_NETWORKING
#
#noinst_PROGRAMS = network.rel
#
#network_rel_SOURCES = ../shared/lpc/network/lpc-ethernet.c
#network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__ -D__BSD_VISIBLE
#network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
#
#
#libbsp_a_LIBADD += network.rel
#
#endif

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
