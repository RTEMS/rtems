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

dist_project_lib_DATA = bsp_specs

###############################################################################
#                  Header                                                     #
###############################################################################

include_HEADERS = include/bsp.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_bsp_HEADERS = ../../shared/include/utility.h \
	../../shared/include/irq-generic.h \
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

dist_project_lib_DATA += ../shared/startup/linkcmds.base startup/linkcmds

###############################################################################
#                  LibBSP                                                     #
###############################################################################
noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# shared
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

# startup
libbsp_a_SOURCES += startup/bspstart.c

# irq
libbsp_a_SOURCES += ../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c \
	../shared/irq/irq_asm.S \
	irq/irq.c

# console
libbsp_a_SOURCES += ../../shared/console.c \
	console/console-config.c

# clock
libbsp_a_SOURCES += clock/clock-config.c

# rtc
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# misc
libbsp_a_SOURCES += misc/system-clocks.c

###############################################################################
#                  Special Rules                                              #
###############################################################################

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
