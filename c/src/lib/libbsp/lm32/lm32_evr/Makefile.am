##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h
include_HEADERS += include/system_conf.h

EXTRA_DIST = ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../shared/startup/bspstart.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bootcard.c ../../shared/sbrk.c ../shared/startup/setvec.c \
    ../../shared/gnatinstallhandler.c
clock_SOURCES = ../shared/clock/ckinit.c
console_SOURCES = ../shared/console/console.c ../shared/console/uart.c
network_SOURCES = ../shared/tsmac/tsmac.c
timer_SOURCES = ../shared/timer/timer.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(network_SOURCES) $(timer_SOURCES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
