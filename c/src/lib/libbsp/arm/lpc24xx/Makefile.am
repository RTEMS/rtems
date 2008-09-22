##
#
# @file
#
# @brief Makefile of LibBSP for the LPC247X boards.
#

# $Id$

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

libcpudir = ../../../libcpu/@RTEMS_CPU@

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h

nodist_include_HEADERS = include/bspopts.h

include_bsp_HEADERS = ../../shared/include/irq-generic.h \
	../../shared/tod.h \
	../shared/include/linker-symbols.h \
	../shared/include/start.h \
	include/irq-config.h \
	include/irq.h \
	include/lpc24xx.h \
	include/system-clocks.h

###############################################################################
#                  Data                                                       #
###############################################################################

EXTRA_DIST = ../shared/start/start.S

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA = ../shared/startup/linkcmds.base \
	startup/linkcmds \
	bsp_specs

###############################################################################
#                  LibBSP                                                     #
###############################################################################

shared_SOURCES = ../../shared/bootcard.c \
	../../shared/bspclean.c \
	../../shared/bspgetworkarea.c \
	../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bsppredriverhook.c \
	../../shared/bsppretaskinghook.c \
	../../shared/gnatinstallhandler.c \
	../../shared/sbrk.c \
	../shared/abort/simple_abort.c

startup_SOURCES = startup/bspstart.c

irq_SOURCES = ../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../shared/irq/irq_asm.S \
	irq/irq.c

console_SOURCES = ../../shared/console.c \
	console/console-config.c

clock_SOURCES = clock/clock-config.c

rtc_SOURCES = ../../shared/tod.c \
	rtc/rtc-config.c

misc_SOURCES = misc/system-clocks.c

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES = $(clock_SOURCES) \
	$(console_SOURCES) \
	$(irq_SOURCES) \
	$(misc_SOURCES) \
	$(rtc_SOURCES) \
	$(shared_SOURCES) \
	$(startup_SOURCES)

###############################################################################
#                  Network                                                    #
###############################################################################

if HAS_NETWORKING

noinst_PROGRAMS = network.rel

network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

libbsp_a_LIBADD = network.rel

endif

###############################################################################
#                  Special Rules                                              #
###############################################################################

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
