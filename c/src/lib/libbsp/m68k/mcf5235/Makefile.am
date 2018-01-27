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
dist_project_lib_DATA += startup/linkcmds
dist_project_lib_DATA += startup/linkcmdsflash
dist_project_lib_DATA += startup/linkcmdsram

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsppredriverhook.c \
    ../../shared/bspreset_loop.c \
    startup/bspgetcpuclockspeed.c \
    ../../shared/bspgetworkarea.c startup/init5235.c startup/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c \
    startup/copyvectors.c
# clock
libbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console.c
# timer
libbsp_a_SOURCES += timer/timer.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
endif

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/mcf5235/cachepd.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/misc.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5235/headers.am
