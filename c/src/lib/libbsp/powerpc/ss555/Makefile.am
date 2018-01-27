ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

EXTRA_DIST += times

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# pclock
libbsp_a_SOURCES += clock/p_clock.c
# console
libbsp_a_SOURCES += console/console.c
# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c startup/iss555.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c start/start.S \
    ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# tm27supp
libbsp_a_SOURCES += startup/tm27supp.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/exceptions.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/irq.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/vectors.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/ss555/headers.am
