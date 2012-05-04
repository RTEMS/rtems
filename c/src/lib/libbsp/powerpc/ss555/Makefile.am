ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_HEADERS += ../../shared/include/coverhd.h
include_bsp_HEADERS = irq/irq.h

EXTRA_DIST = times

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# pclock
libbsp_a_SOURCES += clock/p_clock.c
# console
libbsp_a_SOURCES += console/console.c
# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c startup/iss555.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/start.S \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c
# tm27supp
libbsp_a_SOURCES += startup/tm27supp.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/exceptions.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/irq.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/vectors.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
