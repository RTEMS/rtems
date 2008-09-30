##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

include_HEADERS += include/coverhd.h
include_HEADERS += include/m340uart.h
include_HEADERS += include/m340timer.h
include_HEADERS += include/m68340.h
include_HEADERS += include/m68340.inc
include_HEADERS += include/m68349.inc

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bsppost.c startup/bspstart.c startup/init68340.c \
    ../../shared/bootcard.c ../../m68k/shared/m68kpretaskinghook.c \
    ../../shared/sbrk.c ../../m68k/shared/setvec.c \
    startup/dumpanic.c ../../shared/gnatinstallhandler.c
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/Modif_cpu_asm.S console/console.c \
    console/m340uart.c ../../shared/dummy_printk_support.c
timer_SOURCES = timer/timer.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
