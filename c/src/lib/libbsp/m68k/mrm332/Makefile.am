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

include_HEADERS += include/mrm332.h
nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = times

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds_ROM

startup_SOURCES = startup/start_c.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bspstart.c startup/bsppredriverhook.c \
    ../../shared/bspclean.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../m68k/shared/setvec.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/gnatinstallhandler.c \
    startup/except_vect_332_ROM.S
clock_SOURCES = clock/ckinit.c
console_SOURCES = console/console.c console/sci.c console/sci.h
spurious_SOURCES = spurious/spinit.c
timer_SOURCES = timer/timer.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(spurious_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
