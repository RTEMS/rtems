ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = \
    ../../shared/bspclean.c \
    ../../shared/bspreset_loop.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    startup/init5225x.c startup/bspstart.c \
    ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += clock/clock.c
libbsp_a_SOURCES += console/console.c
libbsp_a_SOURCES += console/debugio.c
libbsp_a_SOURCES += timer/timer.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5225x/cachepd.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5225x/headers.am
