##
#
# @file
#
# @brief Makefile of LibBSP for the LPC32XX boards.
#

# $Id$

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp
include_libcpudir = $(includedir)/libcpu

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = ../../shared/include/coverhd.h \
	include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/stackalloc.h
include_bsp_HEADERS += ../../shared/include/uart-output-char.h
include_bsp_HEADERS += ../../shared/tod.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-timer.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-dma.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-i2s.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-emc.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-lcd.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/mmu.h
include_bsp_HEADERS += include/lpc32xx.h
include_bsp_HEADERS += include/lpc-clock-config.h
include_bsp_HEADERS += include/lpc-ethernet-config.h
include_bsp_HEADERS += include/nand-mlc.h
include_bsp_HEADERS += include/boot.h
include_bsp_HEADERS += include/i2c.h
include_bsp_HEADERS += include/emc.h

include_libcpu_HEADERS = ../../../libcpu/arm/shared/include/arm-cp15.h

###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
project_lib_DATA += startup/linkcmds.lpc32xx

EXTRA_DIST = startup/linkcmds.lpc32xx_phycore \
	startup/linkcmds.lpc32xx_mzx_stage_1 \
	startup/linkcmds.lpc32xx_mzx_stage_2 \
	startup/linkcmds.lpc32xx_mzx

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =
libbsp_a_LIBADD =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppredriverhook.c \
	../../shared/bsppretaskinghook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../../shared/src/stackalloc.c \
	../../shared/src/uart-output-char.c \
	../shared/abort/simple_abort.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S

# Startup
libbsp_a_SOURCES += startup/bspstart.c \
	startup/bspreset.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../../shared/src/irq-info.c \
	../../shared/src/irq-shell.c \
	../../shared/src/irq-server.c \
	irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console.c \
        ../../shared/console_select.c \
	console/console-config.c \
	console/hsu.c \
        ../../shared/console_read.c \
        ../../shared/console_write.c \
        ../../shared/console_control.c

# Clock
libbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# Misc
libbsp_a_SOURCES += misc/timer.c \
	misc/nand-mlc.c \
	misc/nand-mlc-read-blocks.c \
	misc/nand-mlc-write-blocks.c \
	misc/nand-mlc-erase-block-safe.c \
	misc/restart.c \
	misc/boot.c \
	misc/emc.c \
	misc/mmu.c \
	misc/i2c.c

# SSP

# I2C

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../../libcpu/arm/shared/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING

noinst_PROGRAMS = network.rel

network_rel_SOURCES = ../shared/lpc/network/lpc-ethernet.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__ -D__BSD_VISIBLE
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

libbsp_a_LIBADD += network.rel

endif

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
