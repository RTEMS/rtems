##
#
# @file
#
# @brief Makefile of LibBSP for the LPC24XX boards.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h

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
include_bsp_HEADERS += ../shared/lpc/include/lpc-i2s.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-emc.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-dma.h
include_bsp_HEADERS += ../shared/lpc/include/lpc-lcd.h
include_bsp_HEADERS += ../shared/armv7m/include/armv7m-irq.h
include_bsp_HEADERS += include/dma.h
include_bsp_HEADERS += include/i2c.h
include_bsp_HEADERS += include/io.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/lcd.h
include_bsp_HEADERS += include/lpc17xx.h
include_bsp_HEADERS += include/lpc24xx.h
include_bsp_HEADERS += include/lpc-clock-config.h
include_bsp_HEADERS += include/lpc-ethernet-config.h
include_bsp_HEADERS += include/ssp.h
include_bsp_HEADERS += include/start-config.h
include_bsp_HEADERS += include/system-clocks.h

include_HEADERS += ../../shared/include/tm27.h

###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
EXTRA_DIST = 
EXTRA_DIST += startup/linkcmds.lpc17xx_ea_ram
EXTRA_DIST += startup/linkcmds.lpc17xx_ea_rom_int
EXTRA_DIST += startup/linkcmds.lpc17xx_plx800_ram
EXTRA_DIST += startup/linkcmds.lpc17xx_plx800_rom_int
EXTRA_DIST += startup/linkcmds.lpc2362
EXTRA_DIST += startup/linkcmds.lpc23xx_tli800
EXTRA_DIST += startup/linkcmds.lpc24xx_ea
EXTRA_DIST += startup/linkcmds.lpc24xx_ncs_ram
EXTRA_DIST += startup/linkcmds.lpc24xx_ncs_rom_ext
EXTRA_DIST += startup/linkcmds.lpc24xx_ncs_rom_int
EXTRA_DIST += startup/linkcmds.lpc24xx_plx800_ram
EXTRA_DIST += startup/linkcmds.lpc24xx_plx800_rom_int

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
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../../shared/src/stackalloc.c \
	../../shared/src/uart-output-char.c

# Startup
libbsp_a_SOURCES += ../shared/startup/bsp-start-copy-sections.c
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
libbsp_a_SOURCES += irq/irq-dispatch.c

# Console
libbsp_a_SOURCES += ../../shared/console.c \
    console/console-config.c ../../shared/console_select.c \
    ../../shared/console_read.c ../../shared/console_write.c \
    ../../shared/console_control.c

# Clock
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
libbsp_a_SOURCES += ../shared/lpc/clock/lpc-clock-config.c
libbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# Misc
libbsp_a_SOURCES += misc/system-clocks.c \
	misc/dma.c \
	misc/dma-copy.c \
	misc/bspidle.c \
	misc/io.c \
	misc/lcd.c \
	misc/restart.c \
	misc/timer.c

# SSP
libbsp_a_SOURCES += ssp/ssp.c

# I2C
libbsp_a_SOURCES += i2c/i2c.c \
	i2c/i2c-config.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../../libcpu/arm/shared/include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../../../libcpu/arm/shared/include

# Start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += startup/start-config-emc-dynamic.c
libbsp_a_SOURCES += startup/start-config-emc-static.c
libbsp_a_SOURCES += startup/start-config-mpu.c
libbsp_a_SOURCES += startup/start-config-pinsel.c

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
