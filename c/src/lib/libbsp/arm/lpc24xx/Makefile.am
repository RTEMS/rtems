##
#
# @file
#
# @brief Makefile of LibBSP for the LPC24XX boards.
#

# $Id$

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h

nodist_include_HEADERS = include/bspopts.h

nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/tod.h
include_bsp_HEADERS += ../shared/include/linker-symbols.h
include_bsp_HEADERS += ../shared/include/start.h
include_bsp_HEADERS += include/irq-config.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/lpc24xx.h
include_bsp_HEADERS += include/system-clocks.h
include_bsp_HEADERS += include/ssp.h
include_bsp_HEADERS += include/dma.h
include_HEADERS += ../../shared/include/tm27.h

###############################################################################
#                  Data                                                       #
###############################################################################

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds.base \
	startup/linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################

noinst_LIBRARIES += libbsp.a

libbsp_a_SOURCES =

# Shared
libbsp_a_SOURCES += ../../shared/bootcard.c \
	../../shared/bspclean.c \
	../../shared/bspreset.c \
	../../shared/bspgetworkarea.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppredriverhook.c \
	../../shared/bsppretaskinghook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../shared/abort/simple_abort.c

# Startup
libbsp_a_SOURCES += startup/bspstart.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../shared/irq/irq_asm.S \
	irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console.c \
	console/console-config.c

# Clock
libbsp_a_SOURCES += clock/clock-config.c \
	../../../shared/clockdrv_shell.h

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c
# Timer
libbsp_a_SOURCES += ../../shared/timerstub.c

# Misc
libbsp_a_SOURCES += misc/system-clocks.c \
	misc/dma.c

# SSP
libbsp_a_SOURCES += ssp/ssp.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

DISTCLEANFILES = include/bspopts.h

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
