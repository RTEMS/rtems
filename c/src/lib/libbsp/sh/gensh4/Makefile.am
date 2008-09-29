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

include_HEADERS += include/sdram.h
include_HEADERS += include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<

project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.rom \
    startup/linkcmds.rom2ram

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c ../shared/bspstart.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
console_SOURCES = console/console.c ../../shared/dummy_printk_support.c
hw_init_SOURCES = hw_init/hw_init.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(hw_init_SOURCES) $(console_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/sci.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/score.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
