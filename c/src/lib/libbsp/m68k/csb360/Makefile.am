ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


EXTRA_DIST += times

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c startup/init5272.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# console
libbsp_a_SOURCES += console/console-io.c ../../shared/console-polled.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/../../shared/include

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5272/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5272/timer.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/csb360/headers.am
